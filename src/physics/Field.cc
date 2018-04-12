#include "physics/Field.hh"

#include "Geant4/G4SystemOfUnits.hh"

namespace MATHUSLA { namespace MU {

void Field::GetFieldValue(const double Point[4], double* fieldArr) const {
  const double rmax_sq = sqr(1*m);
  const double zmax = 1*m;
  const double Bz = 3*tesla;

  fieldArr[0] = 0;
  fieldArr[1] = 0;
  if (std::abs(Point[2]) < zmax && (sqr(Point[0]) + sqr(Point[1])) < rmax_sq)
    fieldArr[2] = Bz;
}

} } /* namespace MATHUSLA::MU */
