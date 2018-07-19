/*
 * scripts/analysis/muon_mapper.C
 *
 * Copyright 2018 Brandon Gomes
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "helper.hh"

//__MATHUSLA ROOT File Keys_____________________________________________________________________
static const auto MUON_MAP_TREE_KEY = "mu_map0";
static const auto HISTOGRAM_SUFFIX  = "_hist";
static const auto HISTOGRAM_NAME    = std::string(MUON_MAP_TREE_KEY) + HISTOGRAM_SUFFIX;
static const auto EVENTS_KEY        = "EVENTS";
static const auto INITIAL_KE_KEY    = "GEN_KE";
static const auto MOMENTUM_UNIT     = "GEN_P_UNIT";
//----------------------------------------------------------------------------------------------

//__Check If File is MATHUSLA DATAFILE__________________________________________________________
bool is_datafile(TFile* file) {
  try {
    return file->Get("FILETYPE")->GetTitle() == std::string("MATHULSA MU-SIM DATAFILE");
  } catch (...) {}
  return false;
}
//----------------------------------------------------------------------------------------------

//__Get Distance from String Representation of Momentum Vector__________________________________
long double calc_distance(std::string vector_string) {
  using namespace MATHUSLA::MU;
  helper::string::strip(vector_string);
  std::vector<std::string> components;
  helper::string::split(vector_string.substr(1, vector_string.size() - 1), components, ",");
  try {
    // Using the IP as (0, 0, -100)
    return std::hypot(100.0L / std::stold(components[2]) * std::stold(components[0]), 100.0L);
  } catch (...) {
    return 0;
  }
}
//----------------------------------------------------------------------------------------------

//__Calculate Boost_____________________________________________________________________________
long double boost(const long double kinetic,
                  const long double mass) {
  return std::sqrt(kinetic*kinetic + 2*kinetic*mass) / mass;
}
//----------------------------------------------------------------------------------------------

//__Get Histogram Title_________________________________________________________________________
const std::string hist_title(const long long distance,
                             const long long energy) {
  return "MuonMapper at "
    + std::to_string(distance) + "m and "
    + std::to_string(energy)   + "GeV";
}
//----------------------------------------------------------------------------------------------

//__Get Muon Efficiency_________________________________________________________________________
long double efficiency(const TH1D* hist,
                       const long long events) {
  return static_cast<long double>(hist->GetEntries()) / events;
}
//----------------------------------------------------------------------------------------------

//__Construct Valid CSV Path From ROOT Path_____________________________________________________
const std::string csv_path(const std::string& path,
                           const std::size_t event_count) {
  using namespace MATHUSLA::MU;
  std::vector<std::string> tokens, ending;
  helper::string::split(path, tokens, "/");
  helper::string::split(tokens.back(), ending, ".");
  ending.front() += "_" + std::to_string(event_count);
  ending.back() = "csv";
  tokens.back() = helper::string::join(ending, ".");
  return helper::string::join(tokens, "/");
}
//----------------------------------------------------------------------------------------------

//__Main Function: MuonMapper___________________________________________________________________
void muon_mapper(const char* dir) {
  using namespace MATHUSLA::MU;

  for (const auto& path : helper::search_directory(dir)) {
    std::cout << "Reading " << path << ":\n";
    auto data_file = TFile::Open(path.c_str(), "UPDATE");
    if (!data_file || data_file->IsZombie()) continue;
    try {
      if (!is_datafile(data_file)) continue;

      const auto event_count = std::stoull(data_file->Get(EVENTS_KEY)->GetTitle());
      const auto energy = std::llround(std::stold(data_file->Get(INITIAL_KE_KEY)->GetTitle()) / 1000.0L);
      const auto distance = std::llround(calc_distance(data_file->Get(MOMENTUM_UNIT)->GetTitle()));

      auto tree = static_cast<TTree*>(data_file->Get(MUON_MAP_TREE_KEY));
      if (!tree)
        continue;
    
      // correction based on incorrect entry on MuonMapper
      constexpr auto muon_mass = 105.658369L;
      auto true_tree = new TTree("corrected_logB_tree", "corrected_logB_tree");
      Double_t logB, old_logB;
      true_tree->Branch("logB", &logB);
      tree->SetBranchStatus("R", 0); // disable corrupt branch
      tree->SetBranchAddress("logB", &old_logB);
      long double max_K = 0, min_K = energy * 1000.0L;
      const auto size = tree->GetEntries();
      for (int i = 0; i < size; ++i) {
        tree->GetEntry(i);
        const auto B = std::pow(10.0L, old_logB);
        const auto K = muon_mass * (std::sqrt(1 + B*B) - 1) * 1000.0L; // <- scale factor
        min_K = std::min(K, min_K);
        max_K = std::max(K, max_K);
        logB = std::log10(boost(K, muon_mass));
        true_tree->Fill();
      }
      
      true_tree->Write();
      
      const auto min_log_boost = size == 0 ? -1.0L : std::floor(std::log10(boost(min_K, muon_mass)));
      const auto max_log_boost = size == 0 ?  4.0L : std::ceil(std::log10(boost(max_K, muon_mass)));
      const auto width = 0.025L;
      const auto bins = std::llround((max_log_boost - min_log_boost) / width);
      auto mu_map_hist = new TH1D(
        HISTOGRAM_NAME.c_str(),
        hist_title(distance, energy).c_str(),
        bins,
        min_log_boost,
        max_log_boost);

      if (!mu_map_hist)
        continue;

      true_tree->Draw(("logB >> " + HISTOGRAM_NAME).c_str(), "", "goff");

      mu_map_hist->Scale(1.0L / event_count);
      mu_map_hist->GetXaxis()->SetTitle("log10(Boost)");
      mu_map_hist->GetYaxis()->SetTitle("Efficiency");
      mu_map_hist->Write();

      std::cout << "Added Histogram:\n"
                << "  Event Count: " << event_count << "\n"
                << "  Initial KE:  " << energy      << " GeV\n"
                << "  Distance:    " << distance    << " m\n"
                << "  Efficiency:  " << efficiency(mu_map_hist, event_count) << "\n";

      helper::to_csv(csv_path(path, event_count), mu_map_hist);

    } catch (...) {}
    data_file->Close();
  }
}
//----------------------------------------------------------------------------------------------
