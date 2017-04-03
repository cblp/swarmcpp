#!/bin/bash
set -eux -o pipefail

COMPILE_OPTIONS="-c -std=c++11 -Wall -Werror -pedantic"
COMPILE_INCLUDES="-I`pwd`/include"

# TODO(fromagxo, 2017-04-03) randomize order of headers
# headers=($(find include -type f | sed 's|^include/||'))
headers=(64x64.h) # FIXME(fromagxo, 2017-04-03) test ALL headers

function includeAll {
    for header in ${headers[*]}; do
        echo "#include <$header>"
    done
}

mkdir -p tmp
testdir=$(mktemp -d tmp/XXXXXXXX)
cd $testdir

# repetitive inclusion
includeAll > testRepetitiveInclusion.cpp
includeAll >> testRepetitiveInclusion.cpp
g++ $COMPILE_OPTIONS $COMPILE_INCLUDES testRepetitiveInclusion.cpp

# repetitive linking
includeAll > testRepetitiveLinking1.cpp
includeAll > testRepetitiveLinking2.cpp
g++ $COMPILE_OPTIONS            \
    $COMPILE_INCLUDES           \
    testRepetitiveLinking1.cpp  \
    testRepetitiveLinking2.cpp

# exit
rm -rf $testdir
