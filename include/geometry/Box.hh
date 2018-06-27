/* include/geometry/Box.hh
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

#ifndef MU__GEOMETRY_BOX_HH
#define MU__GEOMETRY_BOX_HH
#pragma once

#include "Geant4/G4VSensitiveDetector.hh"

#include "geometry/Construction.hh"

namespace MATHUSLA { namespace MU {

namespace Box { ////////////////////////////////////////////////////////////////////////////////

class Scintillator {
public:
  Scintillator(const std::string& name,
               const double length,
               const double height,
               const double width,
               const double thickness);

  struct Material {
    static G4Material* Casing;
    static G4Material* Scintillator;
    static void Define();
  private:
    Material();
  };

  double GetLength() const { return _length; }
  double GetHeight() const { return _height; }
  double GetWidth() const { return _width; }
  double GetCasingThickness() const { return _thickness; }
  G4LogicalVolume* GetVolume() const { return _lvolume; }
  G4VPhysicalVolume* GetSensitiveVolume() const { return _sensitive; }

  void Register(G4VSensitiveDetector* detector);

  G4VPhysicalVolume* PlaceIn(G4LogicalVolume* parent,
                             const G4Transform3D& transform);

private:
  G4LogicalVolume* _lvolume;
  G4VPhysicalVolume* _sensitive;
  std::string _name;
  double _length, _height, _width, _thickness;
};

class Detector : public G4VSensitiveDetector {
public:
  Detector();

  void Initialize(G4HCofThisEvent* event);
  G4bool ProcessHits(G4Step* step, G4TouchableHistory*);
  void EndOfEvent(G4HCofThisEvent*);

  static const bool DataPerEvent = true;
  static const std::string& DataName;
  static const std::vector<std::string>& DataKeys;

  static G4VPhysicalVolume* Construct(G4LogicalVolume* world);
  static G4VPhysicalVolume* ConstructEarth(G4LogicalVolume* world);
};

} /* namespace Box */ //////////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */

#endif /* MU__GEOMETRY_BOX_HH */
