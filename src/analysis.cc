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
                  const std::initializer_list<const std::string> columns) {
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
                const std::initializer_list<double> values) {
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

} /* namespace Analysis */ /////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */
