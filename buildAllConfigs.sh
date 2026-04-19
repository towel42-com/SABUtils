#!/usr/bin/bash

Usage() {
    echo "buildAllConfigs.sh: --logfile <filename> --json <filename> --cmakeutilsdir <directory> --start <n> --end <n> --build --cmake --verbose --debug --parallel --single [config1 config2...] "
    echo "    Run all configurations"
    echo "      --logfile  : The output log file for all the runs (default buildAllConfigs.log)"
    echo "         --json  : The output json file for all the runs (default buildAllConfigs.json)"
    echo "        --cmake  : Run the cmake stage (may get overridden if required by the build stage) (default ${RUN_CMAKE})"
    echo "        --build  : Run the build stage (default ${RUN_BUILD})"
    echo "      --verbose  : Give extended debugging information (default ${VERBOSE})"
    echo "      --debug    : Runs in debug mode (only runs the first 3 configurations, overrides --end) (default ${DEBUG})"
    echo "      --parallel : Runs in configurations in parallel (default ${PARALLEL})"
    echo "        --single : Runs single configurations only, no two configurations are on at the same time (default ${SINGLE})"
    echo " --cmakeutilsdir : The directory for the Towel 42 CMake Utilities (default ${T42_CMAKEUTILS_DIR})"
    echo "         --start : Start with configuration number (default 0)"
    echo "           --end : End at this configuration number (default MAX)"
    echo "       configN  : The list of configurations to run (default run all)"
    echo ""
    echo "     boolean options (build, cmake, verbose etc can be turned off via --no<option>"
    echo ""
    echo "     -h|--help   : Displays this message"
}

. /usr/local/bin/env_parallel.bash

. buildAllConfigs-utils.sh # call before the session its called inside runConfig for parallel

#declared before the session as they arent used in parallel runs
globalPassed=()
globalSkipped=()
globalFailed=()
numCombinations=0

env_parallel --session

LOG_FILE=buildAllConfigs.log
JSON_FILE=buildAllConfigs.json
RUN_BUILD=1
RUN_CMAKE=1
VERBOSE=0
DEBUG=0
PARALLEL=1
START=0
END=-1
SINGLE=0
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

while [[ $# -gt 0 ]]; do
    arg="$1"
    case $arg in 
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
        --nobuild)
            RUN_BUILD=0
            shift
        ;;
        --cmake)
            RUN_CMAKE=1
            shift
        ;;
        --nocmake)
            RUN_CMAKE=0
            shift
        ;;
        --debug)
            DEBUG=1
            END=2
            shift
        ;;
        --nodebug)
            DEBUG=0
            shift
        ;;
        --parallel)
            PARALLEL=1
            shift
        ;;
        --noparallel)
            PARALLEL=0
            shift
        ;;
        --single)
            SINGLE=1
            shift
        ;;
        --nosingle)
            SINGLE=0
            shift
        ;;
        --verbose)
            VERBOSE=1
            shift
        ;;
        --noverbose)
            VERBOSE=0
            shift
        ;;
        -h*|--help)
            Usage
            exit 0
        ;;
        *)
            CONFIGS_TO_RUN[$1]="1"
            shift
        ;;
    esac
done

readarray -t CONFIGS < <(printf '%s\n' "${CONFIGS[@]}" | sort)

runConfig() {
    local configNum=$1
    . ./buildAllConfigs-utils.sh
    runConfig_Impl $configNum
}
    
numConfigs=${#CONFIGS[@]} 
numCombinations=$((1 << ${numConfigs}))

techo "     Total number of configs: ${numConfigs}\n"
techo "Total number of combinations: ${numCombinations}\n"
techo "                    Logfile : ${LOG_FILE}\n"
techo "               Results JSON : ${JSON_FILE}\n"

rm -rf buildAllConfigs.lockfile

if [[ -f ${LOG_FILE} ]]; then
    mv ${LOG_FILE} ${LOG_FILE}.bak
fi

if [[ -f ${JSON_FILE} ]]; then
    mv ${JSON_FILE} ${JSON_FILE}.bak
fi

PASS_VARS=(
    CONFIGS
    CONFIGS_TO_RUN
    DEBUG
    JSON_FILE
    LOG_FILE
    PARALLEL
    RUN_BUILD
    RUN_CMAKE
    T42_CMAKEUTILS_DIR
    VERBOSE
)


if [[ ${END} -eq -1 ]]; then
    END=$(( $numCombinations - 1 ))
fi
  
if [[ ${DEBUG} == 1 ]]; then
    techo "\nNOTE: Only running the first $(($END + 1)) configurations\n"
fi
  
  
sequence=($(seq $START $END))
if [[ ${SINGLE} == 1 ]]; then
    sequence=(0)
    value=1
    for (( ii=0; ii<${numConfigs}; ii++ )); do
        sequence+=($value)
        value=$(( $value << 1))
    done
    #local n=$1
    ## Check if n > 0 AND bitwise (n & (n - 1)) is 0
    #if [[ ! $((${configNum} & (${configNum} - 1))) == 0 ]]; then
    #    return 0
    #fi
fi

if [[ ${PARALLEL} == 1 ]]; then 
    PASS_VARS_OPT=()
    ERROR=0
    for val in "${PASS_VARS[@]}"; do
        printf -v currPassVarOpt "%s %s" "--env" $val
        PASS_VARS_OPT+=(${currPassVarOpt})

        isIgnored=$(echo $PARALLEL_IGNORED_NAMES | tr ' ' '\n' | grep -w $val)
        if [[ ! -z "${isIgnored}" ]]; then
            terror "ERROR: Required variable '$val' is currently being ignored\n    Declare the variable after the calle to 'env_parallel --session'\n"
            ERROR=1
        fi
    done

    if [[ ${ERROR} == 1 ]]; then
        headerLine 2>&1
        echo $PARALLEL_IGNORED_NAMES 2>&1
        headerLine 2>&1
        exit -1
    fi


    PASS_VARS_OPT=${PASS_VARS_OPT[@]}
    # echo PASS_VARS_OPT=${PASS_VARS_OPT}
    numParallel=-j+0
    env_parallel \
        --eta \
        ${numParallel} \
        runConfig {} ::: ${sequence[@]}
else
    for ii in ${sequence[@]}; do
        runConfig $ii
    done
fi

reportSummary

