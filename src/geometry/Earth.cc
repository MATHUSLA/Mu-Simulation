/* src/geometry/Earth.cc
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

#include "geometry/Earth.hh"

#include <G4SubtractionSolid.hh>
#include <tls.hh>

#include "geometry/Construction.hh"

namespace MATHUSLA { namespace MU {

namespace { ////////////////////////////////////////////////////////////////////////////////////

constexpr auto safety_margin = 1.0*mm;

//__Earth Layer Size Constants__________________________________________________________________
static auto _lastshift                =     0.0L*cm;
static auto _layer_width_x            = 82500.0L*cm;
static auto _layer_width_y            = 82500.0L*cm;
static auto _buffer_zone_x_shift      =  -338.7L*cm;
static auto _buffer_zone_length       =  1376.0L*cm;
static auto _buffer_zone_higher_width =   900.0L*cm;
static auto _buffer_zone_lower_width  =   300.0L*cm;
static auto _buffer_zone_higher_depth =   100.0L*cm;
static auto _buffer_zone_lower_depth  =   160.0L*cm;
static auto _sx1_slab_depth           =   200.0L*cm;
static auto _sandstone_depth          =  4530.0L*cm;
static auto _marl_depth               =  1825.0L*cm;
static auto _mix_depth                =  3645.0L*cm;
//----------------------------------------------------------------------------------------------

} /* anonymous namespace */ ////////////////////////////////////////////////////////////////////

namespace Earth { //////////////////////////////////////////////////////////////////////////////

//__Earth Materials_____________________________________________________________________________
G4Material* Material::CaCO3     = nullptr;
G4Material* Material::Kaolinite = nullptr;
G4Material* Material::SiO2      = nullptr;
G4Material* Material::Marl      = nullptr;
G4Material* Material::Mix       = nullptr;
//----------------------------------------------------------------------------------------------

//__Define Earth Materials______________________________________________________________________
void Material::Define() {
  Material::CaCO3 = new G4Material("CaCO3", 2.71*g/cm3, 3);
  Material::CaCO3->AddElement(Construction::Material::Ca, 1);
  Material::CaCO3->AddElement(Construction::Material::C,  1);
  Material::CaCO3->AddElement(Construction::Material::O,  3);

  Material::Kaolinite = new G4Material("Clay", 2.65*g/cm3, 4);
  Material::Kaolinite->AddElement(Construction::Material::Al, 2);
  Material::Kaolinite->AddElement(Construction::Material::Si, 2);
  Material::Kaolinite->AddElement(Construction::Material::O,  9);
  Material::Kaolinite->AddElement(Construction::Material::H,  4);

  Material::SiO2 = new G4Material("Quartz", 2.445*g/cm3, 2);
  Material::SiO2->AddElement(Construction::Material::Si, 1);
  Material::SiO2->AddElement(Construction::Material::O, 2);

  Material::Marl = new G4Material("Marl", 2.46*g/cm3, 2);
  Material::Marl->AddMaterial(Material::Kaolinite, 35*perCent);
  Material::Marl->AddMaterial(Material::CaCO3,     65*perCent);

  Material::Mix = new G4Material("Mix", 2.54*g/cm3, 2);
  Material::Mix->AddMaterial(Material::Marl, 50*perCent);
  Material::Mix->AddMaterial(Material::SiO2, 50*perCent);
}
//----------------------------------------------------------------------------------------------

//__Earth Layer Sizes___________________________________________________________________________
long double LastShift() {
  return _lastshift;
}
long double LastShift(long double shift) {
  _lastshift = shift;
  return LastShift();
}
long double LayerWidthX() {
  return _layer_width_x;
}
long double LayerWidthX(long double value) {
  _layer_width_x = value;
  return LayerWidthX();
}
long double LayerWidthY() {
  return _layer_width_y;
}
long double LayerWidthY(long double value) {
  _layer_width_y = value;
  return LayerWidthY();
}
long double BufferZoneLength() {
  return _buffer_zone_length;
}
long double BufferZoneLength(long double value) {
  _buffer_zone_length = value;
  return BufferZoneLength();
}
long double BufferZoneHigherWidth() {
  return _buffer_zone_higher_width;
}
long double BufferZoneHigherWidth(long double value) {
  _buffer_zone_higher_width = value;
  return BufferZoneHigherWidth();
}
long double BufferZoneLowerWidth() {
  return _buffer_zone_lower_width;
}
long double BufferZoneLowerWidth(long double value) {
  _buffer_zone_lower_width = value;
  return BufferZoneLowerWidth();
}
long double BufferZoneHigherDepth() {
  return _buffer_zone_higher_depth;
}
long double BufferZoneHigherDepth(long double value) {
  _buffer_zone_higher_depth = value;
  return BufferZoneHigherDepth();
}
long double BufferZoneLowerDepth() {
  return _buffer_zone_lower_depth;
}
long double BufferZoneLowerDepth(long double value) {
  _buffer_zone_lower_depth = value;
  return BufferZoneLowerDepth();
}
long double SX1SlabDepth() {
  return _sx1_depth;
}
long double SandstoneDepth() {
  return _sandstone_depth;
}
long double SandstoneDepth(long double value) {
  _sandstone_depth = value;
  return SandstoneDepth();
}
long double MarlDepth() {
  return _marl_depth;
}
long double MarlDepth(long double value) {
  _marl_depth = value;
  return MarlDepth();
}
long double MixDepth() {
  return _mix_depth;
}
long double MixDepth(long double value) {
  _mix_depth = value;
  return MixDepth();
}
long double TotalShift() {
  return LastShift();
}
long double TotalDepth() {
  return SandstoneDepth() + MarlDepth() + MixDepth();
}
//----------------------------------------------------------------------------------------------

G4Box* BufferZoneSolid() {
  auto higher_solid = Construction::Box("", BufferZoneLength() + safety_margin, BufferZoneHigherWidth() + safety_margin, BufferZoneHigherDepth() + safety_margin);
  auto lower_solid = Construction::Box("", BufferZoneLength() + safety_margin, BufferZoneLowerWidth() + safety_margin, BufferZoneLowerDepth());
  return new G4UnionSolid("", higher_solid, lower_solid, Construction::Transform(0.0, 0.0, 0.5 * (BufferZoneHigherDepth() + BufferZoneLowerDepth() - safety_margin)));
}

//__Earth Logical Volumes_______________________________________________________________________
G4LogicalVolume* Volume() {
  auto earth_box = Construction::Box("", LayerWidthX(), LayerWidthY(), TotalDepth());

  auto earth_solid = new G4SubtractionSolid("Earth", earth_box, BufferZoneSolid(), Construction::Transform(_buffer_zone_x_shift, 0.0, 0.5 * (BufferZoneHigherDepth() - safety_margin - TotalDepth())));

  return Construction::Volume(earth_solid);
}
G4LogicalVolume* SX1SlabVolume() {
  auto sx1_slab_box = Construction::Box("", BufferZoneLength(), BufferZoneHigherWidth(), SX1SlabDepth());

  auto sx1_slab_solid = new G4SubtractionSolid("SX1Slab", sx1_slab_box, BufferZoneSolid(), Construction::Transform(0.0, 0.0, 0.5 * (BufferZoneHigherDepth() - safety_margin - SX1SlabDepth())));

  return Construction::Volume(sx1_slab_solid, Construction::Material::Concrete, Construction::BorderAttributes());
}
G4LogicalVolume* SandstoneVolume() {
  auto sandstone_box = Construction::Box("", LayerWidthX(), LayerWidthY(), SandstoneDepth());

  auto sandstone_solid = new G4SubtractionSolid("Sandstone", sandstone_box, BufferZoneHigherSolid(), Construction::Transform(_buffer_zone_x_shift, 0.0, 0.5 * (BufferZoneHigherDepth() - safety_margin - SandstoneDepth())));

  return Construction::Volume(sandstone_solid, Material::SiO2, Construction::BorderAttributes());
}
G4LogicalVolume* MarlVolume() {
  return Construction::BoxVolume("Marl",
    LayerWidthX(), LayerWidthY(), MarlDepth(), Material::Marl);
}
G4LogicalVolume* MixVolume() {
  return Construction::BoxVolume("Mix",
    LayerWidthX(), LayerWidthY(), MixDepth(), Material::Mix);
}
//----------------------------------------------------------------------------------------------

//__Earth Transformations_______________________________________________________________________
const G4Translate3D Transform() {
  return G4Translate3D(0, 0, TotalShift() + 0.5L * TotalDepth());
}
const G4Translate3D SX1SlabTransform() {
  return G4Translate3D(0, 0, 0.5L * (SX1SlabDepth() - TotalDepth()));
}
const G4Translate3D SandstoneTransform() {
  return G4Translate3D(0, 0, 0.5L * (SandstoneDepth() - TotalDepth()));
}
const G4Translate3D MarlTransform() {
  return G4Translate3D(0, 0, SandstoneDepth() + 0.5L * (MarlDepth() - TotalDepth()));
}
const G4Translate3D MixTransform() {
  return G4Translate3D(0, 0, SandstoneDepth() + MarlDepth() + 0.5L * (MixDepth() - TotalDepth()));
}
//----------------------------------------------------------------------------------------------

//__Construct Earth_____________________________________________________________________________
G4VPhysicalVolume* Construct(G4LogicalVolume* world) {
  Material::Define();
  auto earth = Volume();
  Construction::PlaceVolume(SX1SlabVolume(), earth, SX1SlabTransform());
  Construction::PlaceVolume(SandstoneVolume(), earth, SandstoneTransform());
  Construction::PlaceVolume(MarlVolume(), earth, MarlTransform());
  Construction::PlaceVolume(MixVolume(), earth, MixTransform());
  return Construction::PlaceVolume(earth, world, Transform());
}
//----------------------------------------------------------------------------------------------

} /* namespace Earth */ ////////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */
