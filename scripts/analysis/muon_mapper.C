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
const std::string csv_path(const std::string& path) {
  using namespace MATHUSLA::MU;
  std::vector<std::string> tokens, ending;
  helper::string::split(path, tokens, "/");
  helper::string::split(tokens.back(), ending, ".");
  ending.back() = "csv";
  tokens.back() = helper::string::join(ending, ".");
  return helper::string::join(tokens, "/");
}
//----------------------------------------------------------------------------------------------

//__Main Function: MuonMapper___________________________________________________________________
void muon_mapper(const char* dir) {
  using namespace MATHUSLA::MU;

  for (const auto& path : helper::search_directory(dir)) {
    auto data_file = TFile::Open(path.c_str(), "UPDATE");
    if (!data_file || data_file->IsZombie())
      continue;
    try {
      if (!is_datafile(data_file)) continue;

      const auto event_count = std::stoull(data_file->Get(EVENTS_KEY)->GetTitle());
      const auto energy = std::llround(std::stold(data_file->Get(INITIAL_KE_KEY)->GetTitle()) / 1000.0L);
      const auto distance = std::llround(calc_distance(data_file->Get(MOMENTUM_UNIT)->GetTitle()));

      auto tree = static_cast<TTree*>(data_file->Get(MUON_MAP_TREE_KEY));
      if (!tree) continue;

      auto mu_map_hist = new TH1D(
        HISTOGRAM_NAME.c_str(),
        hist_title(distance, energy).c_str(),
        100,
        0.0L, energy);

      tree->Draw(("KE >> " + HISTOGRAM_NAME).c_str(), "", "goff");

      mu_map_hist->Scale(1.0L / event_count);
      mu_map_hist->GetXaxis()->SetTitle("KE [GeV]");
      mu_map_hist->GetYaxis()->SetTitle("Efficiency");
      mu_map_hist->Write();

      std::cout << "Added Histogram for " << path << "\n"
                << "  Event Count: " << event_count << "\n"
                << "  Initial KE:  " << energy      << "\n"
                << "  Distance:    " << distance    << "\n"
                << "  Efficiency:  " << efficiency(mu_map_hist, event_count) << "\n";

      helper::to_csv(csv_path(path), mu_map_hist);

    } catch (...) {}
  }
}
//----------------------------------------------------------------------------------------------
