/*
 * include/analysis.hh
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

#ifndef MU__ANALYSIS_HH
#define MU__ANALYSIS_HH
#pragma once

#include <vector>
#include <unordered_map>

#include <Geant4/g4root.hh>

namespace MATHUSLA { namespace MU {

namespace Analysis { ///////////////////////////////////////////////////////////////////////////

//__Simulation Setting Key-Value Pair___________________________________________________________
struct SimSetting { std::string name, text; };
using SimSettingList = std::vector<SimSetting>;
//----------------------------------------------------------------------------------------------

//__Simulation Setting Pseudo-Constructors______________________________________________________
inline SimSetting Setting(const std::string& name,
                          const std::string& text) {
  return {name, text};
}
inline SimSetting Setting(const std::string& prefix,
                          const std::string& name,
                          const std::string& text) {
  return {prefix + name, text};
}
//----------------------------------------------------------------------------------------------

namespace detail { /////////////////////////////////////////////////////////////////////////////

//__Add Setting to SimSettingList_______________________________________________________________
inline void AddSetting(SimSettingList& list,
                       const std::string& name,
                       const std::string& text) {
  list.push_back(Setting(name, text));
}
inline void AddSettingWithPrefix(SimSettingList& list,
                                 const std::string& prefix,
                                 const std::string& name,
                                 const std::string& text) {
  list.push_back(Setting(prefix, name, text));
}
//----------------------------------------------------------------------------------------------

//__Add Settings to SimSettingList______________________________________________________________
template<class ...Strings>
void AddSetting(SimSettingList& list,
                const std::string& name,
                const std::string& text,
                const Strings& ...rest) {
  AddSetting(list, name, text);
  AddSetting(list, rest...);
}
template<class ...Strings>
void AddSettingWithPrefix(SimSettingList& list,
                          const std::string& prefix,
                          const std::string& name,
                          const std::string& text,
                          const Strings& ...rest) {
  AddSettingWithPrefix(list, prefix, name, text);
  AddSettingWithPrefix(list, prefix, rest...);
}
//----------------------------------------------------------------------------------------------

} /* namespace detail */ ///////////////////////////////////////////////////////////////////////

//__Generate SimSettingList_____________________________________________________________________
template<class ...Strings>
SimSettingList Settings(const std::string& name,
                        const std::string& text,
                        const Strings& ...rest) {
  SimSettingList out;
  out.reserve(sizeof...(Strings));
  detail::AddSetting(out, name, text, rest...);
  return out;
}
template<class ...Strings>
SimSettingList Settings(const std::string& prefix,
                        const std::string& name,
                        const std::string& text,
                        const Strings& ...rest) {
  SimSettingList out;
  out.reserve(sizeof...(Strings));
  detail::AddSettingWithPrefix(out, prefix, name, text, rest...);
  return out;
}
//----------------------------------------------------------------------------------------------

//__Save Simulation Entries To File_____________________________________________________________
bool Save(const std::string& path,
          SimSetting entry);
bool Save(const std::string& path,
          SimSettingList entries);
//----------------------------------------------------------------------------------------------

namespace ROOT { ///////////////////////////////////////////////////////////////////////////////

//__Setup ROOT Analysis Tool____________________________________________________________________
void Setup();
//----------------------------------------------------------------------------------------------

//__Open Output File____________________________________________________________________________
bool Open(const std::string& path);
//----------------------------------------------------------------------------------------------

//__Save Output_________________________________________________________________________________
bool Save();
//----------------------------------------------------------------------------------------------

//__Data Entry Types____________________________________________________________________________
using DataEntryValueType = double;
using DataEntry = std::vector<DataEntryValueType>;
using DataEntryList = std::vector<DataEntry>;
using NameToDataMap = std::unordered_map<std::string, DataEntryValueType>;
using DataKey = std::string;
using DataKeyList = std::vector<DataKey>;
enum class DataKeyType { Single, Vector };
using DataKeyTypeList = std::vector<DataKeyType>;
static const DataKeyList DefaultDataKeyList{
  "N_HITS",
  "Deposit", "Time", "Detector",
  "PDG", "Track", "Parent", "X", "Y", "Z", "E", "PX", "PY", "PZ",
  "N_GEN",
  "GEN_PDG", "GEN_Track", "GEN_Parent",
  "GEN_T", "GEN_X", "GEN_Y", "GEN_Z", "GEN_E", "GEN_PX", "GEN_PY", "GEN_PZ"
};
static const DataKeyTypeList DefaultDataKeyTypeList{
  DataKeyType::Single,

  DataKeyType::Vector,
  DataKeyType::Vector,
  DataKeyType::Vector,
  DataKeyType::Vector,
  DataKeyType::Vector,
  DataKeyType::Vector,
  DataKeyType::Vector,
  DataKeyType::Vector,
  DataKeyType::Vector,
  DataKeyType::Vector,
  DataKeyType::Vector,
  DataKeyType::Vector,
  DataKeyType::Vector,

  DataKeyType::Single,

  DataKeyType::Vector,
  DataKeyType::Vector,
  DataKeyType::Vector,
  DataKeyType::Vector,
  DataKeyType::Vector,
  DataKeyType::Vector,
  DataKeyType::Vector,
  DataKeyType::Vector,
  DataKeyType::Vector,
  DataKeyType::Vector,
  DataKeyType::Vector
};
//----------------------------------------------------------------------------------------------

//__NTuple Initializer__________________________________________________________________________
bool CreateNTuple(const std::string& name,
                  const DataKeyList& columns,
                  const DataKeyTypeList& types);
//----------------------------------------------------------------------------------------------

//__Add Data to NTuple__________________________________________________________________________
bool FillNTuple(const std::string& name,
                const DataKeyTypeList& types,
                const DataEntry& single_values,
                const DataEntryList& vector_values);
//----------------------------------------------------------------------------------------------

} /* namespace ROOT */ /////////////////////////////////////////////////////////////////////////

} /* namespace Analysis */ /////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */

#endif /* MU__ANALYSIS_HH */
