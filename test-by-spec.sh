#!/bin/bash

TESTDIR="swarm-protocol-docs/test"
if [ ! -e $TESTDIR ]; then
    git submodule init
fi
cd swarm-protocol-docs
#git checkout master
#git pull
git log | head -6
cd -

rm -rf tmp
mkdir tmp

make || exit -1
echo

grep -v '^;' $TESTDIR/64x64.batt > tmp/64x64.ok
./test64x64 > tmp/64x64.out
diff -BU3 tmp/64x64.out tmp/64x64.ok || exit -2
echo OK 64x64

grep -v '^;' $TESTDIR/stamp.batt | grep -v '^$' > tmp/stamp.ok
./teststamp > tmp/stamp.out
diff -BU3 tmp/stamp.out tmp/stamp.ok || exit -2
echo OK stamp

grep -v '^;' $TESTDIR/spec.batt | grep -v '^$'  > tmp/spec.ok
./testspec > tmp/spec.out
diff -BU3 tmp/spec.out tmp/spec.ok || exit -2
echo OK spec

