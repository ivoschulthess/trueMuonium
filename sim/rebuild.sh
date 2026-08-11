#!/bin/sh
source /cvmfs/sft.cern.ch/lcg/views/setupViews.sh LCG_107 x86_64-el9-gcc14-opt
./clean && ./runconfigure && make -j4
