# cppcheck is on pip, installation via pipx is recommended

# Note for installing fbinfer: When extracting with 7z, the option -snld20 has to be passed so that 
# the symlinks within the tarball are extracted correctly. See the following issue:
# https://sourceforge.net/p/sevenzip/bugs/2593/ 

# install locally (without giving sudo perms):
# TAR="   !!!   REPLACE ME  !!!   /infer-linux-x86_64-v1.3.0.tar.xz"
# mkdir -p "~/.local/opt/infer" && 
# tar -C "~/.local/opt/infer" -xJf "$TAR" && \
# ln -sf "~/.local/opt/infer/infer-linux-x86_64-v1.3.0/bin/infer" "~/.local/bin/infer"

# get the directory of the script (taken from https://stackoverflow.com/questions/59895/how-do-i-get-the-directory-where-a-bash-script-is-located-from-within-the-script)
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

# cache results for incremental static analysis
mkdir -p $SCRIPT_DIR/cppcheck_cache

# while other analysers can be run, these are executed individually for less cluttered results
# or (in the case of clang-tidy) used as IDE plugins. CodeChecker is nicer than the native
# infer CLI, especially for long traces. "Security" is enabled as it also finds some potential
# errors separate from clang-tidy, albeit it is much noisier with the used libraries
CodeChecker analyze $SCRIPT_DIR/build/compile_commands.json --output $SCRIPT_DIR/build/reports \
            --enable profile:security --analyzers infer -e infer

CodeChecker parse --export html --output  $SCRIPT_DIR/build/reports_html  $SCRIPT_DIR/build/reports

rm -r $SCRIPT_DIR/build/reports/*