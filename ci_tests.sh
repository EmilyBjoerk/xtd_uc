#!/usr/bin/env bash

# Run tests when any source file in the project changes
while inotifywait -e modify -r .; do
    make -f Makefile_gtest;
done
