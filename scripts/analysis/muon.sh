#!/bin/bash
# Muon Map

DATA_TO="../muon_map/data_$1/"
mkdir -p $DATA_TO
./install --run -q -o $DATA_TO -s scripts/muon_map_loop.mac energy 100 count 1000
