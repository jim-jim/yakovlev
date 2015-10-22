#!/bin/bash

~/CMake.app/Contents/bin/cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DPYTHON_LIBRARY=/Library/Frameworks/Python.framework/Versions/3.4/lib/libpython3.4m.dylib \
    -DPYTHON_INCLUDE_DIR=/Library/Frameworks/Python.framework/Versions/3.4/include/python3.4m \
    .