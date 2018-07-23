/*
 * src/physics/RangeGenerator.cc
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

#include "physics/Generator.hh"

#include <ostream>

#include <Geant4/Randomize.hh>

#include "physics/Units.hh"

namespace MATHUSLA { namespace MU {

namespace Physics { ////////////////////////////////////////////////////////////////////////////

//__Range Generator Constructor_________________________________________________________________
RangeGenerator::RangeGenerator(const std::string& name,
                               const std::string& description,
                               const int id,
                               const double pT,
                               const double eta,
                               const double phi)
    : RangeGenerator(name, description, id, pT, pT, eta, eta, phi, phi) {}
//----------------------------------------------------------------------------------------------

//__Range Generator Constructor_________________________________________________________________
RangeGenerator::RangeGenerator(const std::string& name,
                               const std::string& description,
                               const int id,
                               const double pT_min,
                               const double pT_max,
                               const double eta_min,
                               const double eta_max,
                               const double phi_min,
                               const double phi_max)
    : Generator(name, description, id, 0, 0, 0),
      _pT_min(pT_min), _pT_max(pT_max),
      _eta_min(eta_min), _eta_max(eta_max),
      _phi_min(phi_min), _phi_max(phi_max),
      _ke_min(_ke), _ke_max(_ke),
      _using_range_ke(false) {
  GenerateCommands();
}
//----------------------------------------------------------------------------------------------

//__Range Generator Constructor_________________________________________________________________
RangeGenerator::RangeGenerator(const std::string& name,
                               const std::string& description,
                               const int id,
                               const double pT,
                               const double eta,
                               const double phi,
                               const G4ThreeVector& vertex)
    : RangeGenerator(name, description, id, pT, pT, eta, eta, phi, phi, vertex) {}
//----------------------------------------------------------------------------------------------

//__Range Generator Constructor_________________________________________________________________
RangeGenerator::RangeGenerator(const std::string& name,
                               const std::string& description,
                               const int id,
                               const double pT_min,
                               const double pT_max,
                               const double eta_min,
                               const double eta_max,
                               const double phi_min,
                               const double phi_max,
                               const G4ThreeVector& vertex)
    : Generator(name, description, id, 0, 0, 0, vertex),
      _pT_min(pT_min), _pT_max(pT_max),
      _eta_min(eta_min), _eta_max(eta_max),
      _phi_min(phi_min), _phi_max(phi_max),
      _ke_min(_ke), _ke_max(_ke),
      _using_range_ke(false) {
  GenerateCommands();
}
//----------------------------------------------------------------------------------------------

//__Range Generator Constructor_________________________________________________________________
RangeGenerator::RangeGenerator(const std::string& name,
                               const std::string& description,
                               const int id,
                               const double pT,
                               const double eta,
                               const double phi,
                               const double t0,
                               const G4ThreeVector& vertex)
    : RangeGenerator(name, description, id, pT, pT, eta, eta, phi, phi, t0, vertex) {}
//----------------------------------------------------------------------------------------------

//__Range Generator Constructor_________________________________________________________________
RangeGenerator::RangeGenerator(const std::string& name,
                               const std::string& description,
                               const int id,
                               const double pT_min,
                               const double pT_max,
                               const double eta_min,
                               const double eta_max,
                               const double phi_min,
                               const double phi_max,
                               const double t0,
                               const G4ThreeVector& vertex)
    : Generator(name, description, id, 0, 0, 0, t0, vertex),
      _pT_min(pT_min), _pT_max(pT_max),
      _eta_min(eta_min), _eta_max(eta_max),
      _phi_min(phi_min), _phi_max(phi_max),
      _ke_min(_ke), _ke_max(_ke),
      _using_range_ke(false) {
  GenerateCommands();
}
//----------------------------------------------------------------------------------------------

//__Generate UI Commands________________________________________________________________________
void RangeGenerator::GenerateCommands() {
  _ui_pT_min = CreateCommand<Command::DoubleUnitArg>("pT_min", "Set Minimum Transverse Momentum.");
  _ui_pT_min->SetParameterName("pT_min", false, false);
  _ui_pT_min->SetRange("pT_min > 0");
  _ui_pT_min->SetDefaultUnit("GeV/c");
  _ui_pT_min->SetUnitCandidates("eV/c keV/c MeV/c GeV/c");
  _ui_pT_min->AvailableForStates(G4State_PreInit, G4State_Idle);

  _ui_pT_max = CreateCommand<Command::DoubleUnitArg>("pT_max", "Set Maximum Transverse Momentum.");
  _ui_pT_max->SetParameterName("pT_max", false, false);
  _ui_pT_max->SetRange("pT_max > 0");
  _ui_pT_max->SetDefaultUnit("GeV/c");
  _ui_pT_max->SetUnitCandidates("eV/c keV/c MeV/c GeV/c");
  _ui_pT_max->AvailableForStates(G4State_PreInit, G4State_Idle);

  _ui_eta_min = CreateCommand<Command::DoubleArg>("eta_min", "Set Minimum Pseudorapidity.");
  _ui_eta_min->SetParameterName("eta_min", false);
  _ui_eta_min->AvailableForStates(G4State_PreInit, G4State_Idle);

  _ui_eta_max = CreateCommand<Command::DoubleArg>("eta_max", "Set Maximum Pseudorapidity.");
  _ui_eta_max->SetParameterName("eta_max", false);
  _ui_eta_max->AvailableForStates(G4State_PreInit, G4State_Idle);

  _ui_phi_min = CreateCommand<Command::DoubleUnitArg>("phi_min", "Set Minimum Semi-Opening Angle.");
  _ui_phi_min->SetParameterName("phi_min", false, false);
  _ui_phi_min->SetDefaultUnit("deg");
  _ui_phi_min->SetUnitCandidates("degree deg radian rad milliradian mrad");
  _ui_phi_min->AvailableForStates(G4State_PreInit, G4State_Idle);

  _ui_phi_max = CreateCommand<Command::DoubleUnitArg>("phi_max", "Set Maximum Semi-Opening Angle.");
  _ui_phi_max->SetParameterName("phi_max", false, false);
  _ui_phi_max->SetDefaultUnit("deg");
  _ui_phi_max->SetUnitCandidates("degree deg radian rad milliradian mrad");
  _ui_phi_max->AvailableForStates(G4State_PreInit, G4State_Idle);

  _ui_ke_min = CreateCommand<Command::DoubleUnitArg>("ke_min", "Set Minimum Kinetic Energy.");
  _ui_ke_min->SetParameterName("ke_min", false, false);
  _ui_ke_min->SetRange("ke_min > 0");
  _ui_ke_min->SetDefaultUnit("GeV");
  _ui_ke_min->SetUnitCandidates("eV keV MeV GeV");
  _ui_ke_min->AvailableForStates(G4State_PreInit, G4State_Idle);

  _ui_ke_max = CreateCommand<Command::DoubleUnitArg>("ke_max", "Set Maximum Kinetic Energy.");
  _ui_ke_max->SetParameterName("ke_max", false, false);
  _ui_ke_max->SetRange("ke_max > 0");
  _ui_ke_max->SetDefaultUnit("GeV");
  _ui_ke_max->SetUnitCandidates("eV keV MeV GeV");
  _ui_ke_max->AvailableForStates(G4State_PreInit, G4State_Idle);
}
//----------------------------------------------------------------------------------------------

//__Generate Initial Particles__________________________________________________________________
void RangeGenerator::GeneratePrimaryVertex(G4Event* event) {
  auto vertex = Vertex(_t0, _vertex);
  _eta = G4MTRandFlat::shoot(_eta_min, _eta_max);
  _phi = G4MTRandFlat::shoot(_phi_min, _phi_max);

  if (_using_range_ke) {
    _ke = G4MTRandFlat::shoot(_ke_min, _ke_max);
    _p_unit = Convert(PseudoLorentzTriplet{1.0L, _eta, _phi}).unit();
    _pT = Convert(GetMomentum(_mass, _ke, _p_unit)).pT;
  } else {
    _pT = G4MTRandFlat::shoot(_pT_min, _pT_max);
    const auto conversion = Convert(PseudoLorentzTriplet{_pT, _eta, _phi});
    _ke = std::hypot(conversion.mag(), _mass) - _mass;
    _p_unit = conversion.unit();
  }

  vertex->SetPrimary(CreateParticle(_id, _pT, _eta, _phi));
  event->AddPrimaryVertex(vertex);
}
//----------------------------------------------------------------------------------------------

//__Range Generator Messenger Set Value_________________________________________________________
void RangeGenerator::SetNewValue(G4UIcommand* command,
                                 G4String value) {
  if (command == _ui_pT) {
    _pT = _ui_pT->GetNewDoubleValue(value);
    _pT_min = _pT;
    _pT_max = _pT;
    _using_range_ke = false;
  } else if (command == _ui_pT_min) {
    _pT_min = _ui_pT_min->GetNewDoubleValue(value);
    _using_range_ke = false;
  } else if (command == _ui_pT_max) {
    _pT_max = _ui_pT_max->GetNewDoubleValue(value);
    _using_range_ke = false;
  } else if (command == _ui_eta) {
    _eta = _ui_eta->GetNewDoubleValue(value);
    _eta_min = _eta;
    _eta_max = _eta;
  } else if (command == _ui_eta_min) {
    _eta_min = _ui_eta_min->GetNewDoubleValue(value);
  } else if (command == _ui_eta_max) {
    _eta_max = _ui_eta_max->GetNewDoubleValue(value);
  } else if (command == _ui_phi) {
    _phi = _ui_phi->GetNewDoubleValue(value);
    _phi_min = _phi;
    _phi_max = _phi;
  } else if (command == _ui_phi_min) {
    _phi_min = _ui_phi_min->GetNewDoubleValue(value);
  } else if (command == _ui_phi_max) {
    _phi_max = _ui_phi_max->GetNewDoubleValue(value);
  } else if (command == _ui_ke) {
    _ke = _ui_ke->GetNewDoubleValue(value);
    _ke_min = _ke;
    _ke_max = _ke;
    _using_range_ke = true;
  } else if (command == _ui_ke_min) {
    _ke_min = _ui_ke_min->GetNewDoubleValue(value);
    _using_range_ke = true;
  } else if (command == _ui_ke_max) {
    _ke_max = _ui_ke_max->GetNewDoubleValue(value);
    _using_range_ke = true;
  } else if (command == _ui_p) {
    // TODO: fix
  }

  if (_using_range_ke) {
    _p_unit = Convert(PseudoLorentzTriplet{_pT, _eta, _phi}).unit();
    _pT = Convert(GetMomentum(_mass, _ke, _p_unit)).pT;
  } else {
    const auto conversion = Convert(PseudoLorentzTriplet{_pT, _eta, _phi});
    _ke = std::hypot(conversion.mag(), _mass) - _mass;
    _p_unit = conversion.unit();
  }

  Generator::SetNewValue(command, value);
}
//----------------------------------------------------------------------------------------------

//__Range Generator Information String__________________________________________________________
std::ostream& RangeGenerator::Print(std::ostream& os) const {
  os << "Generator Info:\n  "
     << "Name: "        << _name                                             << "\n  "
     << "Description: " << _description                                      << "\n  "
     << "Particle ID: " << _id                                               << "\n  ";

  if (_using_range_ke) {
    os << "avg ke: "      << G4BestUnit(0.5 * (_ke_min + _ke_max), "Energy")   << "\n    "
       << "ke min: "      << G4BestUnit(_ke_min, "Energy")                     << "\n    "
       << "ke max: "      << G4BestUnit(_ke_max, "Energy")                     << "\n  ";
  } else {
    os << "avg pT: "      << G4BestUnit(0.5 * (_pT_min + _pT_max), "Momentum") << "\n    "
       << "pT min: "      << G4BestUnit(_pT_min, "Momentum")                   << "\n    "
       << "pT max: "      << G4BestUnit(_pT_max, "Momentum")                   << "\n  ";
  }

  os << "avg eta: "     << 0.5 * (_eta_min + _eta_max)                       << "\n    "
     << "eta min: "     << _eta_min                                          << "\n    "
     << "eta max: "     << _eta_max                                          << "\n  "
     << "avg phi: "     << G4BestUnit(0.5 * (_phi_min + _phi_max), "Angle")  << "\n    "
     << "phi min: "     << G4BestUnit(_phi_min, "Angle")                     << "\n    "
     << "phi max: "     << G4BestUnit(_phi_max, "Angle")                     << "\n  "
     << "vertex: "      << G4BestUnit(_t0, "Time")                           << " "
                        << G4BestUnit(_vertex, "Length")                     << "\n";
  return os;
}
//----------------------------------------------------------------------------------------------

//__RangeGenerator Specifications_______________________________________________________________
const Analysis::SimSettingList RangeGenerator::GetSpecification() const {
  return Analysis::Settings(SimSettingPrefix,
    "",         _name,
    "_PDG_ID",  std::to_string(_id),
    (_using_range_ke ? "_KE_MIN" : "_PT_MIN"),
    (_using_range_ke ? std::to_string(_ke_min / Units::Energy)   + " " + Units::EnergyString
                     : std::to_string(_pT_min / Units::Momentum) + " " + Units::MomentumString),
    (_using_range_ke ? "_KE_MAX" : "_PT_MAX"),
    (_using_range_ke ? std::to_string(_ke_max / Units::Energy)   + " " + Units::EnergyString
                     : std::to_string(_pT_max / Units::Momentum) + " " + Units::MomentumString),
    "_ETA_MIN", std::to_string(_eta_min),
    "_ETA_MAX", std::to_string(_eta_max),
    "_PHI_MIN", std::to_string(_phi_min / Units::Angle) + " " + Units::AngleString,
    "_PHI_MAX", std::to_string(_phi_max / Units::Angle) + " " + Units::AngleString,
    "_VERTEX", "(" + std::to_string(_t0         / Units::Time)   + ", "
                   + std::to_string(_vertex.x() / Units::Length) + ", "
                   + std::to_string(_vertex.y() / Units::Length) + ", "
                   + std::to_string(_vertex.z() / Units::Length) + ")");
}
//----------------------------------------------------------------------------------------------

} /* namespace Physics */ //////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */
