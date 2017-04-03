#!/bin/bash
set -eu -o pipefail

TESTDIR="swarm-protocol-docs/test"
if [ ! -e $TESTDIR ]; then
    git submodule init
    git submodule update --init --recursive
fi
(
    cd swarm-protocol-docs
    #git checkout master
    #git pull
    git log | head -6 || :
)

rm -rf tmp
mkdir tmp

(
    mkdir -p build
    cd build
    if [ ! -e Makefile ]; then
        cmake .. || exit 1
    fi
    # make || exit 2 # TODO
    echo
)

HEADERS="64x64"

for HEADER in $HEADERS; do
    echo testing $HEADER
    TEST="test$HEADER"
    make "test$HEADER" || exit 2
    echo
    grep -v '^;' ../$TESTDIR/$HEADER.batt | grep -v '^$' > $HEADER.ok
    ./test$HEADER > $HEADER.out
    diff -BU3 $HEADER.out $HEADER.ok || exit 3
    echo OK $HEADER
done

exit 1 # TODO

grep -v '^;' $TESTDIR/stamp.batt | grep -v '^$' > tmp/stamp.ok
build/teststamp > tmp/stamp.out
diff -BU3 tmp/stamp.out tmp/stamp.ok || exit 4
echo OK stamp

grep -v '^;' $TESTDIR/spec.batt | grep -v '^$'  > tmp/spec.ok
build/testspec > tmp/spec.out
diff -BU3 tmp/spec.out tmp/spec.ok || exit 5
echo OK spec

grep -v '^;' $TESTDIR/op.batt | grep -v '^$'  > tmp/op.ok
build/testop > tmp/op.out
diff -BU3 tmp/op.out tmp/op.ok || exit 6
echo OK op

echo DONE
