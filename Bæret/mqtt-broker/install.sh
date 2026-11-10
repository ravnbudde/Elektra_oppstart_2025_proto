#!/usr/bin/env bash
# Intern del av Bæret/install.sh; bruker samme innlogging som poengtavlen.
set -euo pipefail
SOURCE_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
python3 "$SOURCE_DIR/../configure.py" broker
