#!/bin/bash

TESTDIR=tests
BIN=./shortest

echo "Compiling..."
make

for i in "$TESTDIR"/*
do
		echo "Running tests on $i"
        "$BIN" < "$i"/input | diff - "$i"/output | colordiff
done
