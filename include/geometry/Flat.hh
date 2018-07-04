/* include/geometry/Flat.hh
 *
 * Copyright 2018 Brandon Gomes
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MU__GEOMETRY_FLAT_HH
#define MU__GEOMETRY_FLAT_HH
#pragma once

#include "Geant4/G4VSensitiveDetector.hh"

#include "geometry/Construction.hh"
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

  static Scintillator* Clone(const Scintillator* other,
                             const std::string& new_name);

  constexpr static auto CasingThickness = 0.1*cm;
};

using ScintillatorList = std::vector<Scintillator*>;

////////////////////////////////////////////////////////////////////////////////////////////////

class Layer {
public:
  Layer(const std::string& name,
        const size_t count,
        const Scintillator* scintillator);

  const std::string&      GetName()              const { return _name;          }
  G4LogicalVolume*        GetVolume()            const { return _volume;        }
  G4VPhysicalVolume*      GetPlacement()         const { return _placement;     }
  size_t                  GetScintillatorCount() const { return _count;         }
  const ScintillatorList& GetScintillators()     const { return _scintillators; }

  void Register(G4VSensitiveDetector* detector);

  G4VPhysicalVolume* PlaceIn(G4LogicalVolume* parent,
                             const G4Transform3D& transform=G4Transform3D());

  static Layer* Clone(const Layer& other,
                      const std::string& new_name);

  constexpr static auto ScintillatorSpacing = 0.3*cm;

private:
  std::vector<Scintillator*> _scintillators;
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

  static const bool DataPerEvent = true;
  static const std::string& DataName;
  static const Analysis::ROOT::DataKeyList DataKeys;
  static const Analysis::ROOT::DataKeyTypeList DataKeyTypes;

  static G4VPhysicalVolume* Construct(G4LogicalVolume* world);
  static G4VPhysicalVolume* ConstructEarth(G4LogicalVolume* world);
};

} /* namespace Flat */ /////////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */

#endif /* MU__GEOMETRY_FLAT_HH */
