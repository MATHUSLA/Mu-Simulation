/* src/analysis.cc
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

#include "analysis.hh"

#include <unordered_map>

#include <ROOT/TFile.h>
#include <ROOT/TNamed.h>

namespace MATHUSLA { namespace MU {

namespace Analysis { ///////////////////////////////////////////////////////////////////////////

//__Save Simulation Entries To File_____________________________________________________________
bool Save(const std::string& path,
          SimSetting entry) {
  TFile file(path.c_str(), "UPDATE");
  if (!file.IsZombie()) {
    file.cd();
    TNamed setting(entry.name.c_str(), entry.text.c_str());
    setting.Write();
    file.Close();
    return true;
  }
  return false;
}
bool Save(const std::string& path,
          SimSettingList entries) {
  TFile file(path.c_str(), "UPDATE");
  if (!file.IsZombie()) {
    file.cd();
    for (const auto entry : entries) {
      TNamed setting(entry.name.c_str(), entry.text.c_str());
      setting.Write();
    }
    file.Close();
    return true;
  }
  return false;
}
//----------------------------------------------------------------------------------------------

namespace ROOT { ///////////////////////////////////////////////////////////////////////////////

namespace { ////////////////////////////////////////////////////////////////////////////////////

//__NTuple Name-Id Map__________________________________________________________________________
G4ThreadLocal std::unordered_map<std::string, int> _ntuple;
//----------------------------------------------------------------------------------------------

//__NTuple Data Storage_________________________________________________________________________
G4ThreadLocal std::unordered_map<std::string, DataEntryList> _ntuple_data;
//----------------------------------------------------------------------------------------------



} /* anonymous namespace */ ////////////////////////////////////////////////////////////////////

//__Setup ROOT Analysis Tool____________________________________________________________________
void Setup() {
  _ntuple.clear();
  delete G4AnalysisManager::Instance();
  G4AnalysisManager::Instance()->SetNtupleMerging(false);
  G4AnalysisManager::Instance()->SetVerboseLevel(0);
}
//----------------------------------------------------------------------------------------------

//__Open Output File____________________________________________________________________________
bool Open(const std::string& path) {
  return G4AnalysisManager::Instance()->OpenFile(path);
}
//----------------------------------------------------------------------------------------------

//__Save Output_________________________________________________________________________________
bool Save() {
  return G4AnalysisManager::Instance()->Write() && G4AnalysisManager::Instance()->CloseFile();
}
//----------------------------------------------------------------------------------------------

//__Create ROOT NTuple__________________________________________________________________________
bool CreateNTuple(const std::string& name,
                  const std::vector<std::string>& columns) {
  const auto manager = G4AnalysisManager::Instance();
  const auto id = manager->CreateNtuple(name, name);
  const auto size = columns.size();

  DataEntryList data;
  data.resize(size, {});
  _ntuple_data.insert({name, data});
  auto& list = _ntuple_data[name];

  manager->CreateNtupleDColumn(id, "N");
  for (std::size_t i = 0; i < size; ++i)
    manager->CreateNtupleDColumn(id, columns[i], list[i]);
  manager->FinishNtuple(id);
  return _ntuple.insert({name, id}).second;
}
//----------------------------------------------------------------------------------------------

//__Fill ROOT NTuple____________________________________________________________________________
bool FillNTuple(const std::string& name,
                const DataEntryList& values) {
  const auto size = values.size();
  if (size == 0)
    return false;

  const auto search = _ntuple.find(name);
  if (search == _ntuple.cend())
    return false;

  auto& data = _ntuple_data[name];
  if (data.size() != size)
    return false;

  for (size_t i = 0; i < size; ++i)
    data[i] = values[i];

  const auto id = search->second;
  const auto manager = G4AnalysisManager::Instance();
  manager->FillNtupleDColumn(id, 0, data[0].size());
  manager->AddNtupleRow(id);
  return true;
}
//----------------------------------------------------------------------------------------------

} /* namespace ROOT */ /////////////////////////////////////////////////////////////////////////

} /* namespace Analysis */ /////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */
