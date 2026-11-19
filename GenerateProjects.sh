#!/bin/bash
set -e
premake5 gmake || premake5 gmake2
make -j$(sysctl -n hw.ncpu 2>/dev/null || nproc)