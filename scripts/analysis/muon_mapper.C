#include "TChain.h"
#include "TH2D.h"

#include "helper.hh"

void muon_mapper(const char* path,
                 const size_t length_bins,
                 const double length_min,
                 const double length_max,
                 const size_t energy_bins,
                 const double energy_min,
                 const double energy_max) {

  using namespace MATHUSLA::MU;

  TChain muon_chain("mu_map_event0");
  for (const auto& path : helper::search_directory(path))
    muon_chain.Add(path.c_str());

  auto hist = new TH2D("muon_histogram", "Muon Energy Map",
    length_bins, length_min, length_max,
    energy_bins, energy_min, energy_max);

  muon_chain.Draw("R:KE >> muon_histogram", "", "goff");

  hist->GetXaxis()->SetTitle("Distance [m]");
  hist->GetYaxis()->SetTitle("Kinetic Energy [GeV]");
  
  auto file = new TFile("../muon_map.root","RECREATE");
  hist->Write();
  file->Close();
}
