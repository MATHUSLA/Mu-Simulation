#ifndef MU__PHYSICS_FIELD_HH
#define MU__PHYSICS_FIELD_HH
#pragma once

#include "Geant4/G4MagneticField.hh"

namespace MATHUSLA { namespace MU {

class Field : public G4MagneticField {
public:
  void GetFieldValue(const double Point[4], double* fieldArr) const;
};

} } /* namespace MATHUSLA::MU */

#endif /* MU__PHYSICS_FIELD_HH */
