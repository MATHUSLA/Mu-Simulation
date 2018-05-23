/* src/geometry/flat/Layer.cc
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

#include "geometry/Flat.hh"

#include "Geant4/G4Box.hh"
#include "Geant4/G4UnionSolid.hh"
#include "Geant4/G4VSolid.hh"

namespace MATHUSLA { namespace MU {

namespace Flat { ///////////////////////////////////////////////////////////////////////////////

//__Layer Constructor___________________________________________________________________________
Layer::Layer(const std::string& name,
             const size_t count,
             const Scintillator& scintillator)
    : _scintillator(&scintillator), _volume(nullptr), _placement(nullptr), _name(name), _count(count) {

  const auto&& shift = scintillator.base_width + ScintillatorSpacing;

  const auto&& full_width = (count - 1) * shift + scintillator.GetFullWidth();
  _volume = Construction::BoxVolume(name,
    full_width,
    scintillator.height,
    scintillator.length);

  const auto& name_prefix = name + scintillator.name;
  for (size_t i = 0; i < count; ++i) {
    auto clone = Scintillator::Clone(scintillator, name_prefix + std::to_string(i));

    clone->pvolume = Construction::PlaceVolume(clone->lvolume, _volume,
      Construction::Transform(i * shift + 0.5 * (scintillator.GetFullWidth() - full_width), 0, 0));
  }
}
//----------------------------------------------------------------------------------------------

//__Layer Constructor___________________________________________________________________________
void Layer::Register(G4VSensitiveDetector* detector) {
  const auto&& count = _volume->GetNoDaughters();
  for (size_t i = 0; i < count; ++i) {
    _volume->GetDaughter(i)->GetLogicalVolume()->SetSensitiveDetector(detector);
  }
}
//----------------------------------------------------------------------------------------------

//__Place Layer in the Detector_________________________________________________________________
G4VPhysicalVolume* Layer::PlaceIn(G4LogicalVolume* parent,
                                  const G4Transform3D& transform) {
  return (_placement = Construction::PlaceVolume(_volume, parent, transform));
}
//----------------------------------------------------------------------------------------------

//__Clone Layer_________________________________________________________________________________
Layer* Layer::Clone(const Layer& other,
                    const std::string& new_name) {
  // TODO: optimize this
  return new Layer(new_name, other._count, *other._scintillator);
}
//----------------------------------------------------------------------------------------------

} /* namespace Flat */ /////////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */