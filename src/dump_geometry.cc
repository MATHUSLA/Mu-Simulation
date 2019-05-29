#include "Geant4/G4VPhysicalVolume.hh"
#include "Geant4/G4ThreeVector.hh"
#include "Geant4/G4RotationMatrix.hh"
#include "Geant4/G4Trap.hh"
#include "Geant4/G4Box.hh"

#include "geometry/Construction.hh"

#include <string>
#include <cctype>
#include <iostream>

bool is_scintillator_name(const std::string &name) {
  if (
       name.size() == 5
    && name[0] == 'S'
    && (name[1] == 'A' || name[1] == 'B')
    && std::isdigit(name[2])
    && name[3] == '-'
    && std::isdigit(name[4])
  ) {
    return true;
  } else {
    return false;
  }
}

bool is_strip_name(const std::string &name) {
  if (
       name.size() == 5
    && std::isdigit(name[0])
    && std::isdigit(name[1])
    && std::isdigit(name[2])
    && std::isdigit(name[3])
    && std::isdigit(name[4])
  ) {
    return true;
  } else {
    return false;
  }
}

void dump_volume(const G4VPhysicalVolume &physical_volume, const G4ThreeVector &parent_translation, const G4RotationMatrix &parent_rotation, const bool dump_everything = false) {
  const auto &logical_volume = *(physical_volume.GetLogicalVolume());
  const auto n_daughters = logical_volume.GetNoDaughters();

  const auto name = logical_volume.GetName();

  const auto translation = parent_translation + parent_rotation * physical_volume.GetTranslation();

  auto rotation = parent_rotation;
  const auto *const relative_rotation_ptr = physical_volume.GetRotation();
  if (relative_rotation_ptr != nullptr) {
    rotation = rotation * relative_rotation_ptr->inverse();
  }

  if (dump_everything || (n_daughters == 0 && (is_scintillator_name(name) || is_strip_name(name)))) {
    std::cout << name;
    std::cout.setf(std::ios::fixed);
    std::cout.precision(2);
    std::cout << "," << translation.x() << "," << translation.y() << "," << translation.z();
    std::cout.precision(5);
    std::cout << "," << rotation.thetaX() << "," << rotation.phiX();
    std::cout << "," << rotation.thetaY() << "," << rotation.phiY();
    std::cout << "," << rotation.thetaZ() << "," << rotation.phiZ();
    if (n_daughters == 0 && is_scintillator_name(name)) {
      const auto &trap = *static_cast<const G4Trap *>(logical_volume.GetSolid());
      std::cout << "," << 2.0 * trap.GetXHalfLength1();
      std::cout << "," << 2.0 * trap.GetXHalfLength2();
      std::cout << "," << 2.0 * trap.GetXHalfLength3();
      std::cout << "," << 2.0 * trap.GetXHalfLength4();
      std::cout << "," << 2.0 * trap.GetYHalfLength1();
      std::cout << "," << 2.0 * trap.GetYHalfLength2();
      std::cout << "," << 2.0 * trap.GetZHalfLength();
    } else if (is_strip_name(name)) {
      const auto &box = *static_cast<const G4Box *>(logical_volume.GetSolid());
      std::cout << "," << 2.0 * box.GetXHalfLength();
      std::cout << "," << 2.0 * box.GetYHalfLength();
      std::cout << "," << 2.0 * box.GetZHalfLength();
    }
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
  bool dump_everything = false;
  if (argc == 2 && std::string(argv[1]) == "all") {
    dump_everything = true;
  } else if (argc != 1) {
    std::cout << "Usage: " << argv[0] << " [all]" << std::endl;
    return 1;
  }
  MATHUSLA::MU::Construction::Builder test_stand_builder("Prototype", "test-stand-geometry-dump", true);
  const auto &world = *(test_stand_builder.Construct());
  std::cout << "# Format by column:" << std::endl << std::endl;
  std::cout << "# Volume name" << std::endl;
  std::cout << "# Global x-coordinate of center of volume (mm)" << std::endl;
  std::cout << "# Global y-coordinate of center of volume (mm)" << std::endl;
  std::cout << "# Global z-coordinate of center of volume (mm)" << std::endl;
  std::cout << "# Global zenith angle of local x-axis of volume (rad)" << std::endl;
  std::cout << "# Global azimuthal angle of local x-axis of volume (rad)" << std::endl;
  std::cout << "# Global zenith angle of local y-axis of volume (rad)" << std::endl;
  std::cout << "# Global azimuthal angle of local y-axis of volume (rad)" << std::endl;
  std::cout << "# Global zenith angle of local z-axis of volume (rad)" << std::endl;
  std::cout << "# Global azimuthal angle of local z-axis of volume (rad)" << std::endl;
  std::cout << "# (Additional detector-specific fields for dimensions of scintillators and strips) " << std::endl;
  std::cout << std::endl;
  dump_world(world, dump_everything);
  return 0;
}
