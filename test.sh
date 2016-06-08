#!/bin/bash

if [ ! -e docs/ ]; then
    git clone https://github.com/gritzko/swarm-protocol-docs.git docs
fi

rm -rf tmp
mkdir tmp

make || exit -1

grep -v '^;' docs/test/64x64.batt > tmp/64x64.ok

./test64x64 > tmp/64x64.out
diff -BU3 tmp/64x64.out tmp/64x64.ok || exit -2
echo OK 64x64
