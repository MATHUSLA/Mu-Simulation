#ifndef MU__SCRIPTS_ANALYSIS_HELPER_HH
#define MU__SCRIPTS_ANALYSIS_HELPER_HH
#pragma once

#include <string>

#include "TSystemDirectory.h"

namespace helper {

template<class T, unsigned N>
inline unsigned arraysize(const T (&v)[N]) { return N; }

static const std::string detector_decode(int id) {
  if (id < 0) return "";

  static const std::string detector[] = {
    "A11_C3", "A12_C4", "A13_C5",  "A14_C6",  "A15_C7",
    "A21_C3", "A22_C4", "A23_C5",  "A24_C6",  "A25_C7",
    "A31_B7", "A32_B8", "A33_B9",  "A34_B10", "A35_B11",
    "A41_B8", "A42_B9", "A43_B10", "A44_B11", "A45_B12",
    "A51_C8", "A52_C9", "A53_C10", "A54_C11",
    "A61_C8", "A62_C9", "A63_C10", "A64_C11",

    "B11_C3",  "B12_C4",  "B13_B6",  "B14_B7",  "B15_C7",
    "B21_C3",  "B22_C4",  "B23_C5",  "B24_C6",  "B25_C7",
    "B31_B7",  "B32_B8",  "B33_B9",  "B34_C9",  "B35_C9",
    "B41_A10", "B42_A11", "B43_A11", "B44_B9",  "B45_B12_1", "B46_B11",
    "B51_A10", "B52_A12", "B53_C7",  "B54_C8",  "B55_C9",
    "B61_B11", "B62_B11", "B63_C9",  "B64_B11", "B65_B11"
  };

  if (id < arraysize(detector)) {
    return detector[id];
  } else {
    auto temp = std::to_string(id);
    std::reverse(temp.begin(), temp.end());
    std::string out = "";
    out += temp[0];
    out += '_';
    out += temp[1];
    if (temp[1] == '0') {
      out += temp[2];
      out += '_';
      out += temp[3];
      if (temp.size() == 5)
        out += temp[4];
    } else {
      out += '_';
      out += temp[2];
      if (temp.size() == 4)
        out += temp[3];
    }
    std::reverse(out.begin(), out.end());
    return out;
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
