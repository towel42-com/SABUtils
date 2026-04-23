#!/usr/bin/bash

Usage() {
    echo "buildAllConfigs.sh: --outdir <dir> --logfile <filename> --json <filename> --cmakeutilsdir <directory> --start <n> --end <n> --build --cmake --verbose --debug --parallel --single --forceqt [config1 config2...] "
    echo "    Run all configurations"
    echo "  --outdir <dir> : Output directory (default ${OUT_DIR})"
    echo "       --logfile : The output log file for all the runs (default buildAllConfigs.log)"
    echo "          --json : The output json file for all the runs (default buildAllConfigs.json)"
    echo "         --cmake : Run the cmake stage (may get overridden if required by the build stage) (default ${RUN_CMAKE})"
    echo "         --build : Run the build stage (default ${RUN_BUILD})"
    echo "       --verbose : Give extended debugging information (default ${VERBOSE})"
    echo "         --debug : Runs in debug mode (only runs the first 3 configurations, overrides --end) (default ${DEBUG})"
    echo "      --parallel : Runs in configurations in parallel (default ${PARALLEL})"
    echo "       --forceqt : Enable forcing qt as a secondary config (default ${FORCE_QT})"
    echo "        --single : Runs single configurations only, no two configurations are on at the same time (default ${SINGLE})"
    echo "       --dry-run : Runs in dry-run (ignored if --parallel not set) (default ${DRY_RUN})"
    echo " --cmakeutilsdir : The directory for the Towel 42 CMake Utilities (default ${T42_CMAKEUTILS_DIR})"
    echo "         --start : Start with configuration number (default 0)"
    echo "           --end : End at this configuration number (default MAX)"
    echo "         configN : The list of configurations to run (default run all)"
    echo ""
    echo "     boolean options (build, cmake, verbose etc can be turned off via --no<option>"
    echo ""
    echo "     -h|--help   : Displays this message"
}

processArgs() {
    while [[ $# -gt 0 ]]; do
        local arg="$1"
        case $arg in 
            --outdir)
                shift
                if [[ $# -eq 0 || -z "$1" || "$1" == -* ]]; then
                    echo "Error: --outdir requires a non-empty directory argument"
                    Usage
                    exit 1
                fi
                OUT_DIR="$1"
                shift
            ;;
            --logfile)
                shift
                if [[ $# -eq 0 || -z "$1" || "$1" == -* ]]; then
                    echo "Error: --logfile requires a non-empty filename argument"
                    Usage
                    exit 1
                fi
                if [[ "$1" == */* ]]; then
                    log_dir="$(dirname "$1")"
                    if [[ ! -d "$log_dir" ]]; then
                        echo "Error: invalid log file path '$1'"
                        exit 1
                    fi
                fi
                LOG_FILE="$1"

                shift
            ;;
            --json)
                shift
                if [[ $# -eq 0 || -z "$1" || "$1" == -* ]]; then
                    echo "Error: --json requires a non-empty filename argument"
                    Usage
                    exit 1
                fi
                if [[ "$1" == */* ]]; then
                    json_dir="$(dirname "$1")"
                    if [[ ! -d "$json_dir" ]]; then
                        echo "Error: invalid json file path '$1'"
                        exit 1
                    fi
                fi
                JSON_FILE="$1"
                shift
            ;;
            --cmakeutilsdir)
                shift
                if [[ $# -eq 0 || -z "$1" || "$1" == -* ]]; then
                    echo "Error: --cmakeutilsdir requires a non-empty filename argument"
                    Usage
                    exit 1
                fi
                if [[ "$1" == */* ]]; then
                    if [[ ! -d "$1" ]]; then
                        echo "Error: invalid CMake Utilities  directory '$1'"
                        exit 1
                    fi
                fi
                T42_CMAKEUTILS_DIR=$(cygpath -m $(realpath "$1"))
                shift
            ;;
            --start)
                shift
                START=$1
                shift
            ;;
            --end)
                shift
                END=$1
                shift
            ;;
            --build)
                RUN_BUILD=1
                shift
            ;;
            --nobuild|--no-build)
                RUN_BUILD=0
                shift
            ;;
            --forceqt)
                FORCE_QT=1
                shift
            ;;
            --noforceqt|--no-forceqt)
                FORCE_QT=0
                shift
            ;;
            --cmake)
                RUN_CMAKE=1
                shift
            ;;
            --nocmake|--no-cmake)
                RUN_CMAKE=0
                shift
            ;;
            --debug)
                DEBUG=1
                END=2
                shift
            ;;
            --nodebug|--no-debug)
                DEBUG=0
                shift
            ;;
            --parallel)
                PARALLEL=1
                shift
            ;;
            --noparallel|--no-parallel)
                PARALLEL=0
                shift
            ;;
            --single)
                SINGLE=1
                shift
            ;;
            --nosingle|--no-single)
                SINGLE=0
                shift
            ;;
            --dry-run)
                DRY_RUN=1
                shift
            ;;
            --nodry-run|--no-dry-run|--nodryrun|--no-dryrun)
                DRY_RUN=0
                shift
            ;;
            --verbose)
                VERBOSE=1
                shift
            ;;
            --noverbose|--no-verbose)
                VERBOSE=0
                shift
            ;;
            -h*|--help)
                Usage
                exit 0
            ;;
            --*)
                printf "\e[31mERROR: Unknown switch '%s'\e[0m\n" "${arg}" 2>&1
                Usage
                exit 1
            ;;
            *)
                CONFIGS_TO_RUN[$1]="$1"
                shift
            ;;
        esac
    done
}

generateSequences() {
    local numConfigs=$1

    if [[ ${END} -eq -1 ]]; then
        END=$(( $numCombinations - 1 ))
    fi

    sequence=($(seq $START $END))
    if [[ ${SINGLE} == 1 ]]; then
        sequence=(0)
        local value=1
        local ii
        for (( ii=0; ii<$(($numConfigs - 1)); ii++ )); do
            if [[ "${#sequence[@]}" -ge "${END}" ]]; then
                break
            fi
            sequence+=($value)
            value=$(( $value << 1))
        done
    fi

    qt_sequence=(ON OFF)
    if [[ ${FORCE_QT} == 0 ]]; then
        qt_sequence=(OFF)
    fi
    numConfigsBeingRun=$((${#sequence[@]} * ${#qt_sequence[@]}))
}

validateVariables() {
    techo "\nValidating global variables are not being ignored\n"
        
    declare -a ignoreArray=()
    declare -A ignoreMap=()
    readarray -t ignoreArray <<< "$PARALLEL_IGNORED_NAMES"
    for val in "${ignoreArray[@]}"; do
        if [[ "$val" == "" ]]; then
            continue
        fi
        ignoreMap["$val"]=1
    done

    for val in "${PASS_VARS[@]}"; do
        printf -v currPassVarOpt "%s %s" "--env" $val
        PASS_VARS_OPT+=(${currPassVarOpt})

        local isIgnored=0
        if [[ -v ignoreMap[$val] ]]; then
            isIgnored=1
        fi
        
         if [[ $isIgnored == 1 ]]; then
            errorVars+=("$val")
        fi
    done

    if [[ ${#errorVars[@]} -gt 0 ]]; then
        for var in "${errorVars[@]}"; do
            terror "ERROR: Required variable '$var' is currently being ignored\n"
        done
        terror "    Declare the variable after the call to 'env_parallel --session'\n"
        exit -1
    fi
    techo "Finished validating global variables\n"

    if [[ ${VERBOSE} == 1 ]]; then
        readarray -t allCurr < <(compgen -A function -v)
        declare -a passedVars=()
        for val in "${allCurr[@]}"; do
            if [[ -v ignoreMap[$val] ]]; then
                continue
            fi
            
            if local -p "$val" &>/dev/null; then
                continue
            fi
              
            passedVars+=("$val")
        done
        
        techo "    \nThe following variables are not in the ignore variables set:\n"
        local totalSize=0
        for val in "${passedVars[@]}"; do
            local sz=$(getObjectSize "$val")
            techo "        $val - size: $sz\n"
            totalSize=$(( $totalSize + $sz ))
        done

        techo "    Total size of variables=$totalSize\n"
    fi
}

dumpHex() {
    str="$1"

    echo "================"
    echo "$str"
    echo $str | od -An -tx1
    echo "================"
}
    
showGlobalHeader() {
    local numConfigs=$((${#CONFIGS[@]} + 1)) 
    local numCombinations=$(( (1 << ${#CONFIGS[@]}) ))

    if [[ ${FORCE_QT} == 1 ]]; then
        numCombinations=$((${numCombinations} * 2))
    fi
    declare -a headerInfo=()
    
    local curr=0
    headerInfo[$((curr++))]="Total number of configs;${numConfigs} (includes all config variables disabled)"

    headerInfo[$curr]="Run with force qt as extra flow"
    if [[ $FORCE_QT == 1 ]]; then
        headerInfo[$((curr++))]+=";Yes (doubles the number of runs)"
    else
        headerInfo[$((curr++))]+=";No"
    fi

    headerInfo[$curr]="Run with only one configuration enabled"
    if [[ $SINGLE == 1 ]]; then
        headerInfo[$((curr++))]+=";Yes"
    else
        headerInfo[$((curr++))]+=";No"
    fi

    headerInfo[$((curr++))]="Maximum number of combinations;${numCombinations}"
    headerInfo[$((curr++))]="Results Logfile;${LOG_FILE};1"
    headerInfo[$((curr++))]="Results JSON;${JSON_FILE};1"
    headerInfo[$((curr++))]="BASH Shell Wrapper;${BASH_SHELL_WRAPPER};1"

    generateSequences $numConfigs

    headerInfo[$((curr++))]="Number of (unfiltered) configurations to run;${numConfigsBeingRun}"
    headerInfo[$((curr++))]="Number of filters;${#CONFIGS_TO_RUN[@]}"
    echo "${CONFIGS_TO_RUN[@]}"
    for val in "${CONFIGS_TO_RUN[@]}"; do
        headerInfo[$((curr++))]=";${val}"
    done

    local maxLen=0
    for currHeaderInfo in "${headerInfo[@]}"; do
        declare -a arr
        IFS=";" read -r -a arr <<< "${currHeaderInfo}"
        local currLen=${#arr[0]}
        if [[ $currLen -gt $maxLen ]]; then
            maxLen=$currLen
        fi
    done
    maxLen=$((maxLen + 1))

    for currHeaderInfo in "${headerInfo[@]}"; do
        IFS=";" read -r -a arr <<< "${currHeaderInfo}"
    
    
        value="${arr[1]}"
        if [[ ${#arr[@]} == 3 && ${arr[2]} == 1 ]]; then
            value=$(getOSC8Url "${value}" "" 0)
        fi
        
        printf "%${maxLen}s : %s\n" "${arr[0]}" "${value}"
    done
}

. /usr/local/bin/env_parallel.bash

#declared before the session as they arent used in the run itself
#. buildAllConfigs-utils.sh

configsRun=()
globalPassed=()
globalSkipped=()
globalFailed=()
numCombinations=0
START=0
END=-1
SINGLE=0
value=""
val=""
currHeaderInfo=""
currPassVarOpt=""
declare -a errorVars=()

PASS_VARS=(
    CONFIGS
    CONFIGS_TO_RUN
    JSON_FILE
    LOG_FILE
    OUT_DIR
    PARALLEL
    RUN_BUILD
    RUN_CMAKE
    T42_CMAKEUTILS_DIR
    VERBOSE
    qt_sequence
    sequence
    runConfig_Impl
)

PASS_VARS_OPT=""
DEBUG=0
#headerLine
#set
#headerLine
BASH_SHELL_WRAPPER=bash.sh

env_parallel --session

getGlobalCount() {
    echo $(cat "${GLOBAL_COUNT_FILE}" | wc -l)
}

incGlobalCount() {
    echo "$1" >> "${GLOBAL_COUNT_FILE}"
}

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
    if [[ -f "${JSON_FILE}" ]]; then
        mv "${JSON_FILE}" "${JSON_FILE}.bak"
    fi
    
    local args=(--indent 4 -n '[inputs[]]')

    local configs=()
    while IFS= read -r line; do
        configs+=("$line")
    done < "${GLOBAL_COUNT_FILE}"

    for config in "${configs[@]}"; do
        args+=("${OUT_DIR}/${config}/results.json")
    done    

    waitForLock createGlobalResults
    techo "Joining existing config files into new summary json file ${JSON_FILE}\n"
    jq "${args[@]}" > ${JSON_FILE} # retval comes from jq
    status=$?
    removeLock createGlobalResults
    return $?
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
    local passed=()
    local skipped=()
    local failed=()

    createGlobalResults

    if [[ ! -f ${JSON_FILE} ]]; then
        terror "========================================\n"
        terror "Could not find JSON file '${JSON_FILE}', confirm confurations were run"
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
        
    if [[ ${failedCount} != 0 && ${failedCount} != "" ]] ; then
        terror "Failed Configurations:\n"
        headerLine
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

    count=$(getGlobalCount)
    percent=$(( (100*${count}) / ${numConfigsBeingRun} ))
    techo "\e[0;32m[%d of %d (%d%%)]\e[0m - Configuration #%d QT=%s" ${count} ${numConfigsBeingRun} ${percent} $configNum ${forceQt}
    if [[ ${PARALLEL} == 0 ]]; then
        local failedFormat="\e[31m%d\e[0m"
        if [[ ${#globalFailed[@]} == 0 ]]; then
            failedFormat="%d"
        fi
        techo " - Passed: %d Failed: $failedFormat Previously Skipped: %d\n" ${#globalPassed[@]} ${#globalFailed[@]} ${#globalSkipped[@]}
    else
        techo " - %s\n" ${configName}
    fi
    
    local varNames=()
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
    
    incGlobalCount "${configName}"

    local status=0
    printConfigHeader $configNum $configName $forceQt $localLogFile $localJSONFile
    if [[ ${skipConfig} == 1 ]]; then 
        globalSkipped+=($configName);
        reportConfigFooter -1 "$configName"
        
        if [[ ! -d ${OUT_DIR}/${configName} ]]; then
            mkdir -p ${OUT_DIR}/${configName}
        fi
        jq -n --arg configName "$configName" '[ { "configName":$configName, "status":0, "skipped":1, "setup_status":"skipped", "cmake_status":"skipped", "build_status":"skipped" } ]' > ${localJSONFile}
    else
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
            jq -n --arg configName "$configName" --arg status "$status" --arg setup_status "${SETUP_STATUS}" --arg cmake_status "${CMAKE_STATUS}" --arg build_status "${BUILD_STATUS}" \
                '[ { "configName":$configName, "status":$status, "skipped":0, "setup_status":$setup_status, "cmake_status":$cmake_status, "build_status":$build_status } ]' > ${localJSONFile}
        else
            if [[ ${RUN_CMAKE} == 1 ]]; then
                jq ".[0].status = ${status} | .[0].cmake_status = \"${CMAKE_STATUS}\"" ${localJSONFile} | sponge  ${localJSONFile}
            fi
            if [[ ${RUN_BUILD} == 1 ]]; then
                jq ".[0].status = ${status} | .[0].build_status = \"${BUILD_STATUS}\"" ${localJSONFile} | sponge  ${localJSONFile}
            fi
        fi
        reportConfigFooter ${status} "$configName"
    fi
    return 0
}

runConfig() {
    local configNum=$1
    local forceQt=$2

    #. ./buildAllConfigs-utils.sh 

    runConfig_Impl "$configNum" "$forceQt"
}

numConfigsBeingRun=0

#variables used inside parallel
OUT_DIR=all_build_configs
LOG_FILE=${OUT_DIR}/buildAllConfigs.log
JSON_FILE=${OUT_DIR}/buildAllConfigs.json

if [[ ! -d ${OUT_DIR} ]]; then
    mkdir -p ${OUT_DIR}
fi

RUN_BUILD=1
RUN_CMAKE=1
VERBOSE=0
PARALLEL=1
T42_CMAKEUTILS_DIR=$(cygpath -m $(realpath ../T42-CMakeUtils/))
declare -A CONFIGS_TO_RUN=()
declare -a CONFIGS

CONFIGS=(
    "TOWEL42_BIFSUPPORT"
    "TOWEL42_GIFSUPPORT"
    "TOWEL42_DESIGNERPLUGIN_SUPPORT"
    "TOWEL42_ZIP_SUPPORT"
    "TOWEL42_ENABLE_TESTING"
    "TOWEL42_VSINSTALLER_SUPPORT"
    "TOWEL42_QCONCURRENT_SUPPORT"
    "TOWEL42_QCORE_SUPPORT"
    "TOWEL42_QNETWORK_SUPPORT"
    "TOWEL42_QSQL_SUPPORT"
    "TOWEL42_QSVG_SUPPORT"
    "TOWEL42_QXML_SUPPORT"
    "TOWEL42_QWIDGETS_SUPPORT"
    )
#remove accidental duplicates from configs
readarray -t CONFIGS < <(printf '%s\n' "${CONFIGS[@]}" | sort)

processArgs "$@"

rm -rf buildAllConfigs.lockfile

if [[ -f ${LOG_FILE} ]]; then
    mv ${LOG_FILE} ${LOG_FILE}.bak
fi

if [[ -f ${JSON_FILE} ]]; then
    mv ${JSON_FILE} ${JSON_FILE}.bak
fi

techoVerbose "Creating bash shell wrapper: $BASH_SHELL_WRAPPER\n"
BASH_SHELL_WRAPPER=$(realpath ./bash.sh)
printf "/usr/bin/bash.exe \"\$@\"\n" > $BASH_SHELL_WRAPPER
export PARALLEL_SHELL=$BASH_SHELL_WRAPPER

GLOBAL_COUNT_FILE=${OUT_DIR}/configsRunFile.txt
rm -rf "${GLOBAL_COUNT_FILE}"

touch "${JSON_FILE}"

showGlobalHeader

if [[ ${PARALLEL} == 1 ]]; then 
    PASS_VARS_OPT=()
    
    validateVariables
    
    dryRunOpt=""
    if [[ ${DRY_RUN} == 1 ]]; then
        dryRunOpt=--dry-run
    fi
    PASS_VARS_OPT=${PASS_VARS_OPT[@]}
    # echo PASS_VARS_OPT=${PASS_VARS_OPT}
    numParallel=-j+0
    env_parallel \
        --eta \
        ${dryRunOpt} \
        ${numParallel} \
        --env _ \
        runConfig "{1}" "{2}" ::: "${sequence[@]}" ::: "${qt_sequence[@]}"
else
    for ii in "${sequence[@]}"; do
        for forceQt in "${qt_sequence[@]}"; do
            runConfig $ii ${forceQt}
        done
    done
fi

reportSummary

rm -rf ${BASH_SHELL_WRAPPER}