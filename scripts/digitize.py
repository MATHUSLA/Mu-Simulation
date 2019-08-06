#!/usr/bin/env python3
# -*- coding: utf-8 -*- #

import os
import sys
from contextlib import contextmanager

import awkward
import uproot
from rootpy import ROOT as R
from rootpy.tree import Tree
from path import Path
from root_numpy import root2array, tree2array, array2tree
import numpy as np
import unyt as u


def open_tree(filepath, tree_key):
    """Open TTree."""
    try:
        file = R.TFile.Open(filepath)
        return file, file.Get(tree_key)
    except KeyError:
        return None, None


def copytree(file, tree):
    """"""
    try:
        ctree = tree.CloneTree(0)
        ctree.SetName(ctree.GetName() + "_digi")
        ctree.SetDirectory(0)
        return ctree
    except OSError:
        return None


def digitize(path, file, tree):
    """"""
    output = R.TFile.Open(str(Path(path).splitext()[0] + ".digi.root"), "RECREATE")
    output.cd()
    digitized = digitize_tree(output, path, copytree(file, tree), tree.GetName())
    digitized.Write()
    for (key, name) in map(lambda k: (k, k.GetName()), file.GetListOfKeys()):
        if name != tree.GetName():
            file.cd()
            obj = key.ReadObj()
            output.cd()
            obj.Write()
    output.cd()
    digitize_tag = R.TNamed("DIGITIZED", "TRUE")
    digitize_tag.Write()
    output.Close()


DTYPE = [
    ("Deposit", "float64"),
    ("Time", "float64"),
    ("Detector", "float64"),
    ("PDG", "int"),
    ("Track", "int"),
    ("Parent", "int"),
    ("X", "float64"),
    ("Y", "float64"),
    ("Z", "float64"),
    ("E", "float64"),
    ("PX", "float64"),
    ("PY", "float64"),
    ("PZ", "float64"),
    ("WEIGHT", "float64"),
]


KEYS = [
    "Deposit",
    "Time",
    "Detector",
    "PDG",
    "Track",
    "Parent",
    "X",
    "Y",
    "Z",
    "E",
    "PX",
    "PY",
    "PZ",
    "WEIGHT",
]


EXTRA_KEYS = [
    "GEN_PDG",
    "GEN_Track",
    "GEN_Parent",
    "GEN_T",
    "GEN_X",
    "GEN_Y",
    "GEN_Z",
    "GEN_E",
    "GEN_PX",
    "GEN_PY",
    "GEN_PZ",
    "GEN_WEIGHT",
    "COSMIC_EVENT_ID",
    "COSMIC_CORE_X",
    "COSMIC_CORE_Y",
    "COSMIC_GEN_PRIMARY_ENERGY",
    "COSMIC_GEN_THETA",
    "COSMIC_GEN_PHI",
    "COSMIC_GEN_FIRST_HEIGHT",
    "COSMIC_GEN_ELECTRON_COUNT",
    "COSMIC_GEN_MUON_COUNT",
    "COSMIC_GEN_HADRON_COUNT",
    "COSMIC_GEN_PRIMARY_ID",
    "EXTRA_11",
    "EXTRA_12",
    "EXTRA_13",
    "EXTRA_14",
    "EXTRA_15",
]


ALL_KEYS = KEYS + EXTRA_KEYS


def get_event_components(row):
    """"""
    return (
        np.sort(
            np.array(list(zip(*row[KEYS])), dtype=DTYPE), order=["Detector", "Time"]
        ),
        row,
    )


def get_subtimes(subevent, threshold, spacing):
    """"""
    # passing_event = subevent[subevent["E"] * u.MeV > thresholds[detector_type]]
    # times = passing_event["Time"] * u.ns
    # if len(times) == 0:
    #    return []
    # current = times[0]
    # if len(times) == 1:
    #    return passing_event[times == current]
    # indices = []
    # for t in times[1:]:
    #    if t > current + spacing:
    #        current = t
    #    indices.extend(list(np.where(times == current)[0]))
    # return passing_event[indices]
    starting_index = 0
    times = subevent["Time"] * u.ns
    indices = []
    energies = []
    while starting_index < len(subevent):
        t0 = times[starting_index]
        components = subevent[times <= t0 + spacing]
        summed = np.cumsum(components["E"] * u.MeV)
        above_threshold = np.where(summed >= threshold)[0]
        if len(above_threshold) == 0:
            starting_index += 1
        else:
            indices.append(above_threshold[0])
            energies.append(summed[-1])
            starting_index += len(summed)
    subevent = subevent[indices]
    subevent["E"] = energies
    return subevent


def clear_row(tree):
    """"""
    for key in ALL_KEYS:
        getattr(tree, key).clear()


def fill_tree(tree, subevent, fullevent):
    """"""
    tree.N_HITS = len(subevent)
    tree.N_GEN = len(fullevent["GEN_PDG"])
    for entry in subevent:
        for key in KEYS:
            getattr(tree, key).push_back(entry[key])
    for key in EXTRA_KEYS:
        for entry in fullevent[key]:
            getattr(tree, key).push_back(entry)
    tree.Fill()
    clear_row(tree)


def digitize_tree(
    output,
    path,
    ctree,
    treename,
    spacing=20 * u.ns,
    thresholds=None,
    is_rpc=lambda d: d > 1000,
):
    """"""
    if thresholds is None:
        thresholds = {"scintillator": 1.9 * u.MeV, "rpc": 0.24 * u.keV}
    output.cd()
    ctree.SetDirectory(output)
    for (event, fullevent) in map(get_event_components, root2array(path, treename)):
        arrays = []
        for detector in event["Detector"]:
            detector_only = event[event["Detector"] == detector]
            detector_type = "rpc" if is_rpc(detector) else "scintillator"
            subevent = get_subtimes(detector_only, thresholds[detector_type], spacing)
            if len(subevent) > 0:
                arrays.append(subevent)
        fill_tree(ctree, np.concatenate(arrays, axis=0), fullevent)
    return ctree


def _traverse_root_files_predicate(skipext):
    """"""
    if skipext is not None:
        return lambda f: f.endswith(".root") and not f.endswith(skipext)
    return lambda f: f.endswith(".root")


def traverse_root_files(directory, skipext=None):
    """Traverse Over ROOT Files."""
    predicate = _traverse_root_files_predicate(skipext)
    for root, _, files in os.walk(directory):
        for filepath in filter(predicate, files):
            yield os.path.join(root, filepath)


def main(argv):
    """Main Loop."""
    if len(argv) < 3:
        print("[ERROR] Tree Name and Input directories Required.")
        return 1

    _, name, *input_directories = argv

    if Path(name).exists():
        print("[ERROR] Specify a Tree Name before directories.")
        return 2

    missing = 0
    for counter, directory in enumerate(input_directories):
        directory_path = Path(directory)
        if not directory_path.exists():
            print("[ERROR] Directory Missing.")
            missing += 1
            continue
        if not directory_path.isdir():
            print("[WARN] Can only Analyze Directories. Skipping ... ", directory_path)
            missing += 1
            continue
        print("Analyzing:", directory)
        for filepath in traverse_root_files(directory, skipext=".digi.root"):
            file, tree = open_tree(filepath, name)
            if file and tree:
                print("[ Reading Path:", filepath)
                digitize(filepath, file, tree)
            else:
                print("[ERROR] Path:", filepath, "could not be read.")

    if missing == 1 + counter:
        print("[ERROR] No data found.")
        return 3

    print("[DONE]")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
