#ifndef ANALYSIS_ANALYSIS_HH
#define ANALYSIS_ANALYSIS_HH
#pragma once

#include <string>

#include "Geant4/g4root.hh"

namespace MATHUSLA { namespace MU {

namespace Analysis {

G4bool OpenFile(const std::string& file);

G4bool OpenFile(const std::string& dir, const std::string& file);

G4bool Write();

G4bool CloseFile();

G4bool CreateH1(const std::string& name,
                const std::string& title,
                G4int nbins,
                G4double xmin,
                G4double xmax);

G4int CreateH2(const std::string& name,
               const std::string& title,
               G4int nxbins,
               G4double xmin,
               G4double xmax,
               G4int nybins,
               G4double ymin,
               G4double ymax);

G4bool CreateH1(const std::string& name,
                G4int nbins,
                G4double xmin,
                G4double xmax);

G4int CreateH2(const std::string& name,
               G4int nxbins,
               G4double xmin,
               G4double xmax,
               G4int nybins,
               G4double ymin,
               G4double ymax);

G4bool FillH1(const std::string& name,
              G4double value,
              G4double weight = 1.0);

G4bool FillH2(const std::string& name,
              G4double xvalue,
              G4double yvalue,
              G4double weight = 1.0);

G4bool ScaleH1(const std::string& name,
               G4double factor);

G4bool ScaleH2(const std::string& name,
              G4double factor);

} /* namespace Analysis */

} } /* namespace MATHUSLA::MU */

#endif /* ANALYSIS_ANALYSIS_HH */
