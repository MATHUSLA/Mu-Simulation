#ifndef ANALYSIS_ANALYSISMANAGER_HH
#define ANALYSIS_ANALYSISMANAGER_HH
#pragma once

#include <string>

#include "Geant4/g4root.hh"

namespace MATHUSLA { namespace MU {

namespace AnalysisManager {

bool OpenFile(const std::string& file);

bool OpenFile(const std::string& dir, const std::string& file);

bool Write();

bool CloseFile();

bool CreateNTuple(const std::string& name,
                  const std::string& title,
                  const std::initializer_list<const std::string> columns);

bool CreateNTuple(const std::string& name,
                  const std::initializer_list<const std::string> columns);

bool CreateNTuple(const std::string& name,
                  const std::string& title);

bool CreateNTuple(const std::string& name);

bool FillNTupleColumn(const std::string& name,
                      const std::string& column,
                      const G4double value);

bool FillNTuple(const std::string& name,
                const std::initializer_list<const G4double> values);

bool CreateH1(const std::string& name,
              const std::string& title,
              const G4int nbins,
              const G4double xmin,
              const G4double xmax);

bool CreateH2(const std::string& name,
              const std::string& title,
              const G4int nxbins,
              const G4double xmin,
              const G4double xmax,
              const G4int nybins,
              const G4double ymin,
              const G4double ymax);

bool CreateH1(const std::string& name,
              const G4int nbins,
              const G4double xmin,
              const G4double xmax);

bool CreateH2(const std::string& name,
              const G4int nxbins,
              const G4double xmin,
              const G4double xmax,
              const G4int nybins,
              const G4double ymin,
              const G4double ymax);

bool FillH1(const std::string& name,
              const G4double value,
              const G4double weight = 1.0);

bool FillH2(const std::string& name,
            const G4double xvalue,
            const G4double yvalue,
            const G4double weight = 1.0);

bool ScaleH1(const std::string& name,
             const G4double factor);

bool ScaleH2(const std::string& name,
             const G4double factor);

} /* namespace AnalysisManager */

} } /* namespace MATHUSLA::MU */

#endif /* ANALYSIS_ANALYSISMANAGER_HH */
