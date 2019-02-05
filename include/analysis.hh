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
struct SimSetting {
  std::string name, text;
  SimSetting() = default;
  SimSetting(const std::string& setting_name,
             const std::string& setting_text) : name(setting_name), text(setting_text) {}
  SimSetting(const std::string& prefix,
             const std::string& setting_name,
             const std::string& setting_text) : name(prefix + setting_name), text(setting_text) {}
};
using SimSettingList = std::vector<SimSetting>;
//----------------------------------------------------------------------------------------------

namespace detail { /////////////////////////////////////////////////////////////////////////////

//__Add Setting to SimSettingList_______________________________________________________________
inline void AddSetting(SimSettingList& list,
                       const std::string& name,
                       const std::string& text) {
  list.emplace_back(name, text);
}
inline void AddSettingWithPrefix(SimSettingList& list,
                                 const std::string& prefix,
                                 const std::string& name,
                                 const std::string& text) {
  list.emplace_back(prefix, name, text);
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
//----------------------------------------------------------------------------------------------

//__Generate SimSettingList_____________________________________________________________________
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

//__Generate SimSettingList_____________________________________________________________________
inline SimSettingList Settings(const std::vector<std::string>& names,
                               const std::vector<std::string>& texts) {
  const auto name_count = names.size();
  const auto text_count = texts.size();
  if (name_count != text_count || name_count == 0UL || text_count == 0UL)
    return SimSettingList{};

  SimSettingList out;
  out.reserve(name_count);
  for (std::size_t i{}; i < name_count; ++i)
    detail::AddSetting(out, names[i], texts[i]);
  return out;
}
//----------------------------------------------------------------------------------------------

//__Generate SimSettingList_____________________________________________________________________
inline SimSettingList Settings(const std::string& prefix,
                               const std::vector<std::string>& names,
                               const std::vector<std::string>& texts) {
  const auto name_count = names.size();
  const auto text_count = texts.size();
  if (name_count != text_count || name_count == 0UL || text_count == 0UL)
    return SimSettingList{};

  SimSettingList out;
  out.reserve(name_count);
  for (std::size_t i{}; i < name_count; ++i)
    detail::AddSettingWithPrefix(out, prefix, names[i], texts[i]);
  return out;
}
//----------------------------------------------------------------------------------------------

//__Generate Indexed SimSettingList_____________________________________________________________
inline SimSettingList IndexedSettings(const std::string& name,
                                      const std::vector<std::string>& texts,
                                      const std::size_t starting_index=0UL) {
  const auto text_count = texts.size();
  if (text_count == 0UL)
    return SimSettingList{};

  SimSettingList out;
  out.reserve(text_count);
  for (std::size_t i{}; i < text_count; ++i)
    detail::AddSetting(out, name + std::to_string(starting_index + i), texts[i]);
  return out;
}
//----------------------------------------------------------------------------------------------

//__Generate Indexed SimSettingList_____________________________________________________________
inline SimSettingList IndexedSettings(const std::string& prefix,
                                      const std::string& name,
                                      const std::vector<std::string>& texts,
                                      const std::size_t starting_index=0UL) {
  const auto text_count = texts.size();
  if (text_count == 0UL)
    return SimSettingList{};

  SimSettingList out;
  out.reserve(text_count);
  for (std::size_t i{}; i < text_count; ++i)
    detail::AddSettingWithPrefix(out, prefix, name + std::to_string(starting_index + i), texts[i]);
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
  "PDG", "Track", "Parent",
  "X", "Y", "Z",
  "E", "PX", "PY", "PZ",
  "WEIGHT",

  "N_GEN",

  "GEN_PDG", "GEN_Track", "GEN_Parent",
  "GEN_T", "GEN_X", "GEN_Y", "GEN_Z",
  "GEN_E", "GEN_PX", "GEN_PY", "GEN_PZ",
  "GEN_WEIGHT",

  "EXTRA_00", "EXTRA_01", "EXTRA_02", "EXTRA_03",
  "EXTRA_04", "EXTRA_05", "EXTRA_06", "EXTRA_07",
  "EXTRA_08", "EXTRA_09", "EXTRA_10", "EXTRA_11",
  "EXTRA_12", "EXTRA_13", "EXTRA_14", "EXTRA_15"
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
