#!/bin/bash
# Muon Map

DATA_TO="../muon_data"
DATA_FROM="data"

mkdir -p $DATA_FROM
mkdir -p $DATA_TO

./install --run -q -s scripts/muon_map.mac

if [ "$(ls -A $DATA_FROM)" ]; then
  rsync -av $DATA_FROM/ $DATA_TO/ --remove-source-files
  rm -rf $DATA_FROM
fi

root -l "scripts/analysis/muon_mapper.C(\"$DATA_TO\")"
