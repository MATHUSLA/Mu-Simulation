#include <CLHEP/Units/SystemOfUnits.h>

#include "analysis/HistoManager.hh"
#include "Geant4/G4UnitsTable.hh"

HistoManager::HistoManager()
:fRootFile(0),
fEsci(0.)
{for (G4int k=0; k<kMaxHisto; k++) fHisto[k] = 0;}

HistoManager::~HistoManager()
{
  if (fRootFile) delete fRootFile;
}

void HistoManager::Book()
{
  G4String fileName = "Prototype.root";
  fRootFile = new TFile(fileName,"EnergyDeposit");
  if (! fRootFile) {
    G4cout << " HistoManager::Book :" 
           << " problem creating the ROOT TFile "
           << G4endl;
       }
    return;
  for ( G4int i=0; i<kMaxHisto; ++i ) {
    if (! fHisto[i]) G4cout << "\n can't create histo " << i << G4endl;
  }
}

void HistoManager::Save()
{ 
  if (! fRootFile) return;
  
  fRootFile->Write();       // Writing the histograms to the file
  fRootFile->Close();       // and closing the tree (and the file)
  
  G4cout << "\n----> Histograms are saved\n" << G4endl;
}

void HistoManager::FillHisto(G4int ih, G4double xbin, G4double weight)
{
  if (ih >= kMaxHisto) {
    G4cerr << "---> warning from HistoManager::FillHisto() : histo " << ih
           << " does not exist. (xbin=" << xbin << " weight=" << weight << ")"
           << G4endl;
    return;
  }
  if  (fHisto[ih]) { fHisto[ih]->Fill(xbin, weight); }
}

void HistoManager::Normalize(G4int ih, G4double fac)
{
  if (ih >= kMaxHisto) {
    G4cout << "---> warning from HistoManager::Normalize() : histo " << ih
           << " does not exist. (fac=" << fac << ")" << G4endl;
    return;
  }
  if (fHisto[ih]) fHisto[ih]->Scale(fac);
}

void HistoManager::PrintStatistic()
{}
