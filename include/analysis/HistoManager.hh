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
  static void FillHisto(size_t id, G4double bin, G4double weight=1.0);
  static void Normalize(size_t id, G4double fac);
  static void PrintStatistic();

private:
  HistoManager();
  static std::vector<TH1D*> _histograms;
  static std::vector<TFile*> _root_files;
};

} } /* namespace MATHUSLA::MU */

#endif /* ANALYSIS_HISTOMANAGER_HH */
