#!/usr/bin/bash

#set -o nounset

getYesNo() {
    if [[ $1 == 1 ]]; then
        echo "ON"
    else
        echo "OFF"
    fi
}

# make more generic and move into bin
# add --configs option to list all cmake options
# add --cmake_options (multiple) to add the T42-CMakeUtils dir
# add --boolean_option (multiple) for force qt and force DLL

Usage() {
    echo "buildAllConfigs.sh: --outdir <dir> --logfile <filename> --json <filename> --cmakeutilsdir <directory> --start <n> --end <n> --num-config-jobs <n> --build --cmake --verbose --debug --parallel --all_combinations --forceqt --dll --summary --validate_env [config1 config2...] [-- ...] "
    echo "    Run all configurations"
    echo "     --outdir <dir> : Output directory (default ${OUT_DIR})"
    echo "          --logfile : The output logfile for all the runs (default buildAllConfigs.log)"
    echo "             --json : The output json file for all the runs (default buildAllConfigs.json)"
    echo "            --cmake : Run the cmake stage (may get overridden if required by the build stage) (default $(getYesNo ${RUN_CMAKE}))"
    echo "            --build : Run the build stage (default $(getYesNo ${RUN_BUILD}))"
    echo "          --verbose : Give extended debugging information (default $(getYesNo ${VERBOSE}))"
    echo "            --debug : Runs in debug mode (only runs the first 3 configurations, overrides --end, only runs the RelWithDebInfo build type) (default $(getYesNo ${DEBUG}))"
    echo "         --parallel : Runs in configurations in parallel (default $(getYesNo ${T42_PARALLEL}))"
    echo "          --forceqt : Enable forcing qt as a secondary config (default $(getYesNo ${FORCE_QT}))"
    echo "              --dll : Run for both DLL and non-DLL builds (default $(getYesNo ${FORCE_DLL}))"
    echo " --all_combinations : Runs all combinations of configuration options, Otherwise only run 1 configuration option at a time (default $(getYesNo ${ALL_COMBINATIONS}))"
    echo "    --cmakeutilsdir : The directory for the Towel 42 CMake Utilities (default ${T42_CMAKEUTILS_DIR})"
    echo "            --start : Start with configuration number (default 0)"
    echo "              --end : End at this configuration number (default ALL)"
    echo "  --num-config-jobs : Maximum number of configuration jobs to run (default ALL)"
    echo "          --summary : For the given output directory, rebuild the summary files and report (default $(getYesNo ${SUMMARY_ONLY}))"
    echo "     --validate_env : Run a thorough validation of the environment before running parallel (default $(getYesNo ${VALIDATE_ENV}))"
    echo "            configN : The list of configurations to run (default run all)"
    echo ""
    echo "     boolean options (build, cmake, verbose etc can be turned off via --no<option>"
    echo ""
    echo "     -- is the standard end of options flag.  Anything on the command line after this is passed directly to parallel.  Note, this is ignored when --noparallel is set"
    echo "        For instance, running a \"dry run\" is built into parallel, so simply adding -- --dry-run will turn this feature on"
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
                        echo "Error: invalid logfile path '$1'"
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
            --num-config-jobs)
                shift
                NUM_CONFIGS=$1
                shift
            ;;
            --summary)
                SUMMARY_ONLY=1
                shift
            ;;
            --no-summary|--nosummary)
                SUMMARY_ONLY=0
                shift
            ;;
            --validate)
                VALIDATE_ENV=1
                shift
            ;;
            --no-validate|--novalidated)
                VALIDATE_ENV=0
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
            --dll)
                FORCE_DLL=1
                shift
            ;;
            --nodll|--no-dll)
                FORCE_DLL=0
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
                T42_DEBUG=1
                shift
            ;;
            --nodebug|--no-debug)
                T42_DEBUG=0
                shift
            ;;
            --parallel)
                T42_PARALLEL=1
                shift
            ;;
            --noparallel|--no-parallel)
                T42_PARALLEL=0
                shift
            ;;
            --all_combinations)
                ALL_COMBINATIONS=1
                shift
            ;;
            --noall_combinations|--no-all_combinations)
                ALL_COMBINATIONS=0
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
            --)
                shift
                REMAINDER_ARGS=("$@")
                set --
                ;;
            -h*|--help)
                Usage
                exit 0
            ;;
            -*)
                printColoredText RED "ERROR: Unknown switch \'"${arg}"\'\n" 2>&1
                Usage
                exit 1
            ;;
            *)
                CONFIGS_TO_RUN+=("$1")
                shift
            ;;
        esac
    done
}

strTrim() {
    local var=$1
    var="${var##*([[:space:]])}"
    var="${var%%*([[:space:]])}"
    
    echo $var
}
    
getConfigNameFromJobLog() {
    local runName=$(echo $1 | sed "s#runConfig ##g" | sed "s#'##g")
    readarray -d ";" -t array <<< "$runName"

    local retVal=$(strTrim $(getConfigName ${array[1]} ${array[2]} ${array[3]}))
    echo $retVal
}

determineConfigsToRun() {
    local -n retVal=$1
    
    local count=0
    local finished=0
    for configNum in "${config_sequence[@]}"; do
        for forceQt in "${qt_sequence[@]}"; do
            for forceDLL in "${dll_sequence[@]}"; do
                local curr="${count};${configNum};${forceQt};${forceDLL}"
                retVal+=("$curr")
                count=$(($count + 1))
            done
        done
    done
    if [[ ${#retVal[@]} -ge ${NUM_CONFIGS_BEING_RUN} ]]; then
        retVal=(${retVal[@]:0:${NUM_CONFIGS_BEING_RUN}})
    fi
}

generateSequences() {
    local numConfigs=$1

    if [[ ${END} -eq -1 ]]; then
        END=$(( $numCombinations - 1 ))
    fi
    
    config_sequence=($(seq $START $END))
    if [[ ${ALL_COMBINATIONS} == 0 ]]; then
        config_sequence=(0)
        local value=1
        local ii=0
        for (( ii=0; ii<$(($numConfigs - 1)); ii++ )); do
            if [[ "${#config_sequence[@]}" -ge "${END}" ]]; then
                break
            fi
            config_sequence+=($value)
            value=$(( $value << 1))
        done
    fi

    qt_sequence=(OFF ON)
    if [[ ${FORCE_QT} == 0 ]]; then
        qt_sequence=(OFF)
    fi

    dll_sequence=(OFF ON)
    if [[ ${FORCE_DLL} == 0 ]]; then
        dll_sequence=(OFF)
    fi

    NUM_CONFIGS_BEING_RUN=$((${#config_sequence[@]} * ${#qt_sequence[@]} * ${#dll_sequence[@]}))
    if [[ ${T42_DEBUG} == 1 ]]; then 
        local initialSize=${NUM_CONFIGS_BEING_RUN}
        NUM_CONFIGS_BEING_RUN=4
        techo "INFO: Reducing configurations for debug mode: From $initialSize to ${NUM_CONFIGS_BEING_RUN}\n"
    fi

    if [[ ${NUM_CONFIGS} != -1 ]]; then
        if [[ ${NUM_CONFIGS} -lt ${NUM_CONFIGS_BEING_RUN} ]]; then
            NUM_CONFIGS_BEING_RUN=${NUM_CONFIGS}
        else
            techo "INFO: requested number of config jobs via --num-config-jobs is greater than the normal number of config jobs\n"
        fi
    fi
}

byteSize() {
    local sz=$1
    local retVal=$(numfmt --to=iec $sz)
    retVal+="b"
    echo $retVal
}

validateObjectSizes() {
    local -n retVal=$1
    local correctNumber=$2
    local objType=$3
    local title=$4
    local -n array=$5
    local -n text=$6

    local statusOK=0
    if [[ ${#array[@]} -ne ${correctNumber} ]]; then
        terror "${title} has ${#array[@]} objects and should have ${correctNumber}. Please validate the script\n"
        statusOK=1
    fi

    if [[ ${#array[@]} == 0 ]]; then
        text=""
        return $statusOK
    fi
    
    local size=0
    printf -v text "    %s: %d\n" "${title}" "${#array[@]}"
    for objName in "${array[@]}"; do
        local sz=$(getObjectSizeKnownType "$objName" "$objType")
        local szText=$(byteSize $sz)

        printf -v text "%s        %s - %s\n" "${text}" "$objName" "$szText"
        size=$(( $size + $sz ))
    done
    printf -v text "%s    Total size of %s - %s\n" "${text}" "${title}" "$(byteSize $size)"
    retVal+=$size

    return $statusOK
}

startStatus() {
    local -a msg=$1
    statusStack+=("$msg")
    techo "%s" "${msg}"
}
    
validateEnvSize() {
    local -n varNames=$1
    techo "Getting the size of the %d exported global variables/functions\n" "${#varNames[@]}"
   
    startStatus "    Determining the type of exported object"
    local -a functions=()
    local -a arrays=()
    local -a variables=()
    local -a unknowns=()
    local count=0
    for varName in "${varNames[@]}"; do
        local objType=$(getObjectType "${varName}")
        if [[ "${objType}" == "function" ]]; then
            functions+=("$varName")
        elif [[ "${objType}" == "array" ]]; then
            arrays+=("$varName")
        elif [[ "${objType}" == "variable" ]]; then
            variables+=("$varName")
        else
            unknowns+=("$varName")
        fi
        ((count++))
        if [[ $((${count} % 5)) == 0 ]]; then
            techo "."
        fi
    done
    techo "\n"
    
    functions=($(printf '%s\n' "${functions[@]}" | sort))
    variables=($(printf '%s\n' "${variables[@]}" | sort))
    arrays=($(printf '%s\n' "${arrays[@]}" | sort))
    unknowns=($(printf '%s\n' "${unknowns[@]}" | sort))

    techo "        Finished determining the type of exported object\n"

    techo "    The following are not in the ignore variables dataset, and will be passed into the parallel sub-process:\n"

    local currText=""
    
    local -i totalSize=0

    validateObjectSizes totalSize 38 "function" "Functions" functions currText
    local -i status=$?
    techo "%s" "${currText}" 
    if [[ ${status} != 0 ]]; then
        return 1
    fi

    validateObjectSizes totalSize 18 "variable" "Variables" variables currText
    local -i status=$?
    techo "%s" "${currText}" 
    if [[ ${status} != 0 ]]; then
        return 1
    fi

    validateObjectSizes totalSize 5 "array" "Arrays" arrays currText 
    local -i status=$?
    techo "%s" "${currText}" 
    if [[ ${status} != 0 ]]; then
        return 1
    fi

    validateObjectSizes totalSize 0 "unknown" "Unknown Types:" unknowns currText
    local -i status=$?
    techo "%s" "${currText}" 
    if [[ ${status} != 0 ]]; then
        return 1
    fi
    
    local colorName="GREEN"
    local max=128000
    local totalSize10x=$(($totalSize * 10))
    percent=$(( (100*${totalSize}) / ${max} ))
    if [[ $totalSize10x -gt $((9*$max)) ]]; then
        colorName="RED"
    elif [[ $totalSize10x -gt $((8*$max)) ]]; then
        colorName="YELLOW"
    fi
    printf -v totalSizeText "%s of %s (%d%%)" "$(byteSize $totalSize)" "$(byteSize $max)" ${percent}
    techo "%s\n    Total size of objects = %s%s%s\n%s\n" "$(headerLineText)" "$(printColorCode "$colorName")" "$totalSizeText" "$(printEndColor)" "$(headerLineText)"
}

validateEnvironment() {
    headerLine
    techo "Validating global variables/functions are not being ignored\n"
        
    declare -a ignoreArray=()
    declare -a errorVars=()
    declare -A ignoreMap=()
    readarray -t ignoreArray <<< "$PARALLEL_IGNORED_NAMES"
    for val in "${ignoreArray[@]}"; do
        if [[ "$val" == "" ]]; then
            continue
        fi
        ignoreMap["$val"]=1
    done

    for val in "${PASS_VARS[@]}"; do
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
    techo "    Finished validating global variables/functions\n"

    readarray -t allCurr < <(compgen -A function -v)
    declare -a passedVars=()
    techo "Exporting the necessary functions\n"
    for val in "${allCurr[@]}"; do
        if [[ -v ignoreMap[$val] ]]; then
            continue
        fi
        
        if local -p "$val" &>/dev/null; then
            continue
        fi
          
        passedVars+=("$val")
        if declare -F "${val}" > /dev/null; then
            eval export -f $val
        else
            eval export $val
        fi
    done
    
    techo "    Finished exporting the necessary functions\n"
            
    if [[ ${VALIDATE_ENV} == 1 ]]; then
        validateEnvSize passedVars
        if [[ $? != 0 ]]; then
            return 1
        fi
    fi
}

getMaxReportDescLen() {
    local -n lclHeaderInfo=$1
    local skipBy=$2
    local maxLen=0
    local ii=0
    for ((ii=0; ii<${#lclHeaderInfo[@]}; ii+=${skipBy})); do
        #echo "|${lclHeaderInfo[ii]}|" 1>&2
        if [[ ( "${lclHeaderInfo[ii]}" =~ ^=+$ ) || ( "${lclHeaderInfo[ii+1]}" == "" ) ]]; then
            continue
        fi
        local currLen=${#lclHeaderInfo[ii]}
        if [[ $currLen -gt $maxLen ]]; then
            maxLen=$currLen
        fi
    done
    maxLen=$((maxLen + 1))
    echo $maxLen
}

reportTableInfo() {
    local -n infoArray=$1
    local retVal=""
    local maxLen=$(getMaxReportDescLen infoArray 2)
    local ii=0

    for ((ii=0; ii<"${#infoArray[@]}"; ii+=2)); do
        local header="${infoArray[ii]}"
        local value="${infoArray[ii+1]}"
        if [[ ( "${header}" =~ ^=+$ ) || ( "${value}" == "" ) ]]; then
            printf -v retVal "%s%s\n" "$retVal" "${header}"
            continue
        fi
       
        local -a tmp=()
        readarray -d $';' -t tmp <<<"$value"
        
        local isFile=0
        if [[ ${#tmp[@]} == 3 ]]; then
            value=${tmp[0]}
            local desc=${tmp[1]}
            isFile=$(strTrim ${tmp[2]})
        fi
#        echo "header = |${header}| value = |${value}| desc=|"${desc}"| isFile=|${isFile}|"

        if [[ ${isFile} == 1 ]]; then
            value=$(getOSC8Url "${value}" "${desc}" 0)
        fi
        
        printf -v retVal "%s%${maxLen}s : %s\n" "$retVal" "${header}" "${value}"
    done
    techo "${retVal}"
   
}

reportSummary() {
    local numSeconds=$1
    local passed=()
    local skipped=()
    local failed=()

    createGlobalResults
    results=$?
    if [[ ${results} != 0 ]]; then
        return 1
    fi

    if [[ ! -f ${JSON_FILE} ]]; then
        terror "========================================\n"
        terror "Could not find JSON file '${JSON_FILE}', confirm confurations were run"
        return 1
    fi
    
    waitForLock reportSummary

    local count=$(jq length ${JSON_FILE})

    local skippedCompletely=$(jq '[.[] | select(.setup_status == "skipped" and .cmake_status == "skipped" and .build_status == "skipped")] | length' ${JSON_FILE})

    local incomplete=$(jq '[.[] | select(.setup_status == "skipped" or .cmake_status == "skipped" or .build_status == "skipped")] | length' ${JSON_FILE})

    local passed=$(jq '[.[] | select( (.setup_status == "passed" and .cmake_status != "failed" and .build_status != "failed" ) or (.setup_status != "failed" and .cmake_status == "passed" and .build_status != "failed" ) or (.setup_status != "failed" and .cmake_status != "failed" and .build_status == "passed" ))] | length' ${JSON_FILE})

    local failedCount=$(jq '[.[] | select(.setup_status == "failed" or .cmake_status == "failed" or .build_status == "failed")] | length' ${JSON_FILE})
    local failedStr=$(jq '.[] | select(.setup_status == "failed" or .cmake_status == "failed" or .build_status == "failed") | .configName' ${JSON_FILE} | sed s/\"//g)

    local totalRunTime=$(jq '. | map(.runtime) | add' ${JSON_FILE} | sed s/\"//g)
    local averagePerConfig=$(echo "scale=3; $totalRunTime/$count" | bc)
    local failed=($failedStr)
    
    removeLock reportSummary
    
    local -a summaryInfo=()
    local curr=0
    summaryInfo[$((curr++))]="===========================================" 
    summaryInfo[$((curr++))]=""
    summaryInfo[$((curr++))]="Summary:"
    summaryInfo[$((curr++))]=""
    summaryInfo[$((curr++))]="Configurations run"
    summaryInfo[$((curr++))]=${count}
    summaryInfo[$((curr++))]="Total CPU Run Time"
    summaryInfo[$((curr++))]=$(printf "%.3f seconds" "${totalRunTime}")
    summaryInfo[$((curr++))]="Avg CPU Run Time/Config"
    summaryInfo[$((curr++))]=$(printf "%.3f seconds"  "${averagePerConfig}")
    
    if [[ ${SUMMARY_ONLY} == 0 ]]; then 
        summaryInfo[$((curr++))]="Total Clock Run Time"
        summaryInfo[$((curr++))]=$(printf "%.3f seconds"  "${numSeconds}")
        
        averagePerConfig=$(echo "scale=3; $numSeconds/$count" | bc)
        summaryInfo[$((curr++))]="Avg Clock Run Time/Config"
        summaryInfo[$((curr++))]=$(printf "%.3f seconds"  "${averagePerConfig}")
    fi
    summaryInfo[$((curr++))]="Logfile"
    summaryInfo[$((curr++))]="${LOG_FILE};;1"
    summaryInfo[$((curr++))]="Results JSON file"
    summaryInfo[$((curr++))]="${JSON_FILE};;1"
    if [[ ${T42_PARALLEL} == 1 ]]; then
        summaryInfo[$((curr++))]="Jobs Logfile"
        summaryInfo[$((curr++))]="${JOB_LOGFILE};;1"
    fi
    summaryInfo[$((curr++))]="Skipped Completely"
    summaryInfo[$((curr++))]=${skippedCompletely}
    summaryInfo[$((curr++))]="Incomplete"
    summaryInfo[$((curr++))]=${incomplete}
    summaryInfo[$((curr++))]="Passed"
    summaryInfo[$((curr++))]=${passed}
    summaryInfo[$((curr++))]="Failed"
    summaryInfo[$((curr++))]=${failedCount}
        
    if [[ ${failedCount} != 0 && ${failedCount} != "" ]] ; then
        summaryInfo[$((curr++))]="===========================================" 
        summaryInfo[$((curr++))]=""
        summaryInfo[$((curr++))]="Failed Configurations:" 
        summaryInfo[$((curr++))]=""
        summaryInfo[$((curr++))]="===========================================" 
        summaryInfo[$((curr++))]=""
        for configName in "${failed[@]}"; do
            currText=""
            reportConfigFooter 1 $configName currText
            summaryInfo[$((curr++))]=""
            summaryInfo[$((curr++))]="${currText}"
        done
    fi

    reportTableInfo summaryInfo
}

dumpHex() {
    str="$1"

    echo "================"
    echo "$str"
    echo $str | od -t x1z
    echo "================"
}
    
getFilterHeaderInfo() {
    local -n headerInfoArray=$1
    local -n currIdx=$2

    headerInfoArray[$((currIdx++))]="Number of filters"
    headerInfoArray[$((currIdx++))]="${#CONFIGS_TO_RUN[@]}"
    for val in "${CONFIGS_TO_RUN[@]}"; do
        headerInfoArray[$((currIdx++))]=""
        headerInfoArray[$((currIdx++))]="${val}"
    done
    
    if [[ ${#CONFIGS_TO_RUN[@]} -ne 0 ]]; then
        local count=0
        local finished=0
        declare -a skipped=()
        for configNum in "${config_sequence[@]}"; do
            if [[ ${finished} == 1 ]]; then
                break
            fi
            for forceQt in "${qt_sequence[@]}"; do
                if [[ ${finished} == 1 ]]; then
                    break
                fi
                for forceDLL in "${dll_sequence[@]}"; do
                    local skipConfig=0
                    
                    local configName=$(getConfigName $configNum $forceQt $forceDLL)
                    isSkippedConfig skipConfig ${configName}
                    if [[ ${skipConfig} == 1 ]]; then
                        skipped+=("$configName")
                    else
                        count=$(($count + 1))
                    fi
                    if [[ ${T42_DEBUG} == 1 && ( $(($count+${#skipped[@]})) -ge 2) ]]; then
                        finished=1
                        break
                    fi
                done
            done
        done
        headerInfoArray[$((currIdx++))]="Number of (filtered) configurations to run"
        headerInfoArray[$((currIdx++))]="${count} - Note skipped configurations are still run for summary analysis"
        headerInfoArray[$((currIdx++))]="Number of skipped configurations"
        headerInfoArray[$((currIdx++))]="${#skipped[@]}"
        for val in "${skipped[@]}"; do
            headerInfoArray[$((currIdx++))]=""
            headerInfoArray[$((currIdx++))]="${val}"
        done
    fi
}


showGlobalHeader() {
    local numConfigs=$((${#CMAKE_CONFIG_FLAGS[@]} + 1)) 
    local numCombinations=$(( (1 << ${#CMAKE_CONFIG_FLAGS[@]}) ))

    if [[ ${FORCE_QT} == 1 ]]; then
        numCombinations=$((${numCombinations} * 2))
    fi
    
    if [[ ${FORCE_DLL} == 1 ]]; then
        numCombinations=$((${numCombinations} * 2))
    fi
    
    declare -a headerInfo=()
    
    local curr=0
    headerInfo[$((curr++))]="Total number of configs"
    headerInfo[$((curr++))]="${numConfigs} (includes all config variables disabled)"

    headerInfo[$((curr++))]="Run with force qt as extra flow"
    if [[ $FORCE_QT == 1 ]]; then
        headerInfo[$((curr++))]+="Yes (doubles the number of runs)"
    else
        headerInfo[$((curr++))]+="No"
    fi

    headerInfo[$((curr++))]="Run with build for non-DLL as extra flow"
    if [[ $FORCE_DLL == 1 ]]; then
        headerInfo[$((curr++))]+="Yes (doubles the number of runs)"
    else
        headerInfo[$((curr++))]+="No"
    fi

    headerInfo[$((curr++))]="Run with only one configuration enabled"
    if [[ $ALL_COMBINATIONS == 0 ]]; then
        headerInfo[$((curr++))]+="Yes"
    else
        headerInfo[$((curr++))]+="No"
    fi

    headerInfo[$((curr++))]="Maximum number of combinations"
    headerInfo[$((curr++))]="${numCombinations}"
    headerInfo[$((curr++))]="Results Logfile"
    headerInfo[$((curr++))]="${LOG_FILE};;1"
    headerInfo[$((curr++))]="Results JSON"
    headerInfo[$((curr++))]="${JSON_FILE};;1"
    if [[ ${T42_PARALLEL} == 1 ]]; then
        headerInfo[$((curr++))]="Jobs Logfile"
        headerInfo[$((curr++))]="${JOB_LOGFILE};;1"
        headerInfo[$((curr++))]="BASH Shell Wrapper"
        headerInfo[$((curr++))]="${BASH_SHELL_WRAPPER};;1"
    fi
    
    generateSequences $numConfigs

    headerInfo[$((curr++))]="Number of (unfiltered) configurations to run"
    headerInfo[$((curr++))]="${NUM_CONFIGS_BEING_RUN}"

    getFilterHeaderInfo headerInfo curr

    reportTableInfo headerInfo
}

buildAllConfigs() {
    processArgs "$@"

    LOG_FILE=${OUT_DIR}/buildAllConfigs.log
    JSON_FILE=${OUT_DIR}/buildAllConfigs.json
    JOB_LOGFILE=${OUT_DIR}/jobsLogFile.tsv

    rm -rf buildAllConfigs.lockfile

    if [[ -f ${LOG_FILE} ]]; then
        mv ${LOG_FILE} ${LOG_FILE}.bak
    fi

    if [[ -f ${JSON_FILE} ]]; then
        mv ${JSON_FILE} ${JSON_FILE}.bak
    fi

    local BASH_SHELL_WRAPPER=${OUT_DIR}/bash.sh
    techoVerbose "Creating bash shell wrapper: $BASH_SHELL_WRAPPER\n"
    #BASH_SHELL_WRAPPER=$(realpath "${BASH_SHELL_WRAPPER}")
    if [[ ${VERBOSE} == 1 ]]; then
        printf "echo ============\necho /usr/bin/bash.exe \"\$@\"\necho ============\n" > $BASH_SHELL_WRAPPER
    else
        rm -rf ${BASH_SHELL_WRAPPER}
    fi
    printf "/usr/bin/bash.exe \"\$@\"\n" >> $BASH_SHELL_WRAPPER

    chmod +x ${BASH_SHELL_WRAPPER}
    export PARALLEL_SHELL=$BASH_SHELL_WRAPPER

    GLOBAL_COUNT_FILE=${OUT_DIR}/configsRunFile.txt

    local startTime=$(date +%s.%N)
    if [[ ${SUMMARY_ONLY} == 0 ]]; then 
        showGlobalHeader
        
        rm -rf "${GLOBAL_COUNT_FILE}"
        run
        if [[ $? != 0 ]]; then
            return 1
        fi
    fi
    local endTime=$(date +%s.%N)
    reportSummary $(echo "scale=3; $endTime - $startTime" | bc)

    if [[ ${T42_DEBUG} == 0 ]]; then 
        techoVerbose "Removing created shell wrapper"
        rm -rf ${BASH_SHELL_WRAPPER}
    fi    
}

#set +o nounset
. /usr/local/bin/env_parallel.bash
#set -o nounset

# global variables NOT used inside parallel
declare -a allConfigs=()
declare -a config_sequence=()
declare -a qt_sequence=()
declare -a dll_sequence=()
declare -a configsRun=()
declare -a globalPassed=()
declare -a globalSkipped=()
declare -a globalFailed=()
declare -a errorVars=()
BASH_SHELL_WRAPPER=
configNum=0
numCombinations=0
START=0
END=-1
NUM_CONFIGS=-1
ALL_COMBINATIONS=0
DEBUG_TEE=0
value=""
val=""
currHeaderInfo=""
currPassVarOpt=""

PASS_VARS=(
    CMAKE_CONFIG_FLAGS
    CONFIGS_TO_RUN
    JSON_FILE
    LOG_FILE
    OUT_DIR
    T42_PARALLEL
    RUN_BUILD
    RUN_CMAKE
    T42_CMAKEUTILS_DIR
    VERBOSE
    REMAINDER_ARGS
    runConfig
    isSkippedConfig
)

for passVar in "${PASS_VARS[@]}"; do
    if [[ -v $passVar ]]; then
        echo "Warning: $passVar set in calling scope"
        unset $passVar
    fi
done

declare -A COLOR_MAP=( ["NONE"]="0" ["BLUE"]="6" ["RED"]="1" ["GREEN"]="2" ["YELLOW"]="190" )

FORCE_QT=1
FORCE_DLL=1
forceDLL=0
forceQt=1

startTime=
endTime=
VALIDATE_ENV=1
SUMMARY_ONLY=0

#set +o nounset
env_parallel --session
#set -o nounset

teeToOutput() 
{
    files=("$@")
    local nocat=1
    if [[ ${#files[@]} == 0 && ! -z ${LOG_FILE} ]]; then
        files+=("$LOG_FILE")
        nocat=0
    fi
    
    if [[ ${nocat} == 1 ]]; then
        for file in "${files[@]}" ; do
            if [[ "${file}" == "${LOG_FILE}" ]]; then
                nocat=0
            fi
        done
    fi
    
    if [[ ${DEBUG_TEE} == 1 ]]; then
        echo "FILES=${files[@]}"
    fi
    
    local removeColorPattern=$"s#(\x1b\[[0-9;]*m)##g"

    local openingPatternAndParams=$"\x1b\]8\;[^;]*\;"
    local stringTerm=$"(\x1b\x5c\x5c)"
    local closingPattern=$"\x1b\]8\;\;${stringTerm}"
    local removeOSC8Pattern=$"s#${openingPatternAndParams}[^\x1b]*${stringTerm}([^\x1b]*)${closingPattern}#\2#g"
    
    local sedStrip="sed -E '$removeColorPattern' | sed -E '$removeOSC8Pattern' | sed -E 's# \x08##g'"
    #local sedStrip="sed -E '$removeColorPattern'"

    declare -a args=()
    if [[ ${nocat} == 0 ]]; then
        args+=("\cat")
    fi

    for file in "${files[@]}"; do
        args+=("${sedStrip} >> $file")
    done

    printf -v cmd '"%s" ' "${args[@]}"

    cmd="pee ${cmd}"
    if [[ ${DEBUG_TEE} == 1 ]]; then
        echo "$cmd"
    fi
    eval $cmd
}
 


addToGlobalRunFile() {
    echo "$1" >> "${GLOBAL_COUNT_FILE}"
}
 
teeToError() {
    local extraFile=${2:-""}
    if [[ ! -z ${extraFile} ]]; then
        tee -a "${LOG_FILE}" "${extraFile}" 2>&1
    else
        tee -a "${LOG_FILE}" 2>&1
    fi
}

getEchoPrefix() {
    local prefix=""
    if [[ ( ${T42_PARALLEL} == 1 ) && ( ${RUN_NUMBER} != -1 ) ]]; then
        printf -v prefix "%d> " "$((RUN_NUMBER+1))"
    fi
    echo "${prefix}"
}

techo() {
    local lcl
    printf -v lcl "$@"
    local prefix=$(getEchoPrefix)
    local fmt="${prefix}%s"
    printf "${fmt}" "${lcl}" | teeToOutput
}

techoColoredText() {
    printColoredText "$@" | teeToOutput
}

techoVerbose() {
    if [[ ${VERBOSE} == 1 ]]; then
        techo "$@"
    fi
}

printColorCode() {
    local colorName=$1

    local code=0
    if [[ -v COLOR_MAP["${colorName}"] ]]; then
        code="${COLOR_MAP["${colorName}"]}"
    fi
    #echo "ColorName=$colorName code=$code"
    
    if [[ ${code} != 0 ]]; then
        printf "\e[38;5;${code}m"
    fi
}

printEndColor() {
    printf "\e[0m"
}

printColoredText() {
    local colorName=$1
    shift
    
    printColorCode $colorName
    printf "$@"
    printEndColor
}

terror() {
    printColoredText RED "${@}" | teeToError
}

getObjectType() {
    local target=$1
    if declare -f "$target" > /dev/null; then  # function
        # declare -f prints the function definition; wc -c counts the bytes
        echo "function"
    elif declare -p "$target" 2>/dev/null | grep -q 'declare -[aA]'; then  # arrays
        # ${#var} gets character count; printf | wc -c gets byte count
        echo "array"
    elif [[ -n ${!target+x} ]]; then #simple variables
        echo "variable"
        size=$(printf '%s' "${!target}" | wc -c)
    else
        echo "<UNKNOWN>"
        return 1
    fi
    return 0
}

getObjectSize() {
    local target=$1
    
    local objType=$(getObjectType $target)
    
    #echo "target=${target} - value = ${#target}"
    getObjectSizeKnownType $target $objType
}
    
getObjectSizeKnownType() {
    local target=$1
    local objType=$2

    local size=0
    if [[ "${objType}" == "function" ]]; then
        # declare -f prints the function definition; wc -c counts the bytes
        size=$(declare -f "$target" | wc -c)
    elif [[ "${objType}" == "array" ]]; then
        # ${#var} gets character count; printf | wc -c gets byte count
        eval "size=\$(printf '%s' \"\${$target[@]}\" | wc -c)"
    elif [[ "${objType}" == "variable" ]]; then
        size=$(printf '%s' "${!target}" | wc -c)
    elif [[ "${objType}" == "unknown" ]]; then
        size=$(printf '%s' "${!target}" | wc -c)
    else
        return 1
    fi
    echo $size
    return 0
}

printStack() {
    asError=${1:0}
    
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
    local -n retValTextName=$1
    local arrayName=$2
    local -n array=$2
    local space=$3
    
    local prefix=${prefix}
    if [[ -z "$space" ]]; then
        space=${#arrayName}
    fi

    printf -v ${retValTextName} "%s%${space}s: " "${prefix}" $arrayName
    if [[ ${#array[@]} == 0 ]]; then
        printf -v ${retValTextName} "%s<EMPTY>\n" "${retValTextName}"
    else
        printf -v ${retValTextName} "%s#%d\n" "${retValTextName}" ${#array[@]}
        space=$(($space + 2))
        for val in "${array[@]}"; do
            printf -v ${retValTextName} "%s%s%${space}s%s\n" "${retValTextName}" "${prefix}" "" $val
        done
    fi
}

printAllValuesVerbose() {
    if [[ ${VERBOSE} == 1 ]]; then
        return 0
    fi
    local retVal=""
    printAllValues retVal "$@"
    techo $retVal
}

headerLineText() {
    echo "========================================"
}

headerLine() {
    techo "%s\n" $(headerLineText)
}

getConfigName() {
    local configNum=$1
    local forceQt=$2
    local forceDLL=$3
    
    if [[ -z ${forceQt} ]]; then
        terror "forceQt not set\n"
        printStack 1
    fi

    if [[ -z ${forceDLL} ]]; then
        terror "forceDLL not set\n"
        printStack 1
    fi
    
    printf "build_config_%04d_QT_%s_DLL_%s" $configNum $forceQt $forceDLL
}

getConfigJSONFileName() {
    local configNum=$1
    local forceQt=$2
    local forceDLL=$3
    
    configName=$(getConfigName $configNum $forceQt $forceDLL)
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
    
createGlobalResults() {
    if [[ -f "${JSON_FILE}" ]]; then
        mv "${JSON_FILE}" "${JSON_FILE}.bak"
    fi
    
    headerLine
    techo "Generating Global Summary Report\n"

    local args=(--indent 4 -n '[inputs[]]')

    if [[ ! -f "${GLOBAL_COUNT_FILE}" ]]; then
        terror "    No configurations run, not creating summary results file\n"
        return 1
    fi
    local configs=()
    while IFS= read -r line; do
        configs+=("$line")
    done < "${GLOBAL_COUNT_FILE}"

    if [[ ${#configs[@]} == 0 ]]; then
        terror "    No configurations run, not creating summary results file\n"
        return 1
    fi
    
    if [[ -f "${JOB_LOGFILE}" ]]; then
        readarray -t configsRan < <(tail -n+2 ${JOB_LOGFILE})
        for currConfigString in "${configsRan[@]}" ;do
            
            local -a currConfigArray=()
            readarray -d $'\t' -t currConfigArray <<<"$currConfigString"

            local runValue=${currConfigArray[8]}
            local host=$(strTrim ${currConfigArray[1]})
            local runtime=$(strTrim ${currConfigArray[3]})

            local configName=$(strTrim $(getConfigNameFromJobLog "${runValue}"))
            local currJsonFile="${OUT_DIR}/${configName}/results.json"
            
            if [[ -f "${currJsonFile}" ]]; then
                jq --sort-keys --arg host "${host}" --argjson runtime ${runtime} '.[0] += {"host": $host, "runtime": $runtime}' "${currJsonFile}" | sponge  "${currJsonFile}"
            fi
        done
    fi

    for configName in "${configs[@]}"; do
        if [[ ${T42_DEBUG} == 1 ]]; then
            if [[ ! -f "${OUT_DIR}/${configName}/results.json" ]]; then
                continue;
            fi
        fi
        
        args+=("${OUT_DIR}/${configName}/results.json")
    done    

    waitForLock createGlobalResults
    techo "    Joining existing configuration result files into new summary json file ${JSON_FILE}\n"
    jq "${args[@]}" > ${JSON_FILE} # retval comes from jq
    status=$?
    if [[ $status != 0 ]]; then
        terror "Error joining resultant json files\n"
    else
        techo "    Finished generating Global Summary Report\n"
    fi
    
    removeLock createGlobalResults
    return $?
}

getOSC8Url() { 
    local path=$1
    local desc=${2:-"${path}"}
    local showRed=${3:-1}
   
    abspath=$(cygpath -m $(pwd)/"$path")

    local url=$(printf "file:///%s" "$abspath")
    
    #echo ""
    #echo -n "${url}" | od -An -tx1
    printf -v osc8 "\e]8;;%s\e\\ \b%s\e]8;;\e\\" "$url" "${desc}"
    #dumpHex ${osc8}
    if [[ -f $path || ( $showRed == 0 ) ]]; then
        printf "%s" "${osc8}"
    else
        red="$(printColorCode RED)"
        noColor=$(printEndColor)
        printf "%s%s%s" "${red}" "${osc8}" "${noColor}"
    fi
}

printConfigHeader() {
    local configNum=$1
    local configName=$2
    local forceQt=$3
    local forceDLL=$4
    local localLogFile=$5
    local localJSONFile=$6
    
    local prefix=$(getEchoPrefix)
    local headerText=""
    if [[ ${T42_PARALLEL} == 1 ]]; then
        printf -v headerText "\n"
    fi
    printf -v headerText "%s%s%s\n" "${headerText}" "${prefix}" "$(headerLineText)"

    percent=$(( (100*${RUN_NUMBER}) / ${NUM_CONFIGS_BEING_RUN} ))

    printf -v headerText "%s%s%s[%d of %d (%d%%)]%s" "${headerText}" "${prefix}" "$(printColorCode "GREEN")" $RUN_NUMBER ${NUM_CONFIGS_BEING_RUN} ${percent} "$(printEndColor)"
    printf -v headerText "%s - Configuration #%d QT=%s DLL=%s" "${headerText}" $configNum ${forceQt} ${forceDLL}
    if [[ ${T42_PARALLEL} == 0 ]]; then
        local failedText="0"
        if [[ ${#globalFailed[@]} != 0 ]]; then
            failedText=$(printColoredText RED "%d" ${#globalFailed[@]} )
        fi
        
        printf -v headerText "%s - Passed: %d" "${headerText}" ${#globalPassed[@]} 

        if [[ ${#globalFailed[@]} != 0 ]]; then
            failedText=$(printColoredText RED "%d" ${#globalFailed[@]} )
            printf -v headerText "%s %sFailed: %d%s" "${headerText}" "$(printColorCode "RED")" ${#globalFailed[@]} $(printEndColor)
        else
            printf -v headerText "%s Failed: 0" "${headerText}"
        fi
        
        printf -v headerText "%s Previously Skipped: %d\n" "${headerText}" ${#globalSkipped[@]}
    else
        printf -v headerText "%s - %s\n" "${headerText}" ${configName}
    fi
    
    local varNames=()
    if [[ ${VERBOSE} == 1 ]]; then 
        varNames=(configNum configName forceQt forceDLL localLogFile localJSONFile LOG_FILE RUN_BUILD RUN_CMAKE CMAKE_CONFIG_FLAGS CONFIGS_TO_RUN)
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
            printAllValues headerText ${var} ${size}
        else
            printf -v headerText "%s%s%${size}s: %s\n" "${headerText}" "${prefix}" $var ${!var}
        fi
    done
    techo "%s" "${headerText}"
}

reportConfigFooter() {
    local status=$1
    local configName="$2"

    if [[ -v $3 ]]; then
        local -n outVar=$3
    fi

    local statusText="PASSED"
    local colorName=""
    if [[ $status == -1 ]]; then
        statusText="SKIPPED"
        colorName="$(printColorCode "BLUE")"
    elif [[ $status == 0 ]]; then
        colorName="$(printColorCode "GREEN")"
    else
        statusText="FAILED"
        colorName="$(printColorCode "RED")"
    fi
    local footerText=""
    printf -v footerText "    %s%s%s : %s" "${colorName}" "${statusText}" $(printEndColor) "${configName} -"
    
    if [[ $status != -1 ]]; then
        logURL=$(getOSC8Url "${OUT_DIR}/$configName/${configName}.log" "Logfile")
        jsonURL=$(getOSC8Url "${OUT_DIR}/$configName/results.json" "JSON File")

        if [[ ${RUN_BUILD} == 1 ]]; then
            slnxURL=$(getOSC8Url "${OUT_DIR}/$configName/Towel42Utils.slnx" "Visual Studio Solution")
            printf -v footerText "%s %s -" "${footerText}" "${slnxURL}"
        fi
        printf -v footerText "%s %s - %s" "${footerText}" "${logURL}" "${jsonURL}"
    fi
    if [[ -v outVar ]]; then
        printf -v outVar "%s\n" "${footerText}"
    else
        techo "%s\n" "${footerText}"
    fi
}


getOptions() {
    local configNum=$1
    local forceQt=$2
    local forceDLL=$3
   
    configs=()
    for (( idx=${#CMAKE_CONFIG_FLAGS[@]}-1 ; idx>=0 ; idx-- )) ; do
        local currConfig
        local onOff
        local shiftValue=$(( ($configNum >> ${idx}) ))
        if [[ $shiftValue == 1 ]]; then
            onOff=ON
        else
            onOff=OFF
        fi
        
        printf -v currConfig "%s%s=%s" "-D" ${CMAKE_CONFIG_FLAGS[idx]} ${onOff}
       
        configs+=("$currConfig")
    done
    printf -v currConfig "%s%s=%s" "-D" TOWEL42_FORCE_QT_FOR_CONFIG_TESTING ${forceQt}
    configs+=("$currConfig")

    printf -v currConfig "%s%s=%s" "-D" TOWEL42_UTILS_DLL ${forceDLL}
    configs+=("$currConfig")
    
    configs+=( "-DTOWEL42_CMAKEUTILS_DIR=${T42_CMAKEUTILS_DIR}" )

    echo "${configs[@]}"
}

reportResult() {
    declare -n varRef=$1
    local status=$2
    local extraLogFile=$3
    local addOutputOnError=$4
    local extraSpace=$5
    
    local prefix=$(getEchoPrefix)
    local spacing="${prefix}        "
    if [[ ${extraSpace} == 1 ]]; then
        spacing+="    "
    fi
    
    if [[ $status == -1 ]]; then # skipped
        varRef=skipped
        techoColoredText BLUE "${spacing}SKIPPED\n"
        if [[ ! -z "${extraLogFile}" ]]; then
            echo "${spacing}SKIPPED" >> ${extraLogFile}
        fi
        return 0
    elif [[ $status == 0 ]]; then
        varRef=passed
        techoColoredText GREEN "${spacing}PASSED\n"
        if [[ ! -z "${extraLogFile}" ]]; then
            echo "${spacing}PASSED" >> ${extraLogFile}
        fi
        return 0
    else
        varRef=failed
        techoColoredText RED "${spacing}FAILED\n"
        if [[ ! -z "${extraLogFile}" && ${addOutputOnError} -ne 0 ]]; then
            headerLine
            techo "Failed Messages from command:\n\n"
            tail -n 10 ${extraLogFile} | teeToOutput
            echo "${spacing}FAILED" >> ${extraLogFile}
            headerLine
        fi
        return 1
    fi
}

reportResultBase() {
    reportResult $1 $2 "" 0 0
}

reportResultWithLog() {
    reportResult $1 $2 $3 0 0
}

reportResultWithLogAndResults() {
    reportResult $1 $2 $3 1 $4
}


setupConfigBuildArea() {
    local configName=$1
    
    # if you are running cmake, always start from a clean directory
    # if you are building without cmake, use the existing one
    # if it doesnt exist, create if building
    # if it exists and you are not running cmake or build, do NOT delete it
    local _mkdir=0
    local rmExisting=0
    if [[ ${RUN_CMAKE} == 1 || ( ! -d ${OUT_DIR}/${configName} && ${RUN_BUILD} == 1 ) ]]; then
        _mkdir=1
        rmExisting=1
    fi

    if [[ ${RUN_CMAKE} == 0 && ${RUN_BUILD} == 0 && -d ${OUT_DIR}/${configName} ]]; then
        rmExisting=0
    fi
        

    SETUP_STATUS=skipped
    if [[ ${_mkdir} == 1 ]]; then
        if [[ -d ${OUT_DIR}/${configName} ]]; then
            if [[ ${rmExisting} == 1 ]]; then 
                techo "    Removing existing directory\n"
                rm -rf ${configName} |& teeToOutput
                status=${PIPESTATUS[0]}
                reportResultBase SETUP_STATUS $status
                if [[ $status != 0 ]]; then
                    return $status
                fi
            fi
        fi
        
        techo "    Creating directory ${OUT_DIR}/$configName\n"
        mkdir -p ${OUT_DIR}/$configName |& teeToOutput
        status=${PIPESTATUS[0]}
        reportResultBase SETUP_STATUS $status
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
    local forceDLL=$4
    
    local _forceRunCMake=0
    if [[ ${RUN_BUILD} == 1 ]]; then
        if [[ ${RUN_CMAKE} == 0 ]]; then
            if [[ ! -f ${OUT_DIR}/${configName}/CMakeCache.txt ]]; then
                _forceRunCMake=1
            fi
        fi
    fi

    options=$(getOptions $configNum $forceQt $forceDLL)
    
    techo "    Running CMake\n"
    if [[ ${_forceRunCMake} == 1 || ${RUN_CMAKE} == 1 ]]; then
        local devWarning=-Wno-dev
        outFiles=("$localLogFile")
        if [[ ${VERBOSE} == 1 ]]; then
            devWarning=-Wdev
            outFiles+=("${LOG_FILE}")
            outFiles+=("/dev/stdout")
        fi
      
        echo "=====================================" | teeToOutput "${outFiles}"
        echo "        CMD: cmake -S . -B ${OUT_DIR}/${configName} ${devWarning} $options" | teeToOutput "${outFiles}"
        echo "=====================================" | teeToOutput "${outFiles}"

        cmake -S . -B ${OUT_DIR}/${configName} ${devWarning} $options |& teeToOutput "${localLogFile}" 
        status=${PIPESTATUS[0]}
        reportResultWithLogAndResults CMAKE_STATUS $status "${localLogFile}" 0
        return $status
    else
        reportResultWithLog CMAKE_STATUS -1 "${localLogFile}"
        return 0
    fi
    return 0
}


runBuild() {
    local configName=$1
    
    BUILD_STATUS=skipped
    local allBuildsStatus=0
    echo "    Running all build configurations" | teeToOutput "${localLogFile}" 

    local -a msConfigs=(Debug Release RelWithDebInfo MinSizeRel)
    if [[ ${T42_DEBUG} == 1 ]]; then
        msConfigs=(RelWithDebInfo)
    fi
    
    local prefix=$(getEchoPrefix)
    for msBuildConfig in "${msConfigs[@]}"; do
        echo "${prefix}        Running Build Configuration-$msBuildConfig" | tee -a ${LOG_FILE} ${localLogFile}

        if [[ ${RUN_BUILD} == 1 ]]; then
            local args=()
            args+=(\"${OUT_DIR}/${configName}/ALL_BUILD.vcxproj\")
            args+=(\"--t:Clean,Build\")
            
            if [[ ${T42_PARALLEL} == 1 ]]; then
                args+=(\"--m:1\")
                args+=(\"--p:CL_MPCount=1\")
            fi
            args+=(\"--p:Configuration=${msBuildConfig}\")
            
            local msbuild="C:/Program Files/Microsoft Visual Studio/18/Enterprise/MSBuild/Current/Bin/amd64/MSBuild.exe"
            local argFile=${OUT_DIR}/${configName}/args-${msBuildConfig}.txt
            echo "${args[@]}" > $argFile
            
            if [[ ${VERBOSE} == 1 ]]; then
                echo "${prefix}=====================================" | teeToOutput "${localLogFile}" 
                echo "${prefix}               CMD: $msbuild \"@${argFile}\"" | teeToOutput "${localLogFile}" 
                echo "${prefix}  argfile contents: ${args[@]}" | teeToOutput "${localLogFile}" 
                echo "${prefix}=====================================" | teeToOutput "${localLogFile}" 
            fi
            
            "$msbuild" @"${argFile}" |& teeToOutput "${localLogFile}" 
            status=${PIPESTATUS[0]}

            reportResultWithLogAndResults BUILD_STATUS $status "${localLogFile}" 1

            if [[ $status != 0 ]]; then
                allBuildsStatus=1
                break
            fi
        else
            status=0
            allBuildsStatus=-1
            reportResultWithLog BUILD_STATUS -1 "${localLogFile}"
        fi
    done
    
    echo "${prefix}    All build Configuration Status" | teeToOutput ${LOG_FILE} "${localLogFile}" 
    reportResultWithLog BUILD_STATUS $allBuildsStatus "${localLogFile}"
    if [[ ${allBuildsStatus} == -1 ]]; then
        allBuildsStatus=0
    fi
    return $allBuildsStatus
}

isSkippedConfig() {
    local -n skipit=$1
    local configName=$2

    skipit=1
    if [[ ${#CONFIGS_TO_RUN[@]} == 0 ]]; then
        skipit=0
    fi
    for currConfig in "${CONFIGS_TO_RUN[@]}"; do
        if [[ $configName =~ ${currConfig} ]]; then
            skipit=0
            break;
        fi
    done
}

runConfig() {
    local -a currConfigString="$1"

    local -a currConfigArray=()
    readarray -d ";" -t currConfigArray <<< "$currConfigString"

    RUN_NUMBER=${currConfigArray[0]}
    local configNum=${currConfigArray[1]}
    local forceQt="${currConfigArray[2]}"
    local forceDLL="${currConfigArray[3]}"

    local configName=$(getConfigName $configNum $forceQt $forceDLL)

    declare -g localLogFile=${OUT_DIR}/${configName}/${configName}.log

    local localJSONFile=$(getConfigJSONFileName $configNum $forceQt $forceDLL)

    local skipConfig=0
    isSkippedConfig skipConfig ${configName}
    
    local currStatus="configuration \"$configName\""
    
    addToGlobalRunFile "${configName}"

    local status=0
    printConfigHeader $RUN_NUMBER $configNum $configName $forceQt $forceDLL $localLogFile $localJSONFile
    if [[ ${skipConfig} == 1 ]]; then 
        if [[ ! -d ${OUT_DIR}/${configName} ]]; then
            mkdir -p ${OUT_DIR}/${configName}
        fi

        globalSkipped+=($configName);
        reportConfigFooter -1 "$configName"
        
        if [[ ! -f ${localJSONFile} ]]; then
            echo "[ { \"configName\":\"$configName\", \"status\":0, \"skipped\":1, \"setup_status\":\"skipped\", \"cmake_status\":\"skipped\", \"build_status\":\"skipped\" } ]" > ${localJSONFile}
        fi
        # dont update json if it already exists and you are skipping the config completely
    else
        headerLine
        techo "Running $currStatus\n"
        
        setupConfigBuildArea $configName
        status=$?
        
        if [[ ${status} == 0 ]]; then
            runCMake $configName $configNum $forceQt $forceDLL
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
            echo "[ { \"configName\":\"$configName\", \"status\":$status, \"skipped\":0, \"setup_status\":\"${SETUP_STATUS}\", \"cmake_status\":\"${CMAKE_STATUS}\", \"build_status\":\"${BUILD_STATUS}\" } ]" > ${localJSONFile}
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

run() {
    determineConfigsToRun allConfigs

    if [[ ${T42_PARALLEL} == 1 ]]; then 
        validateEnvironment
        if [[ $? != 0 ]]; then
            return 1
        fi

        if [[ ! -f "${BASH_SHELL_WRAPPER}" ]]; then
            terror "Could not find shell wrapper \"${BASH_SHELL_WRAPPER}\"\n"
            return 1
        fi

        numParallel=-j+0
        
        local haltOpt="--halt soon,fail=30%"
        if [[ ${T42_DEBUG} == 1 ]]; then
            haltOpt="--halt now,success=${NUM_CONFIGS_BEING_RUN}"
            numParallel=-j${NUM_CONFIGS_BEING_RUN}
        fi
        
        declare -a PARALLEL_OPTS=(
            "--eta" 
            "--progress"
            "--termseq" 
            "INT,2000,KILL,20" 
            "--keep-order" 
            "--total-jobs" 
            "${NUM_CONFIGS_BEING_RUN}" 
            "${haltOpt}" 
            "${numParallel}" 
            "--env" 
            "_" 
            "--joblog" 
            "${JOB_LOGFILE}"
        )
        if [[ ${#REMAINDER_ARGS[@]} != 0 ]]; then
            PARALLEL_OPTS+=("${REMAINDER_ARGS[@]}")
        fi

        if [[ ${VERBOSE} == 1 ]]; then
            techo "Launching parallel\n"
            echo "PARALLEL="${PARALLEL_OPTS[@]}" parallel runConfig "{1}" ::: "${allConfigs[@]}"" | tee -a ${LOG_FILE}
        fi
        #set +o nounset
        PARALLEL="${PARALLEL_OPTS[@]}" parallel runConfig "{1}" ::: "${allConfigs[@]}"
        #set -o nounset
            
        techoVerbose "Finished running parallel\n"
    else
        for currConfigString in "${allConfigs[@]}"; do
            runConfig "${currConfigString}"
        done
    fi
}

# global variables used in parallel
declare -a statusStack=()
SETUP_STATUS=skipped
CMAKE_STATUS=skipped
BUILD_STATUS=skipped

NUM_CONFIGS_BEING_RUN=0
RUN_NUMBER=-1

#variables used inside parallel
OUT_DIR=all_build_configs
if [[ ! -d ${OUT_DIR} ]]; then
    mkdir -p ${OUT_DIR}
fi
LOG_FILE=${OUT_DIR}/buildAllConfigs.log
JSON_FILE=${OUT_DIR}/buildAllConfigs.json
JOB_LOGFILE=${OUT_DIR}/jobsLogFile.tsv

SUMMARY_ONLY=0
T42_DEBUG=0
RUN_BUILD=1
RUN_CMAKE=1
VERBOSE=0
REMAINDER_ARGS=()
T42_PARALLEL=1
T42_CMAKEUTILS_DIR=$(cygpath -m $(realpath ../T42-CMakeUtils/))
declare -a CONFIGS_TO_RUN=()
declare -a CMAKE_CONFIG_FLAGS=(
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
readarray -t CMAKE_CONFIG_FLAGS < <(printf '%s\n' "${CMAKE_CONFIG_FLAGS[@]}" | sort)


buildAllConfigs "$@"
