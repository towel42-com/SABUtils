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
            --forceqt|--no-forceqt)
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
    if [[ $FORCE_QT == 0 ]]; then
        qt_sequence=(OFF)
    fi
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
    headerInfo[$((curr++))]="Results Logfile;${LOG_FILE}"
    headerInfo[$((curr++))]="Results JSON;${JSON_FILE}"

    generateSequences $numConfigs

    local numConfigsBeingRun=$((${#sequence[@]} * ${#qt_sequence[@]}))
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
    
        techo "%${maxLen}s : %s\n" "${arr[0]}" "${arr[1]}"
    done
}

. /usr/local/bin/env_parallel.bash

#declared before the session as they arent used in the run itself
. buildAllConfigs-utils.sh

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
)

PASS_VARS_OPT=""
DEBUG=0
#headerLine
#set
#headerLine

env_parallel --session

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
showGlobalHeader

runConfig() {
    local configNum=$1
    local forceQt=$2

    . ./buildAllConfigs-utils.sh 

    runConfig_Impl "$configNum" "$forceQt"
}

rm -rf buildAllConfigs.lockfile

if [[ -f ${LOG_FILE} ]]; then
    mv ${LOG_FILE} ${LOG_FILE}.bak
fi

if [[ -f ${JSON_FILE} ]]; then
    mv ${JSON_FILE} ${JSON_FILE}.bak
fi

if [[ -v ignoreMap ]]; then
    echo "ignoreMap set to ${ignoreMap[@]}"
fi

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
    export PARALLEL_SHELL=./bash.sh
    env_parallel \
        --eta \
        ${dryRunOpt} \
        ${numParallel} \
        --env _ \
        runConfig "{1}" "{2}" ::: ${sequence[@]} ::: ${qt_sequence[@]}
else
    for ii in ${sequence[@]}; do
        for forceQt in ON OFF; do
            runConfig $ii ${forceQt}
        done
    done
fi

reportSummary

