#include "analysis/Analysis.hh"

#include <unordered_map>

namespace MATHUSLA { namespace MU {

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

G4bool Analysis::CreateH1(const std::string& name,
                          const std::string& title,
                          G4int nbins,
                          G4double xmin,
                          G4double xmax) {
  auto id = G4AnalysisManager::Instance()->CreateH1(
    name, title, nbins, xmin, xmax);
  return _h1d.insert({name, id}).second;
}

G4int Analysis::CreateH2(const std::string& name,
                         const std::string& title,
                         G4int nxbins,
                         G4double xmin,
                         G4double xmax,
                         G4int nybins,
                         G4double ymin,
                         G4double ymax) {
  auto id = G4AnalysisManager::Instance()->CreateH2(
    name, title, nxbins, xmin, xmax, nybins, ymin, ymax);
  return _h2d.insert({name, id}).second;
}

G4bool Analysis::CreateH1(const std::string& name,
                          G4int nbins,
                          G4double xmin,
                          G4double xmax) {
  return CreateH1(name, name, nbins, xmin, xmax);
}

G4int Analysis::CreateH2(const std::string& name,
                         G4int nxbins,
                         G4double xmin,
                         G4double xmax,
                         G4int nybins,
                         G4double ymin,
                         G4double ymax) {
  return CreateH2(name, name, nxbins, xmin, xmax, nybins, ymin, ymax);
}

G4bool Analysis::FillH1(const std::string& name,
                        G4double value,
                        G4double weight) {
  return G4AnalysisManager::Instance()->FillH1(_h1d[name], value, weight);
}

G4bool Analysis::FillH2(const std::string& name,
                        G4double xvalue,
                        G4double yvalue,
                        G4double weight) {
  return G4AnalysisManager::Instance()->FillH2(
    _h1d[name], xvalue, yvalue, weight);
}

G4bool Analysis::ScaleH1(const std::string& name, G4double factor) {
  return G4AnalysisManager::Instance()->ScaleH1(_h1d[name], factor);
}

G4bool Analysis::ScaleH2(const std::string& name, G4double factor) {
  return G4AnalysisManager::Instance()->ScaleH1(_h2d[name], factor);
}

} } /* namespace MATHUSLA::MU */