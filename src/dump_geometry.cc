#include "Geant4/G4VPhysicalVolume.hh"
#include "Geant4/G4ThreeVector.hh"
#include "Geant4/G4RotationMatrix.hh"

#include "geometry/Construction.hh"

#include <iostream>
#include <string>

void dump_volume(const G4VPhysicalVolume &physical_volume, const G4ThreeVector &parent_translation, const G4RotationMatrix &parent_rotation, const bool dump_everything = false) {
  const auto &logical_volume = *(physical_volume.GetLogicalVolume());
  const auto n_daughters = logical_volume.GetNoDaughters();

  const auto name = logical_volume.GetName();

  //const auto translation = parent_translation + parent_rotation.inverse() * physical_volume.GetTranslation();
  const auto translation = parent_translation + parent_rotation * physical_volume.GetTranslation();

  auto rotation = parent_rotation;
  const auto *const relative_rotation_ptr = physical_volume.GetRotation();
  if (relative_rotation_ptr != nullptr) {
    //rotation = (*relative_rotation_ptr) * rotation;
    rotation = rotation * (*relative_rotation_ptr);
  }

  if (dump_everything || n_daughters == 0) {
    if ( ! dump_everything && (name.find("_C") != name.npos || name.find("_PMT") != name.npos)) {
      return;
    }
    std::cout << name;
    std::cout << "," << translation.x() << "," << translation.y() << "," << translation.z();
    std::cout << "," << rotation.thetaX() << "," << rotation.thetaY() << "," << rotation.thetaZ();
    std::cout << "," << rotation.phiX() << "," << rotation.phiY() << "," << rotation.phiZ();
    std::cout << std::endl;
  }
  for (G4int daughter_index = 0; daughter_index < n_daughters; daughter_index++) {
    const auto &daughter_volume = *(logical_volume.GetDaughter(daughter_index));
    dump_volume(daughter_volume, translation, rotation, dump_everything);
  }
}

void dump_world(const G4VPhysicalVolume &physical_volume, const bool dump_everything = false) {
  dump_volume(physical_volume, G4ThreeVector(0.0, 0.0, 0.0), G4RotationMatrix(0.0, 0.0, 0.0), dump_everything);
}

int main(const int argc, const char *const argv[]) {
  MATHUSLA::MU::Construction::Builder test("Prototype", "test-stand-geometry-dump", true);
  const auto &world = *(test.Construct());
  bool dump_everything = false;
  if (argc > 1 && std::string(argv[1]) == "all") {
    dump_everything = true;
  }
  dump_world(world, dump_everything);
  return 0;
}
