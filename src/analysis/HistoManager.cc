#include "analysis/HistoManager.hh"

#include "Geant4/G4UnitsTable.hh"

namespace MATHUSLA { namespace MU {

std::array<TH1D*, HistoManager::MaxHistoCount> HistoManager::fHisto = {};
TFile* HistoManager::fRootFile = nullptr;
G4double HistoManager::fEsci = 0;

void HistoManager::Book() {
  fRootFile = new TFile("data/Prototype.root", "RECREATE", "EnergyDeposit");
  if (!fRootFile) {
    G4cout << " HistoManager::Book : problem creating the ROOT TFile\n";
    return;
  }
  for (G4int i = 0; i < MaxHistoCount; ++i) {
    if (!fHisto[i]) G4cout << "\n can't create histo " << i << '\n';
  }
}

void HistoManager::Save() {
  if (!fRootFile) return;
  fRootFile->Write();       // Writing the histograms to the file
  fRootFile->Close();       // and closing the tree (and the file)
  G4cout << "\n----> Histograms are saved\n\n";
}

void HistoManager::FillHisto(G4int id, G4double xbin, G4double weight) {
  if (id >= MaxHistoCount) {
    G4cout << "---> warning from HistoManager::FillHisto() : histo " << id
           << " does not exist. (xbin=" << xbin << " weight=" << weight
           << ")\n";
    return;
  }
  if (fHisto[id]) { fHisto[id]->Fill(xbin, weight); }
}

void HistoManager::Normalize(G4int id, G4double fac) {
  if (id >= MaxHistoCount) {
    G4cout << "---> warning from HistoManager::Normalize() : histo " << id
           << " does not exist. (fac=" << fac << ")\n";
    return;
  }
  if (fHisto[id]) fHisto[id]->Scale(fac);
}

void HistoManager::PrintStatistic() {}

} } /* namespace MATHUSLA::MU */
