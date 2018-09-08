#!/bin/bash
# Muon Map

DATA_TO="../muon_map/data_$1/"
mkdir -p $DATA_TO
./simulation -j$2 -q -o $DATA_TO -s studies/muon_map/loop.mac energy 100 count 1000
