#include <iostream>

#include "TBrowser.h"
#include "TChain.h"

#include "helper.hh"

void prototype_analysis() {
  TSystemDirectory data_folder("data", "data");

  std::vector<std::string> paths;
  helper::collect_paths(&data_folder, paths, "root");

  TChain chain("prototype");
  std::cout << "Data Files: \n";
  for (const auto& path : paths) {
    chain.Add(path.c_str());
    std::cout << "  " << path << "\n";
  }

  double event, deposit, t, detector, pdg, track, x, y, z, e, px, py, pz, d_pmt;
  double timestamp_date, timestamp_time;

  chain.SetBranchAddress("Event",    &event);
  chain.SetBranchAddress("Deposit",  &deposit);
  chain.SetBranchAddress("Time",     &t);
  chain.SetBranchAddress("Detector", &detector);
  chain.SetBranchAddress("PDG",      &pdg);
  chain.SetBranchAddress("TrackID",  &track);
  chain.SetBranchAddress("X",        &x);
  chain.SetBranchAddress("Y",        &y);
  chain.SetBranchAddress("Z",        &z);
  chain.SetBranchAddress("E",        &e);
  chain.SetBranchAddress("PX",       &px);
  chain.SetBranchAddress("PY",       &py);
  chain.SetBranchAddress("PZ",       &pz);
  chain.SetBranchAddress("D_PMT",    &d_pmt);

  chain.SetBranchAddress("TimestampDate", &timestamp_date);
  chain.SetBranchAddress("TimestampTime", &timestamp_time);

  /*
  const auto chain_length = chain.GetEntries();
  for (size_t i = 0; i < chain_length; ++i) {
    chain.GetEntry(i);
    std::cout << detector_decode(detector) << "  " << event << "\n";
  }
  */

}
