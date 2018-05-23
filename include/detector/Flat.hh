#ifndef MU__DETECTOR_FLAT_HH
#define MU__DETECTOR_FLAT_HH
#pragma once

#include "Geant4/G4VSensitiveDetector.hh"

#include "detector/Construction.hh"
#include "tracking.hh"

namespace MATHUSLA { namespace MU {

namespace Flat { ///////////////////////////////////////////////////////////////////////////////

class Scintillator {
public:
  Scintillator(const std::string& name,
               const double length,
               const double height,
               const double base_width,
               const double overlap);

  struct Material {
    static G4Material* Casing;
    static G4Material* Scintillator;
    static void Define();
  private:
    Material();
  };

  std::string name;
  double length;
  double height;
  double base_width;
  double overlap;
  G4LogicalVolume* lvolume;
  G4VPhysicalVolume* pvolume;
  G4VPhysicalVolume* sensitive;

  double GetFullWidth() const { return 2 * base_width - overlap; }

  static Scintillator* Clone(const Scintillator& other,
                             const std::string& new_name);

  constexpr static auto CasingThickness = 0.1*cm;
};

////////////////////////////////////////////////////////////////////////////////////////////////

class Layer {
public:
  Layer(const std::string& name,
        const size_t count,
        const Scintillator& scintillator);

  const std::string&  GetName()             const { return _name;          }
  G4LogicalVolume*    GetVolume()           const { return _volume;        }
  G4VPhysicalVolume*  GetPlacement()        const { return _placement;     }
  const Scintillator* GetScintillator()     const { return _scintillator;  }
  size_t              GetScintillatrCount() const { return _count;         }

  void Register(G4VSensitiveDetector* detector);

  G4VPhysicalVolume* PlaceIn(G4LogicalVolume* parent,
                             const G4Transform3D& transform=G4Transform3D());

  static Layer* Clone(const Layer& other,
                      const std::string& new_name);

  constexpr static auto ScintillatorSpacing = 0.3*cm;

private:
  const Scintillator* _scintillator;
  G4LogicalVolume* _volume;
  G4VPhysicalVolume* _placement;
  std::string _name;
  size_t _count;
};

////////////////////////////////////////////////////////////////////////////////////////////////

class Detector : public G4VSensitiveDetector {
public:
  Detector();

  void Initialize(G4HCofThisEvent* event);
  G4bool ProcessHits(G4Step* step, G4TouchableHistory*);
  void EndOfEvent(G4HCofThisEvent*);

  static G4VPhysicalVolume* Construct(G4LogicalVolume* world);
};

} /* namespace Flat */ /////////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */

#endif /* MU__DETECTOR_FLAT_HH */
