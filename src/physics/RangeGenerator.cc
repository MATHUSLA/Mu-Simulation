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
#include <Geant4/G4MTRandFlat.hh>
#include <Geant4/tls.hh>

#include "physics/Units.hh"

#include <iostream>

namespace MATHUSLA { namespace MU {

namespace Physics { ////////////////////////////////////////////////////////////////////////////

//__Range Generator Constructor_________________________________________________________________
RangeGenerator::RangeGenerator(const std::string& name,
                               const std::string& description,
                               const Particle& particle)
    : RangeGenerator(name, description, particle, particle) {}
//----------------------------------------------------------------------------------------------

//__Range Generator Constructor_________________________________________________________________
RangeGenerator::RangeGenerator(const std::string& name,
                               const std::string& description,
                               const Particle& min,
                               const Particle& max)
    : Generator(name, description, min), _min(min), _max(max) {
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
  _particle.set_eta(G4RandFlat::shoot(_min.eta(), _max.eta()));
  _particle.set_phi(G4RandFlat::shoot(_min.phi(), _max.phi()));
  if (_using_range_ke) {
    _particle.set_ke(G4RandFlat::shoot(_min.ke(), _max.ke()));
  } else {
    _particle.set_pT(G4RandFlat::shoot(_min.pT(), _max.pT()));
  }
  AddParticle(_particle, *event);
}
//----------------------------------------------------------------------------------------------

//__Get Last Event Data_________________________________________________________________________
ParticleVector RangeGenerator::GetLastEvent() const {
  return ParticleVector{};
}
//----------------------------------------------------------------------------------------------

//__Range Generator Messenger Set Value_________________________________________________________
void RangeGenerator::SetNewValue(G4UIcommand* command,
                                 G4String value) {
  if (command == _ui_id) {
    _particle.id = _ui_id->GetNewIntValue(value);
    _min.id = _particle.id;
    _max.id = _particle.id;
  } else if (command == _ui_pT) {
    const auto pT = _ui_pT->GetNewDoubleValue(value);
    _particle.set_pT(pT);
    _min.set_pT(pT);
    _max.set_pT(pT);
    _using_range_ke = false;
  } else if (command == _ui_pT_min) {
    _min.set_pT(_ui_pT_min->GetNewDoubleValue(value));
    _using_range_ke = false;
  } else if (command == _ui_pT_max) {
    _max.set_pT(_ui_pT_max->GetNewDoubleValue(value));
    _using_range_ke = false;
  } else if (command == _ui_eta) {
    const auto eta = _ui_eta->GetNewDoubleValue(value);
    _particle.set_eta(eta);
    _min.set_eta(eta);
    _max.set_eta(eta);
  } else if (command == _ui_eta_min) {
    _min.set_eta(_ui_eta_min->GetNewDoubleValue(value));
  } else if (command == _ui_eta_max) {
    _max.set_eta(_ui_eta_max->GetNewDoubleValue(value));
  } else if (command == _ui_phi) {
    const auto phi = _ui_phi->GetNewDoubleValue(value);
    _particle.set_phi(phi);
    _min.set_phi(phi);
    _max.set_phi(phi);
  } else if (command == _ui_phi_min) {
    _min.set_phi(_ui_phi_min->GetNewDoubleValue(value));
  } else if (command == _ui_phi_max) {
    _max.set_phi(_ui_phi_max->GetNewDoubleValue(value));
  } else if (command == _ui_ke) {
    const auto ke = _ui_ke->GetNewDoubleValue(value);
    _particle.set_ke(ke);
    _min.set_ke(ke);
    _max.set_ke(ke);
    _using_range_ke = true;
  } else if (command == _ui_ke_min) {
    _min.set_ke(_ui_ke_min->GetNewDoubleValue(value));
    _using_range_ke = true;
  } else if (command == _ui_ke_max) {
    _max.set_ke(_ui_ke_max->GetNewDoubleValue(value));
    _using_range_ke = true;
  } else if (command == _ui_p_unit) {
    const auto unit = _ui_p_unit->GetNew3VectorValue(value).unit();
    _particle.set_p_unit(unit);
    _min.set_p_unit(unit);
    _max.set_p_unit(unit);
  } else if (command == _ui_p_mag) {
    const auto magnitude = _ui_p_mag->GetNewDoubleValue(value);
    _particle.set_p_mag(magnitude);
    _min.set_p_mag(magnitude);
    _max.set_p_mag(magnitude);
    _using_range_ke = true;
  } else {
    Generator::SetNewValue(command, value);
  }
}
//----------------------------------------------------------------------------------------------

//__Range Generator Information String__________________________________________________________
std::ostream& RangeGenerator::Print(std::ostream& os) const {
  os << "Generator Info:\n  "
     << "Name:        " << _name        << "\n  "
     << "Description: " << _description << "\n  "
     << "Particle ID: " << _particle.id << "\n  ";

  if (_using_range_ke) {
    os << "avg ke:      " << G4BestUnit(0.5 * (_min.ke() + _max.ke()), "Energy") << "\n    "
       << "ke min:      " << G4BestUnit(_min.ke(), "Energy")                     << "\n    "
       << "ke max:      " << G4BestUnit(_max.ke(), "Energy")                     << "\n  ";
  } else {
    os << "avg pT:      " << G4BestUnit(0.5 * (_min.pT() + _max.pT()), "Momentum") << "\n    "
       << "pT min:      " << G4BestUnit(_min.pT(), "Momentum")                     << "\n    "
       << "pT max:      " << G4BestUnit(_max.pT(), "Momentum")                     << "\n  ";
  }

  os << "avg eta:     "  << 0.5 * (_min.eta() + _max.eta())                      << "\n    "
     << "eta min:     "  << _min.eta()                                           << "\n    "
     << "eta max:     "  << _max.eta()                                           << "\n  "
     << "avg phi:     "  << G4BestUnit(0.5 * (_min.phi() + _max.phi()), "Angle") << "\n    "
     << "phi min:     "  << G4BestUnit(_min.phi(), "Angle")                      << "\n    "
     << "phi max:     "  << G4BestUnit(_max.phi(), "Angle")                      << "\n  "
     << "vertex:      (" << G4BestUnit(_particle.t, "Time")                      << ", "
                         << G4BestUnit(_particle.x, "Length")                    << ", "
                         << G4BestUnit(_particle.y, "Length")                    << ", "
                         << G4BestUnit(_particle.z, "Length")                    << ")\n";
  return os;
}
//----------------------------------------------------------------------------------------------

//__RangeGenerator Specifications_______________________________________________________________
const Analysis::SimSettingList RangeGenerator::GetSpecification() const {
  return Analysis::Settings(SimSettingPrefix,
    "",         _name,
    "_PDG_ID",  std::to_string(_particle.id),
    (_using_range_ke ? "_KE_MIN" : "_PT_MIN"),
    (_using_range_ke ? std::to_string(_min.ke() / Units::Energy)   + " " + Units::EnergyString
                     : std::to_string(_min.pT() / Units::Momentum) + " " + Units::MomentumString),
    (_using_range_ke ? "_KE_MAX" : "_PT_MAX"),
    (_using_range_ke ? std::to_string(_max.ke() / Units::Energy)   + " " + Units::EnergyString
                     : std::to_string(_max.pT() / Units::Momentum) + " " + Units::MomentumString),
    "_P_MAG_MIN", std::to_string(_min.p_mag() / Units::Momentum) + " " + Units::MomentumString,
    "_P_MAG_MAX", std::to_string(_max.p_mag() / Units::Momentum) + " " + Units::MomentumString,
    "_ETA_MIN", std::to_string(_min.eta()),
    "_ETA_MAX", std::to_string(_max.eta()),
    "_PHI_MIN", std::to_string(_min.phi() / Units::Angle) + " " + Units::AngleString,
    "_PHI_MAX", std::to_string(_max.phi() / Units::Angle) + " " + Units::AngleString,
    "_VERTEX", "(" + std::to_string(_particle.t / Units::Time)   + ", "
                   + std::to_string(_particle.x / Units::Length) + ", "
                   + std::to_string(_particle.y / Units::Length) + ", "
                   + std::to_string(_particle.z / Units::Length) + ")");
}
//----------------------------------------------------------------------------------------------

} /* namespace Physics */ //////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */
