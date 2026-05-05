#!/usr/bin/bash

before=$(compgen -A variable -A function)
. ~/bin/scripts/buildAllConfigs.sh

T42_CMAKEUTILS_DIR=$(cygpath -m $(realpath ../T42-CMakeUtils/))
declare -a args=(
    "--cmake-option"
    "-DTOWEL42_CMAKEUTILS_DIR=${T42_CMAKEUTILS_DIR}"
    "--cmake-config-option"
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
    "--cmake-boolean-option"
    "TOWEL42_UTILS_DLL"
    "force-dll"
    "Run for both DLL and non-DLL builds"
    "TOWEL42_FORCE_QT_FOR_CONFIG_TESTING"
    "force-qt"
    "Enable forcing qt as a secondary config"
)

set -- "$@" "${args[@]}" 

unset args
unset T42_CMAKEUTILS_DIR

#declare -A depFuncs
#getDependentFunctions --noprint runConfig depFuncs

#declare -A depVars=()
#getGlobalVarsInFunctions depFuncs depVars

set +o nounset

buildAllConfigs "$@"

after=$(compgen -A variable -A function)

#echo $after | grep -Fvx "$before"
