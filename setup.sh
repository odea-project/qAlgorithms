need_install=""

if ! command -v g++ >/dev/null 2>&1; then
  need_install="$need_install gcc"
fi

if ! command -v make >/dev/null 2>&1; then
  need_install="$need_install make"
fi

# jq
if ! command -v cmake >/dev/null 2>&1; then
  need_install="$need_install cmake"
fi

# zlib-ng (header/library)
if [ ! -f /usr/include/zlib-ng.h ] && [ ! -f /usr/local/include/zlib-ng.h ]; then
  if command -v ldconfig >/dev/null 2>&1; then
    if ! ldconfig -p 2>/dev/null | grep -qi 'libz\.so'; then
      need_install="$need_install zlib-ng"
    fi
  else
    need_install="$need_install zlib-ng"
  fi
fi

need_install="$(echo "$need_install" | xargs)"

if [ -z "$need_install" ]; then
  echo "All packages required for building qAlgorithms are already installed."

  # get the directory of the script (taken from https://stackoverflow.com/questions/59895/how-do-i-get-the-directory-where-a-bash-script-is-located-from-within-the-script)
  SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

  if [ -f $SCRIPT_DIR/CMakeLists.txt ]; then
    mkdir -p build && cmake -S . -B build && cmake --build ./build -j
    # @todo finish off the installation by registering qalgo to the users path
    exit 0
  else
    echo "Error: CMakeLists.txt is missing" 
    exit 2
  fi
else
  echo "Installing missing packages: $need_install"
fi

if command -v apk >/dev/null 2>&1; then
  sudo apk add --no-cache $need_install
elif command -v apt-get >/dev/null 2>&1; then
  sudo apt-get update && sudo apt-get install -y $need_install
elif command -v dnf >/dev/null 2>&1; then
  sudo dnf install -y $need_install
elif command -v pacman >/dev/null 2>&1; then
  sudo pacman -Sy --noconfirm $need_install
elif command -v zypper >/dev/null 2>&1; then
  sudo zypper --non-interactive install $need_install
else
  echo "Error: Package manager not found. You must manually install: $need_install" >&2
  exit 1
fi