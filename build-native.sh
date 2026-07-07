#!/usr/bin/env bash
# Build Skannerz TV MAME driver as a native Linux binary.
# Output: ./tvgames (repo root)
set -euo pipefail

REPO_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$REPO_DIR"

echo "=== Building tvgames (this will take a while) ==="
# USE_QTDEBUG=0: skip the Qt debugger UI (qmake6/moc aren't installed here).
# NOWERROR=1: GCC 12 throws a false-positive -Werror=restrict in unrelated
#   core files (src/lib/formats/fsmeta.cpp, src/emu/device.cpp, ...) — a
#   known GCC 12/13 std::variant+memcpy warning bug, not a real bug.
make \
  SUBTARGET=tvgames \
  SOURCES=src/mame/tvgames \
  REGENIE=1 \
  USE_QTDEBUG=0 \
  NOWERROR=1 \
  -j"$(nproc)"

echo "=== Done! Run with: ./tvgames rad_sktv ==="
