/*
 * studies/helper.hh
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

#ifndef MU__STUDIES_HELPER_HH
#define MU__STUDIES_HELPER_HH
#pragma once

#include <cstdio>
#include <ostream>
#include <string>

#include "TSystemDirectory.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TFile.h"
#include "TTree.h"

namespace MATHUSLA { namespace MU {

namespace helper { /////////////////////////////////////////////////////////////////////////////

//__Get the Size of an Array____________________________________________________________________
template<class T, std::size_t N>
std::size_t arraysize(const T (&v)[N]) { return N; }
//----------------------------------------------------------------------------------------------

//__Decode Prototype Detector___________________________________________________________________
inline const std::string prototype_detector_decode(const std::size_t id) {
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
    const auto out = std::to_string(id);
    return out.size() < 5 ? "0" + out : out;
  }
}
//----------------------------------------------------------------------------------------------

namespace io { /////////////////////////////////////////////////////////////////////////////////

namespace { ////////////////////////////////////////////////////////////////////////////////////
//__Recursive TSystemFile Traversal_____________________________________________________________
void _collect_paths(TSystemDirectory* dir,
                    std::vector<std::string>& paths,
                    const std::string& ext) {
  if (!dir || !dir->GetListOfFiles()) return;
  for (const auto&& obj : *dir->GetListOfFiles()) {
    const auto file = static_cast<TSystemFile*>(obj);
    const auto&& name = std::string(file->GetName());
    if (!file->IsDirectory()) {
      if (ext == "" || name.substr(1 + name.find_last_of(".")) == ext)
        paths.push_back(std::string(file->GetTitle()) + "/" + name);
    } else if (!(name == "." || name == "..")) {
      _collect_paths(static_cast<TSystemDirectory*>(file), paths, ext);
    }
  }
}
//----------------------------------------------------------------------------------------------
} /* anonymous namespace */ ////////////////////////////////////////////////////////////////////

//__Search and Collect ROOT File Paths__________________________________________________________
inline std::vector<std::string> search_directory(const std::string& path,
                                                 const std::string& ext="root") {
  std::vector<std::string> paths{};
  _collect_paths(new TSystemDirectory("data", path.c_str()), paths, ext);
  return paths;
}
//----------------------------------------------------------------------------------------------

//__Save TObjects to File_______________________________________________________________________
void save_objects(TFile* file,
                  const TObject* object) {
  file->WriteTObject(object);
}
template<class ...Args>
void save_objects(TFile* file,
                  const TObject* object,
                  Args&& ...args) {
  save_objects(file, object);
  save_objects(file, std::forward<Args>(args)...);
}
//----------------------------------------------------------------------------------------------

//__Run Function While File is Open_____________________________________________________________
template<class UnaryFunction>
UnaryFunction while_open(const std::string& path,
                         const std::string& mode,
                         UnaryFunction f) {
  auto file = TFile::Open(path.c_str(), mode.c_str());
  if (file && !file->IsZombie()) {
    file->cd();
    f(file);
    file->Close();
  }
  return std::move(f);
}
//----------------------------------------------------------------------------------------------

} /* namespace io */ ///////////////////////////////////////////////////////////////////////////

namespace string { /////////////////////////////////////////////////////////////////////////////

//__Split String on Delimeters__________________________________________________________________
template <class Range>
void split(const std::string& string,
           Range& tokens,
           const std::string& delimiters=" ") {
  const auto size = string.size();
  std::string::size_type position, previous = 0;
  while (previous <= size) {
    position = string.find_first_of(delimiters, previous);
    if (position == std::string::npos)
      position = size;
    tokens.emplace_back(string.data() + previous, position - previous);
    previous = position + 1;
  }
}
//----------------------------------------------------------------------------------------------

//__Join Range of Strings on Delimeters_________________________________________________________
template <class Range>
std::string join(const Range& tokens,
                 const std::string& delimeter="") {
  const auto begin = tokens.cbegin();
  const auto end = tokens.cend();
  if (begin == end)
    return "";

  std::string out;
  out.append(*begin);
  std::for_each(begin + 1, end, [&](const std::string& string) { out += delimeter + string; });
  return out;
}
//----------------------------------------------------------------------------------------------

//__Remove Leading and Trailing Spaces__________________________________________________________
inline std::string& strip(std::string& string) {
  const auto end = string.end();

  auto forward = string.cbegin();
  while (std::isspace(*forward) && forward != end) ++forward;

  auto reverse = string.crbegin();
  while (std::isspace(*reverse) && reverse.base() != forward) ++reverse;

  return string = std::string(forward, reverse.base());
}
//----------------------------------------------------------------------------------------------

} /* namespace string */ ///////////////////////////////////////////////////////////////////////

namespace hist { ///////////////////////////////////////////////////////////////////////////////

//__Convert 1D Histogram to CSV File____________________________________________________________
inline bool to_csv(const std::string& path,
                   const TH1* hist,
                   const char delimeter=',') {
  std::ofstream file(path);
  if (!file) return false;

  const auto size = hist->GetNbinsX();
  for (int i = 1; i <= size; ++i) {
    file << hist->GetXaxis()->GetBinLowEdge(i)
         << delimeter
         << hist->GetBinContent(i)
         << "\n";
  }
  return static_cast<bool>(file);
}
//----------------------------------------------------------------------------------------------

//__Convert 2D Histogram to CSV File____________________________________________________________
inline bool to_csv(const std::string& path,
                   const TH2* hist,
                   const char delimeter=',') {
  std::ofstream file(path);
  if (!file) return false;

  const auto x_size = hist->GetNbinsX();
  const auto y_size = hist->GetNbinsY();
  for (int i = 1; i <= x_size; ++i) {
    const auto x_data = hist->GetXaxis()->GetBinLowEdge(i);
    for (int j = 1; j <= y_size; ++j) {
      file << x_data
           << delimeter
           << hist->GetYaxis()->GetBinLowEdge(j)
           << hist->GetBinContent(i, j)
           << "\n";
    }
  }
  return static_cast<bool>(file);
}
//----------------------------------------------------------------------------------------------

//__Convert 3D Histogram to CSV File____________________________________________________________
inline bool to_csv(const std::string& path,
                   const TH3* hist,
                   const char delimeter=',') {
  std::ofstream file(path);
  if (!file) return false;

  const auto x_size = hist->GetNbinsX();
  const auto y_size = hist->GetNbinsY();
  const auto z_size = hist->GetNbinsZ();
  for (int i = 1; i <= x_size; ++i) {
    const auto x_data = hist->GetXaxis()->GetBinLowEdge(i);
    for (int j = 1; j <= y_size; ++j) {
      const auto y_data = hist->GetYaxis()->GetBinLowEdge(j);
      for (int k = 1; k <= z_size; ++k) {
        file << x_data
             << delimeter
             << y_data
             << delimeter
             << hist->GetZaxis()->GetBinLowEdge(k)
             << hist->GetBinContent(i, j)
             << "\n";
      }
    }
  }
  return static_cast<bool>(file);
}
//----------------------------------------------------------------------------------------------

} /* namespace hist */ /////////////////////////////////////////////////////////////////////////

namespace tree { ///////////////////////////////////////////////////////////////////////////////

//__Convert CSV File to TTree___________________________________________________________________
inline TTree* from_csv(const std::string& name,
                       const std::string& path,
                       const std::string& branch_format,
                       const char delimeter=',') {
  auto out = new TTree(name.c_str(), name.c_str());
  out->ReadFile(path.c_str(), branch_format.c_str(), delimeter);
  return out;
}
//----------------------------------------------------------------------------------------------

//__Get Tree from File__________________________________________________________________________
TTree* get(TFile* file,
           const std::string& name) {
  return dynamic_cast<TTree*>(file->Get(name.c_str()));
}
//----------------------------------------------------------------------------------------------

//__Set TTree Branch Base Function______________________________________________________________
void set_addresses(TTree* tree,
                   const std::string& name,
                   void* address) {
  tree->SetBranchAddress(name.c_str(), address);
}
template<class... Args>
void set_addresses(TTree* tree,
                   const std::string& name,
                   void* address,
                   Args&& ...args) {
  set_addresses(tree, name, address);
  set_addresses(tree, std::forward<Args>(args)...);
}
//----------------------------------------------------------------------------------------------

//__Linear Traverse Entries in TTree____________________________________________________________
template<class PreCondition, class Function>
std::pair<PreCondition, Function> traverse(std::vector<TTree*> trees,
                                           PreCondition pre_condition,
                                           Function f) {
  const auto size = trees.size();
  std::vector<int64_t> size_vector;
  size_vector.reserve(size);
  for (const auto& tree : trees) {
    const auto entries = tree->GetEntries();
    if (!pre_condition(entries)) {
      return std::make_pair(std::move(pre_condition), std::move(f));
    }
    size_vector.push_back(entries);
  }

  std::size_t counter{};
  bool active = true;
  for (; active; ++counter) {
    active = false;
    for (std::size_t i{}; i < size; ++i) {
      if (size_vector[i] > counter) {
        trees[i]->GetEntry(counter);
        active = true;
      }
    }
    f(counter);
  }

  return std::make_pair(std::move(pre_condition), std::move(f));
}
//----------------------------------------------------------------------------------------------

//__Linear Traverse Entries in TTree____________________________________________________________
template<class Function>
Function traverse(std::vector<TTree*> trees,
                  Function f) {
  return traverse(trees, [](const int64_t) { return true; }, f).second;
}
//----------------------------------------------------------------------------------------------

} /* namespace tree */ /////////////////////////////////////////////////////////////////////////

} /* namespace helper */ ///////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */

#endif /* MU__STUDIES_HELPER_HH */
