#include "TChain.h"
#include "TH2D.h"

#include "helper.hh"

void muon_mapper(const char* path) {

  using namespace MATHUSLA::MU;

  for (const auto& path : helper::search_directory(path)) {
    auto tree = new TTree("mu_map_event0", "Muon Mapper Data");
    tree->ReadFile(path.c_str(), "KE:R");
    auto mu_hist = new TH1D("mu_hist", path.c_str(), 100, 0, 100);
    tree->Draw("KE >> mu_hist");
    mu_hist->Scale(1.0L/10);
    auto new_file = new TFile(("hist" + path).c_str());
    new_file->cd();
    mu_hist->Write();
    new_file->Close();
  }

}
