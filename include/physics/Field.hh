#ifndef PHYSICS_FIELD_HH
#define PHYSICS_FIELD_HH
#pragma once

#include "Geant4/globals.hh"
#include "Geant4/G4MagneticField.hh"
#include "Geant4/G4ThreeVector.hh"

namespace MATHUSLA { namespace MU {

class Field : public G4MagneticField {
public:
  void GetFieldValue(const G4double Point[4], G4double* fieldArr) const;
};

} } /* namespace MATHUSLA::MU */

#endif /* PHYSICS_FIELD_HH */
