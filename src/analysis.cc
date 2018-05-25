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

namespace MATHUSLA { namespace MU {

namespace Analysis { ///////////////////////////////////////////////////////////////////////////

namespace { ////////////////////////////////////////////////////////////////////////////////////
//__NTuple Name-Id Map__________________________________________________________________________
G4ThreadLocal std::unordered_map<std::string, int> _ntuple;
//----------------------------------------------------------------------------------------------
} /* anonymous namespace */ ////////////////////////////////////////////////////////////////////

//__Setup Analysis Tool_________________________________________________________________________
void Setup() {
  _ntuple.clear();
  delete G4AnalysisManager::Instance();
  G4AnalysisManager::Instance()->SetNtupleMerging(true);
  G4AnalysisManager::Instance()->SetVerboseLevel(0);
}
//----------------------------------------------------------------------------------------------

//__Open Output File____________________________________________________________________________
bool Open(const std::string& file) {
  return G4AnalysisManager::Instance()->OpenFile(file);
}
//----------------------------------------------------------------------------------------------

//__Save Output_________________________________________________________________________________
bool Save() {
  return G4AnalysisManager::Instance()->Write() && G4AnalysisManager::Instance()->CloseFile();
}
//----------------------------------------------------------------------------------------------

//__Create ROOT NTuple__________________________________________________________________________
bool CreateNTuple(const std::string& name,
                  const std::vector<std::string> columns) {
  const auto manager = G4AnalysisManager::Instance();
  const auto id = manager->CreateNtuple(name, name);
  for (const auto& column : columns)
    manager->CreateNtupleDColumn(id, column);
  manager->FinishNtuple(id);
  return _ntuple.insert({name, id}).second;
}
//----------------------------------------------------------------------------------------------

//__Fill ROOT NTuple____________________________________________________________________________
bool FillNTuple(const std::string& name,
                const std::vector<double> values) {
  if (values.size() <= 0)
    return false;

  const auto search = _ntuple.find(name);
  if (search == _ntuple.end())
    return false;

  const auto manager = G4AnalysisManager::Instance();
  const auto ntuple_id = search->second;
  const auto iter = values.begin();
  for (size_t i = 0; i < values.size(); ++i)
    if (!manager->FillNtupleDColumn(ntuple_id, i, iter[i]))
      return false;
  manager->AddNtupleRow(ntuple_id);
  return true;
}
//----------------------------------------------------------------------------------------------

//__Add Data to NTuple__________________________________________________________________________
bool FillNTuple(const std::string& prefix,
                const size_t id,
                const std::vector<double> values) {
  return FillNTuple(prefix + std::to_string(id), values);
}
//----------------------------------------------------------------------------------------------

//__NTuple Collection Initializer_______________________________________________________________
bool GenerateNTupleCollection(const size_t count,
                              const std::string& prefix,
                              const std::vector<std::string> columns) {
  bool pass = true;
  for (size_t i = 0; i < count; ++i)
    pass = pass && Analysis::CreateNTuple(prefix + std::to_string(i), columns);
  return pass;
}
//----------------------------------------------------------------------------------------------

} /* namespace Analysis */ /////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */
