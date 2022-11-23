#!/usr/bin/bash

#*================================================================
#*   Copyright (C) 2022 XUranus All rights reserved.
#*   
#*   File:         run_test.sh
#*   Author:       XUranus
#*   Date:         2022-11-18
#*   Description:  
#*
#================================================================*/

mkdir build
cmake -S . -B build
cmake --build build
./build/json_test @?
