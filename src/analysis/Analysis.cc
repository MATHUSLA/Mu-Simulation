#include "analysis/Analysis.hh"

#include <unordered_map>

namespace MATHUSLA { namespace MU {

static std::unordered_map<std::string, G4int> _ntuple;
static std::unordered_map<std::string, G4int> _ntuple_cols;
static std::unordered_map<std::string, G4int> _h1d;
static std::unordered_map<std::string, G4int> _h2d;

G4bool Analysis::OpenFile(const std::string& file) {
  return G4AnalysisManager::Instance()->OpenFile(file);
}

G4bool Analysis::OpenFile(const std::string& dir, const std::string& file) {
  return OpenFile(dir + "/" + file);
}

G4bool Analysis::Write() {
  return G4AnalysisManager::Instance()->Write();
}

G4bool Analysis::CloseFile() {
  return G4AnalysisManager::Instance()->CloseFile();
}

G4bool Analysis::CreateNTuple(const std::string& name,
                              const std::string& title,
                              const std::initializer_list<const std::string> columns) {
  auto manager = G4AnalysisManager::Instance();
  auto id = manager->CreateNtuple(name, title);
  for (const auto& column : columns) {
    auto col_id = manager->CreateNtupleDColumn(id, column);
    _ntuple_cols.insert({name + column, col_id});
  }
  manager->FinishNtuple(id);
  return _ntuple.insert({name, id}).second;
}

G4bool Analysis::CreateNTuple(const std::string& name,
                              const std::initializer_list<const std::string> columns) {
  return CreateNTuple(name, name, columns);
}

G4bool Analysis::CreateNTuple(const std::string& name,
                              const std::string& title) {
  return CreateNTuple(name, title, {});
}

G4bool Analysis::CreateNTuple(const std::string& name) {
  return CreateNTuple(name, name);
}

G4bool Analysis::FillNTupleColumn(const std::string& name,
                                  const std::string& column,
                                  const G4double value) {
  auto manager = G4AnalysisManager::Instance();
  auto out = manager->FillNtupleDColumn(_ntuple[name], _ntuple_cols[name + column], value);
  manager->AddNtupleRow(_ntuple[name]);
  return out;
}

G4bool Analysis::FillNTuple(const std::string& name,
                            const std::initializer_list<const G4double> values) {
  if (values.size() <= 0)
    return false;

  auto manager = G4AnalysisManager::Instance();
  auto ntuple_id = _ntuple[name];
  auto iter = values.begin();
  for (size_t i = 0; i < values.size(); ++i) {
    if (!manager->FillNtupleDColumn(ntuple_id, i, iter[i]))
      return false;
  }
  manager->AddNtupleRow(ntuple_id);
  return true;
}

G4bool Analysis::CreateH1(const std::string& name,
                          const std::string& title,
                          const G4int nbins,
                          const G4double xmin,
                          const G4double xmax) {
  auto id = G4AnalysisManager::Instance()->CreateH1(name, title, nbins, xmin, xmax);
  return _h1d.insert({name, id}).second;
}

G4int Analysis::CreateH2(const std::string& name,
                         const std::string& title,
                         const G4int nxbins,
                         const G4double xmin,
                         const G4double xmax,
                         const G4int nybins,
                         const G4double ymin,
                         const G4double ymax) {
  auto id = G4AnalysisManager::Instance()->CreateH2(
    name, title, nxbins, xmin, xmax, nybins, ymin, ymax);
  return _h2d.insert({name, id}).second;
}

G4bool Analysis::CreateH1(const std::string& name,
                          const G4int nbins,
                          const G4double xmin,
                          const G4double xmax) {
  return CreateH1(name, name, nbins, xmin, xmax);
}

G4int Analysis::CreateH2(const std::string& name,
                         const G4int nxbins,
                         const G4double xmin,
                         const G4double xmax,
                         const G4int nybins,
                         const G4double ymin,
                         const G4double ymax) {
  return CreateH2(name, name, nxbins, xmin, xmax, nybins, ymin, ymax);
}

G4bool Analysis::FillH1(const std::string& name,
                        const G4double value,
                        const G4double weight) {
  return G4AnalysisManager::Instance()->FillH1(_h1d[name], value, weight);
}

G4bool Analysis::FillH2(const std::string& name,
                        const G4double xvalue,
                        const G4double yvalue,
                        const G4double weight) {
  return G4AnalysisManager::Instance()->FillH2(_h1d[name], xvalue, yvalue, weight);
}

G4bool Analysis::ScaleH1(const std::string& name,
                         const G4double factor) {
  return G4AnalysisManager::Instance()->ScaleH1(_h1d[name], factor);
}

G4bool Analysis::ScaleH2(const std::string& name,
                         const G4double factor) {
  return G4AnalysisManager::Instance()->ScaleH1(_h2d[name], factor);
}

} } /* namespace MATHUSLA::MU */
