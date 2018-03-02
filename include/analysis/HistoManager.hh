#ifndef ANALYSIS_HISTOMANAGER_HH
#define ANALYSIS_HISTOMANAGER_HH
#pragma once

#include <array>

#include "Geant4/globals.hh"

#include "ROOT/TH1D.h"
#include "ROOT/TFile.h"
#include "ROOT/TTree.h"

namespace MATHUSLA { namespace MU {

class HistoManager {
public:
  static void Book();
  static void Save();
  static void FillHisto(G4int id, G4double bin, G4double weight=1.0);
  static void Normalize(G4int id, G4double fac);
  static void PrintStatistic();

private:
  HistoManager();
  constexpr static G4int MaxHistoCount = 1;
  static std::array<TH1D*, MaxHistoCount> fHisto;
  static TFile* fRootFile;
  static G4double fEsci;
};

} } /* namespace MATHUSLA::MU */

#endif /* ANALYSIS_HISTOMANAGER_HH */