#include "helper.hh"

// FIXME: unfortunate magic variable
static const auto number_of_events = 1000;

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
    std::vector<std::string> ending;
    helper::string::split(tokens.back(), ending, ".");
    ending.back() = "csv";
    tokens.back() = helper::string::join(ending, ".");
    helper::to_csv(helper::string::join(tokens, "/"), mu_hist);
  }

}
