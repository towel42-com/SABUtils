#!/usr/bin/bash

techo() {
    printf "$@" | tee -a ${LOG_FILE}
}

techoVerbose() {
    if [[ ${VERBOSE} == 1 ]]; then
        techo "$@"
    fi
}

terror() {
    printf -v tmp "$@"
    printf "\e[31m%s\e[0m" "${tmp}" | tee -a ${LOG_FILE} 2>&1
}

getObjectSize() {
    local target=$1
    
    #echo "target=${target} - value = ${#target}"
    
    # Check if it is a defined function
    local size=0
    if declare -f "$target" > /dev/null; then
        # declare -f prints the function definition; wc -c counts the bytes
        local size=$(declare -f "$target" | wc -c)
    elif declare -p "$target" 2>/dev/null | grep -q 'declare -[aA]'; then
        # ${#var} gets character count; printf | wc -c gets byte count
        eval "size=\$(printf '%s' \"\${$target[@]}\" | wc -c)"
    elif [[ -n ${!target+x} ]]; then
        size=$(printf '%s' "${!target}" | wc -c)
    else
        return 1
    fi
    echo $size
    return 0
}

printStack() {
    asError=$1
    
    local cmd=techo
    if [[ ${asError} == 1 ]]; then
        cmd=terror
    fi
    local ii=0
    local stackSize=${#FUNCNAME[@]}
    ${cmd} "Stack trace (most recent call first):\n"
    # Start from 1 to skip the 'print_stack' function itself
    for (( ii=1; ii < stackSize; ii++ )); do
        local func="${FUNCNAME[$ii]:-(top level)}"
        local file="${BASH_SOURCE[$ii]:-(no file)}"
        local line="${BASH_LINENO[$((ii-1))]}"
        ${cmd} "  at $func() in $file:$line\n"
    done
}

printAllValues() {
    local arrayName=$1
    local -n array=$1
    local space=$2
    
    if [[ -z "$space" ]]; then
        space=${#arrayName}
    fi

    techo "%${space}s: " $arrayName
    if [[ ${#array[@]} == 0 ]]; then
        techo "<EMPTY>\n"
    else
        techo "#%d\n" ${#array[@]}
        space=$(($space + 2))
        for val in "${array[@]}"; do
            techo "%${space}s%s\n" "" $val
        done
    fi
}

printAllValuesVerbose() {
    if [[ ${VERBOSE} == 1 ]]; then
        return 0
    fi
    printAllValues "$@"
}

blankLine() {
    techo "\n"
}

headerLine() {
    techo "========================================\n"
}

getConfigName() {
    local configNum=$1
    local forceQt=$2
    
    if [[ -z ${forceQt} ]]; then
        terror "forceQt not set\n"
        printStack 1
    fi
    
    printf "build_config_%04d_QT_%s" $configNum $forceQt
}

getConfigJSONFileName() {
    local configNum=$1
    local forceQt=$2
    
    configName=$(getConfigName $configNum $forceQt)
    echo "${OUT_DIR}/${configName}/results.json"
}

waitForLock() {
    #echo waitForLock $1
    while [[ -f buildAllConfigs.lockfile ]]; do
        sleep .05
    done
    touch buildAllConfigs.lockfile    
}

removeLock() {
    #echo removeLock $1
    rm -rf buildAllConfigs.lockfile
}


reportConfigFooter() {
    local status=$1
    local config="$2"
    
    slnxURL=$(getOSC8Url "${OUT_DIR}/$config/Towel42Utils.slnx" "Solution")
    logURL=$(getOSC8Url "${OUT_DIR}/$config/${config}.log" "Log File")
    jsonURL=$(getOSC8Url "${OUT_DIR}/$config/results.json" "JSON File")
    
    local statusText="PASSED"
    local color="\e[0;"
    
    if [[ $status == -1 ]]; then
        statusText="SKIPPED"
        color="\e[34m"
    elif [[ $status != 0 ]]; then
        statusText="FAILED"
        color="\e[31m"
    fi
    printf -v statusText "${color}%s\e[0m" "${statusText}"
    
    if [[ $status == -1 ]]; then
        techo "    %s: %s\n" "${statusText}" "$config"
    else
        headerLine
        techo "    %s: %s - %s - %s - %s\n" "${statusText}" "$config" "${slnxURL}" "${logURL}" "${jsonURL}"
    fi
}

addJSONResults() {
    local configNum=$1
    local forceQt=$2
    
    local configJsonFile=$(getConfigJSONFileName $configNum $forceQt)
    if [[ ! -f ${configJsonFile} ]]; then
        return 1
    fi

    if [[ ! -f ${JSON_FILE} ]]; then
        touch ${JSON_FILE}
    fi
    
    #techo "Adding results from $configName\n"
    waitForLock addJSONResults  
    jq -n '[inputs[]]' ${JSON_FILE} ${configJsonFile} | sponge ${JSON_FILE}
    removeLock addJSONResults
    
    return 0
}
   
updateGlobalResults() {
    local currConfigNum=$1
    local forceQt=$2

    local configJsonFile=$(getConfigJSONFileName $configNum $forceQt)    
    if [[ ! -f ${configJsonFile} ]]; then
        terror "Could not find JSON file '${configJsonFile}'"
        continue;
    fi

    addJSONResults $configNum $forceQt
    return 0
}

createGlobalResults() {
    local START=$1
    local END=$2
    local JSON_FILE=$3
    
    if [[ -z $JSON_FILE ]]; then
        JSON_FILE=buildAllConfigs.json
    fi
    
    if [[ -f "${JSON_FILE}" ]]; then
        mv "${JSON_FILE}" "${JSON_FILE}.bak"
    fi
    
    if [[ -z $START ]]; then
        START=$(ls -d1 ${OUT_DIR}/build_config_* | sort -n | head -n 1 | sed -e 's/${OUT_DIR}\/build_config_//')
    fi    
        
    if [[ -z $END ]]; then
        END=$(ls -d1 ${OUT_DIR}/build_config_* | sort -n | tail -n 1 | sed -e 's/${OUT_DIR}\/build_config_//')
    fi    

    techo "Finding existing results files: START=$START END=$END\n"
    args=(--indent 4 -n '[inputs[]]')
    files=($(eval ls ${OUT_DIR}/build_config_{$START..$END}/results.json))
    args+=("${files[@]}")
    
    waitForLock createGlobalResults
    techo "Joining existing config files into new summary json file ${JSON_FILE}\n"
    jq "${args[@]}" > ${JSON_FILE} # retval comes from jq
    removeLock createGlobalResults
}

getOSC8Url() { 
    local path=$1
    local desc="$2"
    local showRed=1
    if [[ -v $3 ]]; then
        showRed=$3
    fi
   
    abspath=$(cygpath -m $(pwd)/"$path")

    local url=$(printf "file:///%s" "$abspath")
    local url=$(printf "file:///%s" "$abspath")
    
    if [[ "$desc" == "" ]]; then
        desc="$path"
    fi

    #echo ""
    #echo -n "${url}" | od -An -tx1
    printf -v osc8 "\e]8;;%s\e\\%s\e]8;;\e\\" "$url" "${desc}"
    if [[ -f $path || ( $showRed == 0 ) ]]; then
        printf "\e[0m%s\e[0m" "${osc8}"
    else
        printf "\e[31m%s\e[0m" "${osc8}"
    fi
}

reportSummary() {
    JSON_FILE=$1
    if [[ -z $JSON_FILE ]]; then
        JSON_FILE=${OUT_DIR}/buildAllConfigs.json
    fi

    local passed=()
    local skipped=()
    local failed=()
    if [[ ! -f ${JSON_FILE} ]]; then
        terror "========================================\n"
        terror "Could not find JSON file '${JSON_FILE}', confirm confurations ran or rerun createGlobalResults"
        return 1
    fi
    
    waitForLock reportSummary

    local count=$(jq length ${JSON_FILE})
    local skippedCompletely=$(jq '[.[] | select(.setup_status == "skipped" and .cmake_status == "skipped" and .build_status == "skipped")] | length' ${JSON_FILE})
    local incomplete=$(jq '[.[] | select(.setup_status == "skipped" or .cmake_status == "skipped" or .build_status == "skipped")] | length' ${JSON_FILE})
    local passed=$(jq '[.[] | select( (.setup_status == "skipped" and .cmake_status != "failed" and .build_status != "failed" ) or (.setup_status != "failed" and .cmake_status == "skipped" and .build_status != "failed" ) or (.setup_status != "failed" and .cmake_status != "failed" and .build_status == "skipped" ))] | length' ${JSON_FILE})
    local failedCount=$(jq '[.[] | select(.setup_status == "failed" or .cmake_status == "failed" or .build_status == "failed")] | length' ${JSON_FILE})
    local failedStr=$(jq '.[] | select(.setup_status == "failed" or .cmake_status == "failed" or .build_status == "failed") | .configName' ${JSON_FILE} | sed s/\"//g)
    local failed=($failedStr)
    
    removeLock reportSummary
    
    local logFileURL=$(getOSC8Url "${LOG_FILE}")
    local jsonFileURL=$(getOSC8Url "${JSON_FILE}")
    
    techo "===========================================\n"
    techo "Summary:\n"
    techo "    Configurations Run: ${count}\n"
    techo "              Log file: %s\n" ${logFileURL}
    techo "     Results JSON file: %s\n" ${jsonFileURL}
    techo "    Skipped Completely: ${skippedCompletely}\n"
    techo "            Incomplete: ${incomplete}\n"
    techo "                Passed: ${passed}\n"
    techo "                Failed: ${failedCount}\n"
        
    if [[ ${failedCount} != 0 ]] ; then
        terror "Failed Configurations:\n"
        for config in "${failed[@]}"; do
            reportConfigFooter 1 $config
        done
    fi

}

SETUP_STATUS=skipped
CMAKE_STATUS=skipped
BUILD_STATUS=skipped

printConfigHeader() {
    local configNum=$1
    local configName=$2
    local forceQt=$3
    local localLogFile=$4
    local localJSONFile=$5
    
    if [[ ${PARALLEL} == 1 ]]; then
        blankLine
    fi
    headerLine

    local varNames=(configName)
    if [[ ${PARALLEL} == 0 ]]; then
        local failedFormat="\e[31m%d\e[0m"
        if [[ ${#globalFailed[@]} == 0 ]]; then
            failedFormat="%d"
        fi
        techo "\e[0;32m[%d of %d]\e[0m - Configuration #%d QT=%s - Passed: %d Failed: $failedFormat Previously Skipped: %d\n" ${globalCount} ${numConfigsBeingRun} $configNum ${forceQt} ${#globalPassed[@]} ${#globalFailed[@]} ${#globalSkipped[@]}
        varNames=()
    fi
    
    if [[ ${VERBOSE} == 1 ]]; then 
        varNames=(configNum configName forceQt localLogFile localJSONFile LOG_FILE RUN_BUILD RUN_CMAKE CONFIGS CONFIGS_TO_RUN)
    fi
    
    local size=0
    for var in "${varNames[@]}"; do
        currLength=${#var}
        if [[ "$currLength" -gt "$size" ]]; then
            size=$currLength
        fi
    done
        
    for var in "${varNames[@]}"; do
        local def=$(declare -p "$var" 2>/dev/null)
        if [[ "$def" =~ "declare -a" || "$def" =~ "declare -A" ]]; then
            printAllValues ${var} ${size}
        else
            techo "%${size}s: %s\n" $var ${!var}
        fi
    done
}

getOptions() {
    local configNum=$1
    local forceQt=$2
   
    configs=()
    for (( idx=${#CONFIGS[@]}-1 ; idx>=0 ; idx-- )) ; do
        local currConfig
        local onOff
        local shiftValue=$(( ($configNum >> ${idx}) ))
        if [[ $shiftValue == 1 ]]; then
            onOff=ON
        else
            onOff=OFF
        fi
        
        printf -v currConfig "%s%s=%s" "-D" ${CONFIGS[idx]} ${onOff}
       
        configs+=("$currConfig")
    done
    printf -v currConfig "%s%s=%s" "-D" TOWEL42_FORCE_QT_FOR_CONFIG_TESTING ${forceQt}
    configs+=("$currConfig")

    echo "${configs[@]}"
}

reportResult() {
    declare -n varRef=$1
    local status=$2
    local extraLogFile=$3
    local extraSpace=$4
    
    local spacing="        "
    if [[ ! -z "${extraSpace}" ]]; then
        spacing="            "
    fi
    if [[ $status == 0 ]]; then
        varRef=passed
        techo "${spacing}PASSED\n"
        if [[ ! -z "${extraLogFile}" ]]; then
            echo "${spacing}PASSED" >> ${extraLogFile}
        fi
        return 0
    else
        varRef=failed
        techo "${spacing}FAILED\n"
        if [[ ! -z "${extraLogFile}" ]]; then
            headerLine
            techo "Failed Messages from command:\n\n"
            tail -n 30 ${extraLogFile} | tee -a ${LOG_FILE}
            echo "${spacing}FAILED" >> ${extraLogFile}
            headerLine
        fi
        return 1
    fi
}

setupConfigBuildArea() {
    local configName=$1
    
    # if you are running cmake, always start from a clean directory
    # if you are building without cmake, use the existing one
    # if it doesnt exist, create if building
    # if it exists and you are not running cmake or build, DO NOT delete it
    local _mkdir=0
    if [[ ${RUN_CMAKE} == 1 || ( ! -d ${OUT_DIR}/${configName} && ${RUN_BUILD} == 1 ) ]]; then
        _mkdir=1
    fi

    SETUP_STATUS=skipped
    if [[ ${_mkdir} == 1 ]]; then
        if [[ -d ${OUT_DIR}/${configName} ]]; then
            techo "    Removing existing directory\n"
            rm -rf ${configName} |& tee -a ${LOG_FILE}
            status=${PIPESTATUS[0]}
            reportResult SETUP_STATUS $status
            if [[ $status != 0 ]]; then
                return $status
            fi
        fi
        
        techo "    Creating directory ${OUT_DIR}/$configName\n"
        mkdir -p ${OUT_DIR}/$configName |& tee -a ${LOG_FILE}
        status=${PIPESTATUS[0]}
        reportResult SETUP_STATUS $status
        if [[ $status != 0 ]]; then
            return $status
        fi
    fi
    return 0
}

runCMake() {
    local configName=$1
    local configNum=$2
    local forceQt=$3
    
    local _forceRunCMake=0
    if [[ ${RUN_BUILD} == 1 ]]; then
        if [[ ${RUN_CMAKE} == 0 ]]; then
            if [[ ! -f ${OUT_DIR}/${configName}/CMakeCache.txt ]]; then
                _forceRunCMake=1
            fi
        fi
    fi

    options=$(getOptions $configNum $forceQt)
    
    if [[ ${_forceRunCMake} == 1 || ${RUN_CMAKE} == 1 ]]; then
        techo "    Running CMake\n"
        local devWarning=-Wno-dev
        outFiles=("$localLogFile")
        if [[ ${VERBOSE} == 1 ]]; then
            devWarning=-Wdev
            outFiles+=("${LOG_FILE}")
            outFiles+=("/dev/stdout")
        fi
       
        for outFile in "${outFiles[@]}"; do
            echo "=====================================" &>>  ${outFile}
            echo "        CMD: cmake -S . -B ${OUT_DIR}/${configName} ${devWarning} -DTOWEL42_CMAKEUTILS_DIR=${T42_CMAKEUTILS_DIR} $options" &>> ${outFile}
            echo "=====================================" &>>  ${outFile}
        done
        cmake -S . -B ${OUT_DIR}/${configName} ${devWarning} -DTOWEL42_CMAKEUTILS_DIR=${T42_CMAKEUTILS_DIR} $options |& tee -a ${LOG_FILE} >> ${localLogFile}
        status=${PIPESTATUS[0]}
        reportResult CMAKE_STATUS $status ${localLogFile}
        return $status
    fi
    return 0
}


runBuild() {
    local configName=$1
    
    BUILD_STATUS=skipped
    local allBuildsStatus=0
    if [[ ${RUN_BUILD} == 1 ]]; then
        echo "    Running all build configurations" | tee -a ${LOG_FILE} ${localLogFile}

        for config in Debug Release RelWithDebInfo MinSizeRel; do
            echo "        Running Build-$config" | tee -a ${LOG_FILE} ${localLogFile}

            local args=()
            args+=(\"${OUT_DIR}/${configName}/ALL_BUILD.vcxproj\")
            args+=(\"--t:Clean,Build\")
            
            if [[ ${PARALLEL} == 1 ]]; then
                args+=(\"--m:1\")
                args+=(\"--p:CL_MPCount=1\")
            fi
            args+=(\"--p:Configuration=${config}\")
            
            local msbuild="C:/Program Files/Microsoft Visual Studio/18/Enterprise/MSBuild/Current/Bin/amd64/MSBuild.exe"
            local argFile=${OUT_DIR}/${configName}/args-${config}.txt
            echo "${args[@]}" > $argFile
            
            if [[ ${VERBOSE} == 1 ]]; then
                echo "=====================================" | tee -a ${LOG_FILE} ${localLogFile}
                echo "               CMD: $msbuild \"@${argFile}\"" | tee -a ${LOG_FILE} ${localLogFile}
                echo "  argfile contents: ${args[@]}" | tee -a ${LOG_FILE} ${localLogFile}
                echo "=====================================" | tee -a ${LOG_FILE} ${localLogFile}
            fi
            
            "$msbuild" @"${argFile}" |& tee -a ${LOG_FILE} >> ${localLogFile}
            status=${PIPESTATUS[0]}
            reportResult BUILD_STATUS $status ${localLogFile} 1

            if [[ $status != 0 ]]; then
                allBuildsStatus=1
                break
            fi
            
        done
        echo "    All build Configuration Status" | tee -a ${LOG_FILE} ${localLogFile}
        reportResult BUILD_STATUS $allBuildsStatus ${localLogFile}
        return $allBuildsStatus
    fi
    return 0
}


runConfig_Impl() {
    local configNum=$1
    local forceQt=$2
    
    local configName=$(getConfigName $configNum $forceQt)

    declare -g localLogFile=${OUT_DIR}/${configName}/${configName}.log
    local localJSONFile=$(getConfigJSONFileName $configNum $forceQt)

    local skipConfig=0
    if [[ "${#CONFIGS_TO_RUN[@]}" -gt 0 && ! -v CONFIGS_TO_RUN["${configName}"] ]]; then #requires bash 4.2+
        skipConfig=1
    fi

    local currStatus="configuration \"$configName\""
    globalCount=$(( $globalCount + 1 ))

    printConfigHeader $configNum $configName $forceQt $localLogFile $localJSONFile
    if [[ ${skipConfig} == 1 ]]; then 
        globalSkipped+=($configName);
        reportConfigFooter -1 "$configName"
        return 0
    fi
    headerLine
    techo "Running $currStatus\n"
    
    setupConfigBuildArea $configName
    status=$?
    
    if [[ ${status} == 0 ]]; then
        runCMake $configName $configNum $forceQt
        status=$?
        if [[ ${status} == 0 ]]; then
            runBuild $configName
            status=$?
        fi
    fi

    if [[ ${status} == 0 ]]; then
        globalPassed+=($configName)
    else
        globalFailed+=($configName)
    fi

    if [[ ! -f ${localJSONFile} ]]; then
        if [[ ${RUN_CMAKE} == 1 || ${RUN_BUILD} == 1 ]]; then
            jq -n --arg configName "$configName" --arg status "$status" --arg setup_status "${SETUP_STATUS}" --arg cmake_status "${CMAKE_STATUS}" --arg build_status "${BUILD_STATUS}" \
                '[ { "configName":$configName, "status":$status, "setup_status":$setup_status, "cmake_status":$cmake_status, "build_status":$build_status } ]' > ${localJSONFile}
        fi
    else
        if [[ ${RUN_CMAKE} == 1 ]]; then
            jq ".[0].status = ${status} | .[0].cmake_status = \"${CMAKE_STATUS}\"" ${localJSONFile} | sponge  ${localJSONFile}
        fi
        if [[ ${RUN_CMAKE} == 1 ]]; then
            jq ".[0].status = ${status} | .[0].build_status = \"${BUILD_STATUS}\"" ${localJSONFile} | sponge  ${localJSONFile}
        fi
    fi
    
    updateGlobalResults $configNum $forceQt
    
    reportConfigFooter ${status} "$configName"
    return 0
}
    

