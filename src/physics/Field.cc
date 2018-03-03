#include "physics/Field.hh"

#include "Geant4/G4SystemOfUnits.hh"

namespace MATHUSLA { namespace MU {

void Field::GetFieldValue(const G4double Point[4], G4double* fieldArr) const {
  const G4double rmax_sq = sqr(1*m);
  const G4double zmax = 1*m;
  const G4double Bz = 3*tesla;

  fieldArr[0] = 0;
  fieldArr[1] = 0;
  if (std::abs(Point[2]) < zmax && (sqr(Point[0]) + sqr(Point[1])) < rmax_sq)
    fieldArr[2] = Bz;
}

} } /* namespace MATHUSLA::MU */
