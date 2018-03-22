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

G4bool CreateNTuple(const std::string& name,
                    const std::string& title,
                    const std::initializer_list<const std::string> columns);

G4bool CreateNTuple(const std::string& name,
                    const std::initializer_list<const std::string> columns);

G4bool CreateNTuple(const std::string& name,
                    const std::string& title);

G4bool CreateNTuple(const std::string& name);

G4bool FillNTupleColumn(const std::string& name,
                        const std::string& column,
                        const G4double value);

G4bool FillNTuple(const std::string& name,
                  const std::initializer_list<const G4double> values);

G4bool CreateH1(const std::string& name,
                const std::string& title,
                const G4int nbins,
                const G4double xmin,
                const G4double xmax);

G4int CreateH2(const std::string& name,
               const std::string& title,
               const G4int nxbins,
               const G4double xmin,
               const G4double xmax,
               const G4int nybins,
               const G4double ymin,
               const G4double ymax);

G4bool CreateH1(const std::string& name,
                const G4int nbins,
                const G4double xmin,
                const G4double xmax);

G4int CreateH2(const std::string& name,
               const G4int nxbins,
               const G4double xmin,
               const G4double xmax,
               const G4int nybins,
               const G4double ymin,
               const G4double ymax);

G4bool FillH1(const std::string& name,
              const G4double value,
              const G4double weight = 1.0);

G4bool FillH2(const std::string& name,
              const G4double xvalue,
              const G4double yvalue,
              const G4double weight = 1.0);

G4bool ScaleH1(const std::string& name,
               const G4double factor);

G4bool ScaleH2(const std::string& name,
               const G4double factor);

} /* namespace Analysis */

} } /* namespace MATHUSLA::MU */

#endif /* ANALYSIS_ANALYSIS_HH */
