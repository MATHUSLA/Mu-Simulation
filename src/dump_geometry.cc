#include "Geant4/G4VPhysicalVolume.hh"
#include "Geant4/G4ThreeVector.hh"
#include "Geant4/G4RotationMatrix.hh"

#include "geometry/Construction.hh"

#include <iostream>

void dump_volume(const G4VPhysicalVolume &physical_volume, const G4ThreeVector &parent_translation, const G4RotationMatrix &parent_rotation) {
  const auto &logical_volume = *(physical_volume.GetLogicalVolume());
  const auto n_daughters = logical_volume.GetNoDaughters();

  const auto name = logical_volume.GetName();

  const auto translation = parent_translation + parent_rotation.inverse() * physical_volume.GetTranslation();

  auto rotation = parent_rotation;
  const auto *const relative_rotation_ptr = physical_volume.GetRotation();
  if (relative_rotation_ptr != nullptr) {
    rotation = (*relative_rotation_ptr) * rotation;
  }

  if (n_daughters == 0) {
    if (name.find("_C") != name.npos || name.find("_PMT") != name.npos) {
      return;;
    }
    std::cout << name;
    std::cout << "," << translation.x() << "," << translation.y() << "," << translation.z();
    std::cout << "," << rotation.phiX() << "," << rotation.phiY() << "," << rotation.phiZ();
    std::cout << std::endl;
  } else {
    for (G4int daughter_index = 0; daughter_index < n_daughters; daughter_index++) {
      const auto &daughter_volume = *(logical_volume.GetDaughter(daughter_index));
      dump_volume(daughter_volume, translation, rotation);
    }
  }
}

int main() {
	MATHUSLA::MU::Construction::Builder test("Prototype", "test-stand-geometry-dump", true);
	const auto &world = *test.Construct();
	dump_volume(world, G4ThreeVector(0.0, 0.0, 0.0), G4RotationMatrix(0.0, 0.0, 0.0));
	return 0;
}
