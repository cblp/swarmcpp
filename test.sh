#!/bin/bash
set -eux -o pipefail

./test-headers.sh

# FIXME(cblp, 2017-04-03) still failing
./test-by-spec.sh
