#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
OUT="$(mktemp -d)"
trap 'rm -rf "$OUT"' EXIT
CXX="${CXX:-c++}"
"$CXX" -std=c++11 -Wall -Wextra -Wpedantic -Werror \
  -I"$ROOT/tests/raylib_stub" -I"$ROOT/game" \
  -c "$ROOT/game/app.cpp" -o "$OUT/app.o"
"$CXX" -std=c++11 -Wall -Wextra -Wpedantic -Werror \
  -I"$ROOT/tests/raylib_stub" -I"$ROOT/game" \
  -c "$ROOT/main.cpp" -o "$OUT/main.o"
echo "UI and entry point compile with the raylib API stub."
