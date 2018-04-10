#include "analysis/AnalysisManager.hh"

#include <unordered_map>

namespace MATHUSLA { namespace MU {

static G4ThreadLocal std::unordered_map<std::string, G4int> _ntuple = {};

void AnalysisManager::Setup() {
  _ntuple.clear();
  delete G4AnalysisManager::Instance();
  G4AnalysisManager::Instance()->SetNtupleMerging(true);
  G4AnalysisManager::Instance()->SetVerboseLevel(0);
}

bool AnalysisManager::Open(const std::string& file) {
  return G4AnalysisManager::Instance()->OpenFile(file);
}

bool AnalysisManager::Save() {
  return G4AnalysisManager::Instance()->Write()
    && G4AnalysisManager::Instance()->CloseFile();
}

bool AnalysisManager::CreateNTuple(const std::string& name,
                                   const std::string& title,
                                   const std::initializer_list<const std::string> columns) {
  const auto manager = G4AnalysisManager::Instance();
  const auto id = manager->CreateNtuple(name, title);
  for (const auto& column : columns) {
    manager->CreateNtupleDColumn(id, column);
  }
  manager->FinishNtuple(id);
  return _ntuple.insert({name, id}).second;
}

bool AnalysisManager::CreateNTuple(const std::string& name,
                                   const std::initializer_list<const std::string> columns) {
  return CreateNTuple(name, name, columns);
}

bool AnalysisManager::FillNTuple(const std::string& name,
                                 const std::initializer_list<const G4double> values) {
  if (values.size() <= 0)
    return false;

  const auto search = _ntuple.find(name);
  if (search == _ntuple.end())
    return false;

  const auto manager = G4AnalysisManager::Instance();
  const auto ntuple_id = search->second;
  const auto iter = values.begin();
  for (size_t i = 0; i < values.size(); ++i) {
    if (!manager->FillNtupleDColumn(ntuple_id, i, iter[i]))
      return false;
  }
  manager->AddNtupleRow(ntuple_id);
  return true;
}

} } /* namespace MATHUSLA::MU */
