# this is the full command to run static analysis with cppcheck on the entire codebase. Obviously,
# cppcheck must be installed first.

# get the directory of the script (taken from https://stackoverflow.com/questions/59895/how-do-i-get-the-directory-where-a-bash-script-is-located-from-within-the-script)
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

# cache results for incremental static analysis
mkdir -p $SCRIPT_DIR/cppcheck_cache
# 
cppcheck --project="$SCRIPT_DIR/build/compile_commands.json" --cppcheck-build-dir="$SCRIPT_DIR/cppcheck_cache" \
         --inline-suppr --enable=style --suppress=*:*/external/* --suppress=postfixOperator \
         --suppress=dangerousTypeCast --suppress=uninitMemberVarNoCtor --suppress=constVariablePointer
        