#!/usr/bin/env bash
# Build Skannerz TV MAME driver as WebAssembly.
# Output: build/asmjs/bin/mamesktv.js + mamesktv.wasm
# Then copies them to web/sktv/ for serving.
set -euo pipefail

REPO_DIR="$(cd "$(dirname "$0")" && pwd)"
EMSDK="${EMSDK:-$HOME/emsdk}"

if [ ! -f "$EMSDK/emsdk_env.sh" ]; then
  echo "ERROR: emsdk not found at $EMSDK. Install it first."
  exit 1
fi

# shellcheck disable=SC1091
source "$EMSDK/emsdk_env.sh"

EMSCRIPTEN="$(dirname "$(which emcc)")"

cd "$REPO_DIR"

# On a fresh checkout neither the genie build-file generator nor
# build/generated/version.cpp exist yet. The top-level makefile's "generate"
# target normally produces both, but this script drives genie directly, so
# build them explicitly here. Both rules are no-ops once already up to date.
echo "=== Building genie ==="
make genie -j"$(nproc)"

echo "=== Generating build/generated/version.cpp ==="
make TARGET=mame SUBTARGET=sktv BUILDDIR=build build/generated/version.cpp

echo "=== Generating build files ==="
mkdir -p build/generated/mame/sktv/
EMSCRIPTEN="$EMSCRIPTEN" CC=emcc \
  3rdparty/genie/bin/linux/genie \
    --with-emulator \
    --OPTIMIZE=2 \
    --target=mame \
    --subtarget=sktv \
    --build-dir=build \
    --NOASM=1 \
    --osd=sdl \
    --targetos=asmjs \
    --PLATFORM=x86 \
    --gcc=asmjs \
    --gcc_version="$(emcc -v 2>&1 | grep -oP '(?<=clang version )[0-9.]+' | head -1)" \
    gmake

echo "=== Building (this will take a while) ==="
EMSCRIPTEN="$EMSCRIPTEN" make \
  -R \
  -C build/projects/sdl/mamesktv/gmake-asmjs \
  config=release \
  -j"$(nproc)" \
  2>&1

echo "=== Copying output to web/sktv/ ==="
# The linker outputs to the repo root (TARGETDIR=../../../../..)
cp -v mamesktv.js   web/sktv/
cp -v mamesktv.wasm web/sktv/

echo "=== Done! Serve with: cd web/sktv && python3 serve.py ==="
