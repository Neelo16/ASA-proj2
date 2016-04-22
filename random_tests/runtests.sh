#!/bin/bash

FILE=../shortest.c
BIN=./shortest

set -e

echo "Compiling..."
gcc -o $BIN -O3 -ansi -Wall $FILE -lm

for i in *.in
do
		TEST=${i%.in}
		echo "Running tests on $i"
        { time $BIN < "$i" ; } 2> "$TEST.time"
done
