#!/bin/bash
set -e

find . -iname *.h -o -iname *.cc -o -iname *.c | xargs clang-format-12 --dry-run --Werror
