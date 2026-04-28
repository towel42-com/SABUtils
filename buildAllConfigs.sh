#!/usr/bin/bash

#set -o nounset

getYesNo() {
    if [[ $1 == 1 ]]; then
        echo "ON"
    else
        echo "OFF"
    fi
}

Usage() {
    echo "buildAllConfigs.sh: --outdir <dir> --logfile <filename> --json <filename> --cmakeutilsdir <directory> --start <n> --end <n> --build --cmake --verbose --debug --parallel --all_combinations --forceqt --dll [config1 config2...] "
    echo "    Run all configurations"
    echo "  --outdir <dir> : Output directory (default ${OUT_DIR})"
    echo "       --logfile : The output logfile for all the runs (default buildAllConfigs.log)"
    echo "          --json : The output json file for all the runs (default buildAllConfigs.json)"
    echo "         --cmake : Run the cmake stage (may get overridden if required by the build stage) (default $(getYesNo ${RUN_CMAKE}))"
    echo "         --build : Run the build stage (default $(getYesNo ${RUN_BUILD}))"
    echo "       --verbose : Give extended debugging information (default $(getYesNo ${VERBOSE}))"
    echo "         --debug : Runs in debug mode (only runs the first 3 configurations, overrides --end, only runs the RelWithDebInfo build type) (default $(getYesNo ${DEBUG}))"
    echo "      --parallel : Runs in configurations in parallel (default $(getYesNo ${T42_PARALLEL}))"
    echo "       --forceqt : Enable forcing qt as a secondary config (default $(getYesNo ${FORCE_QT}))"
    echo "           --dll : Run for both DLL and non-DLL builds (default $(getYesNo ${FORCE_DLL}))"
    echo "--all_combinations : Runs all combinations of configuration options, Otherwise only run 1 configuration option at a time (default $(getYesNo ${ALL_COMBINATIONS}))"
    echo "       --dry-run : Runs in dry-run (ignored if --parallel not set) (default $(getYesNo ${DRY_RUN}))"
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

generateSequences() {
    local numConfigs=$1

    if [[ ${END} -eq -1 ]]; then
        END=$(( $numCombinations - 1 ))
    fi
    
    config_sequence=($(seq $START $END))
    if [[ ${ALL_COMBINATIONS} == 0 ]]; then
        config_sequence=(0)
        local value=1
        local ii
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

    numConfigsBeingRun=$((${#config_sequence[@]} * ${#qt_sequence[@]} * ${#dll_sequence[@]}))
    if [[ ${T42_DEBUG} == 1 ]]; then 
        local initialSize=${numConfigsBeingRun}
        numConfigsBeingRun=4
        techo "INFO: Reducing configurations for debug mode: From $initialSize to ${numConfigsBeingRun}\n"
    fi
}

validateVariables() {
    headerLine
    techo "Validating global variables are not being ignored\n"
        
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
    techo "    Finished validating global variables\n"

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
    headerLine
        
    if [[ ${VERBOSE} == 1 ]]; then
        techo "Getting the size of exported global varables\n"
        techo "    The following variables are not in the ignore variables set:\n"
        local totalSize=0
        for val in "${passedVars[@]}"; do
            local sz=$(getObjectSize "$val")
            techo "        $val - size: $sz\n"
            totalSize=$(( $totalSize + $sz ))
        done

        techo "        =======================\n"
        techo "        Total size of variables=$totalSize\n"
        techo "    Finished getting the size of exported global varables\n"
    fi
}

dumpHex() {
    str="$1"

    echo "================"
    echo "$str"
    echo $str | od -An -tx1
    echo "================"
}
    
getFilterHeaderInfo() {
    local -n headerInfoArray=$1
    local -n currIdx=$2

    headerInfoArray[$((currIdx++))]="Number of filters"
    headerInfoArray[$((currIdx++))]="${#CONFIGS_TO_RUN[@]}"
    headerInfoArray[$((curr++))]=0
    for val in "${CONFIGS_TO_RUN[@]}"; do
        headerInfoArray[$((currIdx++))]=""
        headerInfoArray[$((currIdx++))]="${val}"
        headerInfoArray[$((curr++))]=0
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
        headerInfoArray[$((curr++))]=0
        headerInfoArray[$((currIdx++))]="Number of skipped configurations"
        headerInfoArray[$((currIdx++))]="${#skipped[@]}"
        headerInfoArray[$((curr++))]=0
        for val in "${skipped[@]}"; do
            headerInfoArray[$((currIdx++))]=""
            headerInfoArray[$((currIdx++))]="${val}"
            headerInfoArray[$((curr++))]=0
        done
    fi
}

getMaxLen() {
    local -n lclHeaderInfo=$1
    local maxLen=0
    for ((ii=0; ii<${#lclHeaderInfo[@]}; ii+=3)); do
        local currLen=${#lclHeaderInfo[ii]}
        if [[ $currLen -gt $maxLen ]]; then
            maxLen=$currLen
        fi
    done
    maxLen=$((maxLen + 1))
    echo $maxLen
}

reportHeaderInfo() {
    local -n headerInfoArray=$1

    local retVal=""
    
    local maxLen=$(getMaxLen headerInfoArray)
    for ((ii=0; ii<${#headerInfoArray[@]}; ii+=3)); do
        local header="${headerInfoArray[ii]}"
        local value="${headerInfoArray[ii+1]}"
        local isFile="${headerInfoArray[ii+2]}"
        
        #echo "header=${header}"
        #echo "value=${value}"
        
        if [[ ${isFile} == 1 ]]; then
            value=$(getOSC8Url "${value}" "" 0)
        fi
        
        printf -v retVal "%s%${maxLen}s : %s\n" "$retVal" "${header}" "${value}"
    done
    techo "${retVal}"
   
}

showGlobalHeader() {
    local numConfigs=$((${#CONFIGS[@]} + 1)) 
    local numCombinations=$(( (1 << ${#CONFIGS[@]}) ))

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
    headerInfo[$((curr++))]=0

    headerInfo[$((curr++))]="Run with force qt as extra flow"
    if [[ $FORCE_QT == 1 ]]; then
        headerInfo[$((curr++))]+="Yes (doubles the number of runs)"
    else
        headerInfo[$((curr++))]+="No"
    fi
    headerInfo[$((curr++))]=0

    headerInfo[$((curr++))]="Run with build for non-DLL as extra flow"
    if [[ $FORCE_DLL == 1 ]]; then
        headerInfo[$((curr++))]+="Yes (doubles the number of runs)"
    else
        headerInfo[$((curr++))]+="No"
    fi
    headerInfo[$((curr++))]=0

    headerInfo[$((curr++))]="Run with only one configuration enabled"
    if [[ $ALL_COMBINATIONS == 0 ]]; then
        headerInfo[$((curr++))]+="Yes"
    else
        headerInfo[$((curr++))]+="No"
    fi
    headerInfo[$((curr++))]=0

    headerInfo[$((curr++))]="Maximum number of combinations"
    headerInfo[$((curr++))]="${numCombinations}"
    headerInfo[$((curr++))]=0
    headerInfo[$((curr++))]="Results Logfile"
    headerInfo[$((curr++))]="${LOG_FILE}"
    headerInfo[$((curr++))]=1
    headerInfo[$((curr++))]="Results JSON"
    headerInfo[$((curr++))]="${JSON_FILE}"
    headerInfo[$((curr++))]=1
    if [[ ${T42_PARALLEL} == 1 ]]; then
        headerInfo[$((curr++))]="Jobs Logfile"
        headerInfo[$((curr++))]="${JOB_LOGFILE}"
        headerInfo[$((curr++))]=1
        headerInfo[$((curr++))]="BASH Shell Wrapper"
        headerInfo[$((curr++))]="${BASH_SHELL_WRAPPER}"
        headerInfo[$((curr++))]=1
    fi
    
    generateSequences $numConfigs

    headerInfo[$((curr++))]="Number of (unfiltered) configurations to run"
    headerInfo[$((curr++))]="${numConfigsBeingRun}"
    headerInfo[$((curr++))]=0

    getFilterHeaderInfo headerInfo curr

    reportHeaderInfo headerInfo
}

#set +o nounset
. /usr/local/bin/env_parallel.bash
#set -o nounset

configsRun=()
globalPassed=()
globalSkipped=()
globalFailed=()
numCombinations=0
START=0
END=-1
ALL_COMBINATIONS=0
DEBUG_TEE=0
DRY_RUN=0
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
    T42_PARALLEL
    RUN_BUILD
    RUN_CMAKE
    T42_CMAKEUTILS_DIR
    VERBOSE
    qt_sequence
    config_sequence
    dll_sequence
    runConfig
    isSkippedConfig
)

PASS_VARS_OPT=""
#headerLine
#set
#headerLine

for passVar in "${PASS_VARS[@]}"; do
    if [[ -v $passVar ]]; then
        echo "Warning: $passVar set in calling scope"
        unset $passVar
    fi
done
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
        tee -a "${LOG_FILE}" ${2:-""} 2>&1
    else
        tee -a "${LOG_FILE}" 2>&1
    fi
}

techo() {
    printf "$@" | teeToOutput
}

techoColoredText() {
    printColoredText "$@" | teeToOutput
}

techoVerbose() {
    if [[ ${VERBOSE} == 1 ]]; then
        techo "$@"
    fi
}

declare -A COLOR_MAP=( ["NONE"]="0" ["BLUE"]="6" ["RED"]="1" ["GREEN"]="2" )
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
    
    if [[ -z "$space" ]]; then
        space=${#arrayName}
    fi

    printf -v ${retValTextName} "%${space}s: " $arrayName
    if [[ ${#array[@]} == 0 ]]; then
        printf -v ${retValTextName} "%s<EMPTY>\n" "${retValTextName}"
    else
        printf -v ${retValTextName} "%s#%d\n" "${retValTextName}" ${#array[@]}
        space=$(($space + 2))
        for val in "${array[@]}"; do
            printf -v ${retValTextName} "%s%${space}s%s\n" "${retValTextName}" "" $val
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

blankLine() {
    techo "\n"
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

addJSONResults() {
    local configNum=$1
    local forceQt=$2
    local forceDLL=$3
    
    local configJsonFile=$(getConfigJSONFileName $configNum $forceQt $forceDLL)
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
    local forceDLL=$3

    local configJsonFile=$(getConfigJSONFileName $configNum $forceQt $forceDLL)    
    if [[ ! -f ${configJsonFile} ]]; then
        terror "Could not find JSON file '${configJsonFile}'"
        continue;
    fi

    addJSONResults $configNum $forceQt $forceDLL
    return 0
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
    for config in "${configs[@]}"; do
        if [[ ${T42_DEBUG} == 1 ]]; then
            if [[ ! -f "${OUT_DIR}/${config}/results.json" ]]; then
                continue;
            fi
        fi
            
        args+=("${OUT_DIR}/${config}/results.json")
    done    

    waitForLock createGlobalResults
    techo "    Joining existing config files into new summary json file ${JSON_FILE}\n"
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

reportSummary() {
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
    local failed=($failedStr)
    
    removeLock reportSummary
    
    local logFileURL=$(getOSC8Url "${LOG_FILE}" "Log File")
    local jsonFileURL=$(getOSC8Url "${JSON_FILE}" "JSON File")
    
    printf -v summaryText "===========================================\n"
    printf -v summaryText "%sSummary:\n" "${summaryText}"
    printf -v summaryText "%s    Configurations Run: %d\n" "${summaryText}" "${count}"
    printf -v summaryText "%s               Logfile: %s\n" "${summaryText}" "${logFileURL}"
    printf -v summaryText "%s     Results JSON file: %s\n" "${summaryText}" "${jsonFileURL}"
    if [[ ${T42_PARALLEL} == 1 ]]; then
        local jobsLogFileURL=$(getOSC8Url "${JOB_LOGFILE}" "Jobs File")
        printf -v summaryText "%s          Jobs Logfile: %s\n"  "${summaryText}" "${jobsLogFileURL}"
    fi
    printf -v summaryText "%s    Skipped Completely: %d\n"  "${summaryText}" "${skippedCompletely}"
    printf -v summaryText "%s            Incomplete: %d\n"  "${summaryText}" "${incomplete}"
    printf -v summaryText "%s                Passed: %d\n"  "${summaryText}" "${passed}"
    printf -v summaryText "%s                Failed: %d\n"  "${summaryText}" "${failedCount}"
        
    if [[ ${failedCount} != 0 && ${failedCount} != "" ]] ; then
        printf -v summaryText "%s===========================================\n" "${summaryText}"
        printf -v summaryText "%sFailed Configurations:\n" "${summaryText}"
        printf -v summaryText "%s===========================================\n" "${summaryText}"
        for config in "${failed[@]}"; do
            currText=""
            reportConfigFooter 1 $config currText
            printf -v summaryText "%s%s" "${summaryText}" "${currText}"
        done
    fi

    printf "%s" "${summaryText}"
}

SETUP_STATUS=skipped
CMAKE_STATUS=skipped
BUILD_STATUS=skipped

printConfigHeader() {
    local runNumber=$1
    local configNum=$2
    local configName=$3
    local forceQt=$4
    local forceDLL=$5
    local localLogFile=$6
    local localJSONFile=$7
    
    local headerText=""
    if [[ ${T42_PARALLEL} == 1 ]]; then
        printf -v headerText "\n"
    fi
    printf -v headerText "%s%s\n" "${headerText}" "$(headerLineText)"

    percent=$(( (100*${runNumber}) / ${numConfigsBeingRun} ))

    printf -v headerText "%s%s[%d of %d (%d%%)]%s" "${headerText}" "$(printColorCode "GREEN")" $runNumber ${numConfigsBeingRun} ${percent} "$(printEndColor)"
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
        varNames=(configNum configName forceQt forceDLL localLogFile localJSONFile LOG_FILE RUN_BUILD RUN_CMAKE CONFIGS CONFIGS_TO_RUN)
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
            printf -v headerText "%s%${size}s: %s\n" "${headerText}" $var ${!var}
        fi
    done
    techo "%s" "${headerText}"
}

reportConfigFooter() {
    local status=$1
    local config="$2"

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
    if [[ ! -v outVar ]]; then
        printf -v footerText "Finished running configuration "${configName}"\n"
    fi
    printf -v footerText "%s    %s%s%s :" "${footerText}" "${colorName}" "${statusText}" $(printEndColor)
    
    if [[ $status != -1 ]]; then
        logURL=$(getOSC8Url "${OUT_DIR}/$config/${config}.log" "Logfile")
        jsonURL=$(getOSC8Url "${OUT_DIR}/$config/results.json" "JSON File")

        if [[ ${RUN_BUILD} == 1 ]]; then
            slnxURL=$(getOSC8Url "${OUT_DIR}/$config/Towel42Utils.slnx" "Visual Studio Solution")
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

    printf -v currConfig "%s%s=%s" "-D" TOWEL42_UTILS_DLL ${forceDLL}
    configs+=("$currConfig")

    echo "${configs[@]}"
}

reportResult() {
    declare -n varRef=$1
    local status=$2
    local extraLogFile=$3
    local addOutputOnError=$4
    local extraSpace=$5
    
    local spacing="        "
    if [[ ! -z "${extraSpace}" ]]; then
        spacing="            "
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
    reportResult $1 $2 $3 1 0
}


setupConfigBuildArea() {
    local configName=$1
    
    # if you are running cmake, always start from a clean directory
    # if you are building without cmake, use the existing one
    # if it doesnt exist, create if building
    # if it exists and you are not running cmake or build, DO NOT delete it
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
        echo "        CMD: cmake -S . -B ${OUT_DIR}/${configName} ${devWarning} -DTOWEL42_CMAKEUTILS_DIR=${T42_CMAKEUTILS_DIR} $options" | teeToOutput "${outFiles}"
        echo "=====================================" | teeToOutput "${outFiles}"

        cmake -S . -B ${OUT_DIR}/${configName} ${devWarning} -DTOWEL42_CMAKEUTILS_DIR=${T42_CMAKEUTILS_DIR} $options |& teeToOutput "${localLogFile}" 
        status=${PIPESTATUS[0]}
        reportResultWithLogAndResults CMAKE_STATUS $status "${localLogFile}"
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
    
    for config in "${msConfigs[@]}"; do
        echo "        Running Build Configuration-$config" | tee -a ${LOG_FILE} ${localLogFile}

        if [[ ${RUN_BUILD} == 1 ]]; then
            local args=()
            args+=(\"${OUT_DIR}/${configName}/ALL_BUILD.vcxproj\")
            args+=(\"--t:Clean,Build\")
            
            if [[ ${T42_PARALLEL} == 1 ]]; then
                args+=(\"--m:1\")
                args+=(\"--p:CL_MPCount=1\")
            fi
            args+=(\"--p:Configuration=${config}\")
            
            local msbuild="C:/Program Files/Microsoft Visual Studio/18/Enterprise/MSBuild/Current/Bin/amd64/MSBuild.exe"
            local argFile=${OUT_DIR}/${configName}/args-${config}.txt
            echo "${args[@]}" > $argFile
            
            if [[ ${VERBOSE} == 1 ]]; then
                echo "=====================================" | teeToOutput "${localLogFile}" 
                echo "               CMD: $msbuild \"@${argFile}\"" | teeToOutput "${localLogFile}" 
                echo "  argfile contents: ${args[@]}" | teeToOutput "${localLogFile}" 
                echo "=====================================" | teeToOutput "${localLogFile}" 
            fi
            
            "$msbuild" @"${argFile}" |& teeToOutput "${localLogFile}" 
            status=${PIPESTATUS[0]}

            reportResultWithLogAndResults BUILD_STATUS $status "${localLogFile}"

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
    
    echo "    All build Configuration Status" | teeToOutput ${LOG_FILE} "${localLogFile}" 
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
    for config in "${CONFIGS_TO_RUN[@]}"; do
        if [[ $configName =~ ${config} ]]; then
            skipit=0
            break;
        fi
    done
}

runConfig() {
    local runNumber=$1
    local configNum=$2
    local forceQt=$3
    local forceDLL=$4
    
    local configName=$(getConfigName $configNum $forceQt $forceDLL)

    declare -g localLogFile=${OUT_DIR}/${configName}/${configName}.log
    local localJSONFile=$(getConfigJSONFileName $configNum $forceQt $forceDLL)

    local skipConfig=0
    isSkippedConfig skipConfig ${configName}
    
    local currStatus="configuration \"$configName\""
    
    addToGlobalRunFile "${configName}"

    local status=0
    printConfigHeader $runNumber $configNum $configName $forceQt $forceDLL $localLogFile $localJSONFile
    if [[ ${skipConfig} == 1 ]]; then 
        if [[ ! -d ${OUT_DIR}/${configName} ]]; then
            mkdir -p ${OUT_DIR}/${configName}
        fi

        globalSkipped+=($configName);
        reportConfigFooter -1 "$configName"
        
        echo "[ { \"configName\":\"$configName\", \"status\":0, \"skipped\":1, \"setup_status\":\"skipped\", \"cmake_status\":\"skipped\", \"build_status\":\"skipped\" } ]" > ${localJSONFile}
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
    if [[ ${T42_PARALLEL} == 1 ]]; then 
        PASS_VARS_OPT=()
        
        validateVariables
        
        if [[ ! -f "${BASH_SHELL_WRAPPER}" ]]; then
            terror "Could not find shell wrapper \"${BASH_SHELL_WRAPPER}\"\n"
            return 1
        fi

        dryRunOpt=""
        if [[ ${DRY_RUN} == 1 ]]; then
            dryRunOpt=--dry-run
        fi
        PASS_VARS_OPT=${PASS_VARS_OPT[@]}
        # echo PASS_VARS_OPT=${PASS_VARS_OPT}
        numParallel=-j+0
        
        local haltOpt="--halt soon,fail=30%"
        if [[ ${T42_DEBUG} == 1 ]]; then
            haltOpt="--halt now,success=${numConfigsBeingRun}"
            numParallel=-j${numConfigsBeingRun}
        fi
        
        PARALLEL_OPTS="--eta --termseq INT,2000,KILL,20 --keep-order --total-jobs ${numConfigsBeingRun} ${haltOpt} ${dryRunOpt} ${numParallel} --env _ --joblog ${JOB_LOGFILE}"
        #echo ${PARALLEL_OPTS}

        if [[ ${VERBOSE} == 1 ]]; then
            techo "Launching parallel\n"
            echo "PARALLEL=${PARALLEL_OPTS} parallel runConfig "{#}" "{1}" "{2}" "{3}" ::: "${config_sequence[@]}" ::: "${qt_sequence[@]}" ::: "${dll_sequence[@]}"" | tee -a ${LOG_FILE}
        fi
        #set +o nounset
        PARALLEL=${PARALLEL_OPTS} parallel runConfig "{#}" "{1}" "{2}" "{3}" ::: "${config_sequence[@]}" ::: "${qt_sequence[@]}" ::: "${dll_sequence[@]}"
        #set -o nounset
            
        techoVerbose "Finished running parallel\n"
    else
        local count=0
        local finished=0
        for configNum in "${config_sequence[@]}"; do
            if [[ ${finished} == 1 ]]; then
                break
            fi
            for forceQt in "${qt_sequence[@]}"; do
                if [[ ${finished} == 1 ]]; then
                    break
                fi
                for forceDLL in "${dll_sequence[@]}"; do
                    runConfig "${count}" $configNum ${forceQt} ${forceDLL}
                    count=$(($count + 1))
                    if [[ ${T42_DEBUG} == 1 && ( $count -ge ${numConfigsBeingRun} ) ]]; then
                        finished=1
                        break
                    fi
                done
            done
        done
    fi
}

numConfigsBeingRun=0

#variables used inside parallel
OUT_DIR=all_build_configs
LOG_FILE=${OUT_DIR}/buildAllConfigs.log

JSON_FILE=${OUT_DIR}/buildAllConfigs.json
JOB_LOGFILE=${OUT_DIR}/jobsLogFile.log

if [[ ! -d ${OUT_DIR} ]]; then
    mkdir -p ${OUT_DIR}
fi

T42_DEBUG=0
RUN_BUILD=1
FORCE_QT=1
FORCE_DLL=1
RUN_CMAKE=1
VERBOSE=0
T42_PARALLEL=1
T42_CMAKEUTILS_DIR=$(cygpath -m $(realpath ../T42-CMakeUtils/))
declare -a CONFIGS_TO_RUN=()
declare -a CONFIGS=()

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

BASH_SHELL_WRAPPER=${OUT_DIR}/bash.sh
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
rm -rf "${GLOBAL_COUNT_FILE}"

touch "${JSON_FILE}"

showGlobalHeader

run

if [[ ${DRY_RUN} == 0 ]]; then
    reportSummary
fi


if [[ ${T42_DEBUG} == 0 ]]; then 
    techoVerbose "Removing created shell wrapper"
    rm -rf ${BASH_SHELL_WRAPPER}
fi    