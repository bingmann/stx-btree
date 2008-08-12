#!/bin/bash -x

for subdir in [gi]cc*; do

    pushd $subdir

    if [ ! -e speed-all.txt ]; then
        ./speedtest
    fi

    popd

done 
