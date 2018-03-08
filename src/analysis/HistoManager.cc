#include "analysis/HistoManager.hh"

namespace MATHUSLA { namespace MU {

std::vector<TH1D*> HistoManager::_histograms = {};
std::vector<TFile*> HistoManager::_root_files = {};

void HistoManager::Book() {
  _root_files[0] = new TFile("data/Prototype.root", "RECREATE", "EnergyDeposit");
  if (!_root_files[0]) {
    G4cout << " HistoManager::Book : problem creating the ROOT TFile\n";
    return;
  }
  for (auto& histogram : _histograms) {
    if (!histogram) G4cout << "\n can't create histograms \n";
  }
}

void HistoManager::Save() {
  if (!_root_files[0]) return;
  _root_files[0]->Write();       // Writing the histograms to the file
  _root_files[0]->Close();       // and closing the tree (and the file)
  G4cout << "\n----> Histograms are saved\n\n";
}

void HistoManager::FillHisto(size_t id, G4double xbin, G4double weight) {
  if (id >= _histograms.size()) {
    G4cout << "---> warning from HistoManager::FillHisto() : histo " << id
           << " does not exist. (xbin=" << xbin << " weight=" << weight
           << ")\n";
    return;
  }
  if (_histograms[id]) {
    _histograms[id]->Fill(xbin, weight);
  }
}

void HistoManager::Normalize(size_t id, G4double fac) {
  if (id >= _histograms.size()) {
    G4cout << "---> warning from HistoManager::Normalize() : histo " << id
           << " does not exist. (fac=" << fac << ")\n";
    return;
  }
  if (_histograms[id])
    _histograms[id]->Scale(fac);
}

void HistoManager::PrintStatistic() {}

} } /* namespace MATHUSLA::MU */
