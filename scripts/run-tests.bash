#!/usr/bin/env bash

cmake --build build && ctest --test-dir build --output-on-failure
