#include "helper.hh"

static const auto number_of_events = 10;

void muon_mapper(const char* path) {

  using namespace MATHUSLA::MU;

  for (const auto& path : helper::search_directory(path)) {
    auto data_file = TFile::Open(path.c_str(), "READ");
    if (!data_file)
      continue;

    auto tree = static_cast<TTree*>(data_file->Get("mu_map_event0"));
    auto mu_hist = new TH1D("mu_hist", path.c_str(), 100, 0, 100);
    tree->Draw("KE >> mu_hist", "", "goff");
    mu_hist->Scale(1.0L/number_of_events);

    std::vector<std::string> tokens;
    helper::string::split(path, tokens, "/");
    tokens.back().insert(0, "hist_");
    const auto new_path = helper::string::join(tokens, "/");

    auto out_file = new TFile(new_path.c_str(), "NEW");
    if (!out_file)
      continue;

    out_file->cd();
    mu_hist->Write();
    out_file->Close();
  }

}
