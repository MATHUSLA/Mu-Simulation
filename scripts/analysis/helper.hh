#ifndef MU__SCRIPTS_ANALYSIS_HELPER_HH
#define MU__SCRIPTS_ANALYSIS_HELPER_HH
#pragma once

#include <string>

#include "TSystemDirectory.h"

namespace helper {

template<class T, unsigned N>
unsigned arraysize(const T (&v)[N]) { return N; }

static const std::string detector_decode(int id) {
  if (id < 0) return "";

  static const std::string detector[] = {
    "A11_C7",  "A12_C6",  "A13_C5",  "A14_C4",  "A15_C3",
    "A21_C3",  "A22_C4",  "A23_C5",  "A24_C6",  "A25_C7",
    "A31_B11", "A32_B10", "A33_B9",  "A34_B8",  "A35_B7",
    "A41_B8",  "A42_B9",  "A43_B10", "A44_B11", "A45_B12",
    "A51_C11", "A52_C10", "A53_C9",  "A54_C8",
    "A61_C8",  "A62_C9",  "A63_C10", "A64_C11",

    "B11_C7",  "B12_B7",    "B13_B6", "B14_C4",  "B15_C3",
    "B21_C3",  "B22_C4",    "B23_C5", "B24_C6",  "B25_C7",
    "B31_C9",  "B32_C9",    "B33_B9", "B34_B8",  "B35_B7",
    "B41_B11", "B42_B12_1", "B43_B9", "B44_A11", "B45_A11", "B46_A10",
    "B51_A10", "B52_A12",   "B53_C7", "B54_C8",  "B55_C9",
    "B61_B11", "B62_B11",   "B63_C9", "B64_B11", "B65_B11"
  };

  if (id < arraysize(detector)) {
    return detector[id];
  } else {
    auto out = std::to_string(id);
    if (out.size() < 5) return "0" + out;
    else return out;
  }
}

void collect_paths(TSystemDirectory* dir, std::vector<std::string>& paths, const std::string& ext) {
  if (!dir || !dir->GetListOfFiles()) return;
  for (const auto&& obj: *dir->GetListOfFiles()) {
    const auto file = static_cast<TSystemFile*>(obj);
    const auto name = std::string(file->GetName());
    if (!file->IsDirectory()) {
      if (ext == "" || name.substr(1 + name.find_last_of(".")) == ext)
        paths.push_back(std::string(file->GetTitle()) + "/" + name);
    } else if (!(name == "." || name == "..")) {
      collect_paths(static_cast<TSystemDirectory*>(file), paths, ext);
    }
  }
}

} /* namespace helper */

#endif /* MU__SCRIPTS_ANALYSIS_HELPER_HH */
