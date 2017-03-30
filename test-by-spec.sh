#!/bin/bash

TESTDIR="swarm-protocol-docs/test"
if [ ! -e $TESTDIR ]; then
    git submodule init
    git submodule update --init --recursive
fi
cd swarm-protocol-docs
#git checkout master
#git pull
git log | head -6
cd -

rm -rf tmp
mkdir tmp

if [ ! -e Makefile ]; then
    cmake CMakeLists.txt || exit 1
fi

HEADERS="64x64"

for HEADER in $HEADERS; do
    echo testing $HEADER
    TEST="test$HEADER"
    make "test$HEADER" || exit 2
    echo
    grep -v '^;' $TESTDIR/$HEADER.batt > tmp/$HEADER.ok
    sh -c "./test$HEADER" > tmp/$HEADER.out
    diff -BU3 tmp/$HEADER.out tmp/$HEADER.ok || exit 3
    echo OK $HEADER
done
exit 1

grep -v '^;' $TESTDIR/stamp.batt | grep -v '^$' > tmp/stamp.ok
./teststamp > tmp/stamp.out
diff -BU3 tmp/stamp.out tmp/stamp.ok || exit 4
echo OK stamp

grep -v '^;' $TESTDIR/spec.batt | grep -v '^$'  > tmp/spec.ok
./testspec > tmp/spec.out
diff -BU3 tmp/spec.out tmp/spec.ok || exit 5
echo OK spec

grep -v '^;' $TESTDIR/op.batt | grep -v '^$'  > tmp/op.ok
./testop > tmp/op.out
diff -BU3 tmp/op.out tmp/op.ok || exit 6
echo OK op

echo DONE

