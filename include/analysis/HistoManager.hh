#ifndef ANALYSIS_HISTOMANAGER_HH
#define ANALYSIS_HISTOMANAGER_HH 1
#pragma once

#include "ROOT/TH1D.h"
#include "ROOT/TFile.h"
#include "ROOT/TTree.h"
#include "Geant4/globals.hh"

const G4int kMaxHisto = 1;

class HistoManager
{
public:
	HistoManager();
   ~HistoManager();

    void Book();
    void Save();

    void FillHisto(G4int id, G4double bin, G4double weight = 1.0);
    void Normalize(G4int id, G4double fac);    
    
    void PrintStatistic();

private:
    TFile*   fRootFile;
    TH1D*    fHisto[kMaxHisto];                

    G4double fEsci;
};

#endif