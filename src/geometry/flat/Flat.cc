/* src/geometry/flat/Flat.cc
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

#include "Geant4/G4SDManager.hh"

#include "tracking.hh"

namespace MATHUSLA { namespace MU {

namespace Flat { ///////////////////////////////////////////////////////////////////////////////

namespace { ////////////////////////////////////////////////////////////////////////////////////

//__Prototype Hit Collection____________________________________________________________________
std::vector<Layer*> _layers;
//----------------------------------------------------------------------------------------------

//__Prototype Hit Collection____________________________________________________________________
G4ThreadLocal Tracking::HitCollection* _hit_collection;
//----------------------------------------------------------------------------------------------

} /* anonymous namespace */ ////////////////////////////////////////////////////////////////////

//__Detector Constructor________________________________________________________________________
Detector::Detector() : G4VSensitiveDetector("MATHUSLA/MU/Flat") {
  collectionName.insert("Flat_HC");

  for (auto& layer : _layers) {
    layer->Register(this);
  }
}
//----------------------------------------------------------------------------------------------

//__Initalize Event_____________________________________________________________________________
void Detector::Initialize(G4HCofThisEvent* event) {
  _hit_collection = Tracking::GenerateHitCollection(this, event);
}
//----------------------------------------------------------------------------------------------

//__Hit Processing______________________________________________________________________________
G4bool Detector::ProcessHits(G4Step* step, G4TouchableHistory*) {
  // TODO: implement
  return false;
}
//----------------------------------------------------------------------------------------------

//__Post-Event Processing_______________________________________________________________________
void Detector::EndOfEvent(G4HCofThisEvent*) {
  if (verboseLevel >= 2 && _hit_collection)
    std::cout << *_hit_collection;
}
//----------------------------------------------------------------------------------------------

//__Build Detector______________________________________________________________________________
G4VPhysicalVolume* Detector::Construct(G4LogicalVolume* world) {
  Scintillator::Material::Define();

  constexpr double total_outer_box_height = 6000*mm;

  auto DetectorVolume = Construction::BoxVolume(
    "Flat", 2800*mm, 2800*mm, total_outer_box_height);

  auto S1 = new Scintillator("S1", 3000*mm, 25*mm, 35*mm, 15*mm);

  auto L1 = new Layer("L1", 100, *S1);

  L1->PlaceIn(DetectorVolume, Construction::Transform(0, 0, 0, 1, 0, 0, 90*deg));

  return Construction::PlaceVolume(DetectorVolume, world,
    G4Translate3D(0, 0, -0.5*total_outer_box_height));
}
//----------------------------------------------------------------------------------------------

} /* namespace Flat */ /////////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */
