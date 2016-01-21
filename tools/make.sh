#!/bin/bash
# prepare string source code files

python strinCify.py strings.txt ma_strings && mv ma_strings.* ../src/
