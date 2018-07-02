/* src/physics/Generator.cc
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

#include <cmath>
#include <ostream>

#include <Geant4/Randomize.hh>
#include <Geant4/G4ParticleTable.hh>

#include "physics/Units.hh"

namespace MATHUSLA { namespace MU {

namespace Physics { ////////////////////////////////////////////////////////////////////////////

//__Check if Particle Matches the Cut___________________________________________________________
bool ParticleCut::matches(const int particle_id,
                          const G4ThreeVector& momentum) const {
  return particle_id == id && matches(momentum);
}
//----------------------------------------------------------------------------------------------

//__Check if Particle Matches the Cut___________________________________________________________
bool ParticleCut::matches(const G4ThreeVector& momentum) const {
  const auto triplet = Convert(momentum);
  bool match_pT = true, match_eta = true, match_phi = true;
  if (!min.pT && !max.pT) match_pT = (triplet.pT <= max.pT) && (triplet.pT >= min.pT);
  if (!min.eta && !max.eta) match_eta = (triplet.eta <= max.eta) && (triplet.eta >= min.eta);
  if (!min.phi && !max.phi) match_phi = (triplet.phi <= max.phi) && (triplet.phi >= min.phi);
  return match_pT && match_eta && match_phi;
}
//----------------------------------------------------------------------------------------------

//__Check if Particle Matches Any Cut in PropagationList________________________________________
bool InPropagationList(const PropagationList& list,
                       const int particle_id,
                       const G4ThreeVector& momentum) {
  for (const auto& cut : list)
    if (cut.matches(particle_id, momentum)) return true;
  return false;
}
bool InPropagationList(const PropagationList& list,
                       const G4ThreeVector& momentum) {
  for (const auto& cut : list)
    if (cut.matches(momentum)) return true;
  return false;
}
//----------------------------------------------------------------------------------------------

//__Get Mass of Particle from ID________________________________________________________________
double GetMass(const int id) {
  return id == 0 ? 0 : G4ParticleTable::GetParticleTable()->FindParticle(id)->GetPDGMass();
}
//----------------------------------------------------------------------------------------------

//__Get Momentum from Mass and Kinetic Energy___________________________________________________
const G4ThreeVector GetMomentum(const double mass,
                                const double ke,
                                const G4ThreeVector& p_unit) {
  return p_unit.unit() * std::sqrt(ke * (ke + 2.0L * mass));
}
//----------------------------------------------------------------------------------------------

//__Convert Momentum to Pseudo-Lorentz Triplet__________________________________________________
const PseudoLorentzTriplet Convert(const G4ThreeVector& momentum) {
  const auto magnitude = momentum.mag();
  if (magnitude == 0)
    return {};
  const auto eta = std::atanh(momentum.x() / magnitude);
  return {magnitude / std::cosh(eta), eta, std::atan2(momentum.y(), -momentum.z())};
}
//----------------------------------------------------------------------------------------------

//__Convert Pseudo-Lorentz Triplet to Momentum__________________________________________________
const G4ThreeVector Convert(const PseudoLorentzTriplet& triplet) {
  return G4ThreeVector(
     triplet.pT * std::sinh(triplet.eta),
     triplet.pT * std::sin(triplet.phi),
    -triplet.pT * std::cos(triplet.phi));
}
//----------------------------------------------------------------------------------------------

//__Default Vertex for IP_______________________________________________________________________
G4PrimaryVertex* DefaultVertex() {
  return new G4PrimaryVertex(0, 0, 100*m, 0);
}
//----------------------------------------------------------------------------------------------

//__Create Particle from ID and Momentum________________________________________________________
G4PrimaryParticle* CreateParticle(const int id,
                                  const G4ThreeVector& p) {
  return new G4PrimaryParticle(id, p.x(), p.y(), p.z());
}
//----------------------------------------------------------------------------------------------

//__Create Particle from ID and Pseudo-Lorentz Triplet__________________________________________
G4PrimaryParticle* CreateParticle(const int id,
                                  const double pT,
                                  const double eta,
                                  const double phi) {
  return CreateParticle(id, PseudoLorentzTriplet{pT, eta, phi});
}
//----------------------------------------------------------------------------------------------

//__Create Particle from ID and Pseudo-Lorentz Triplet__________________________________________
G4PrimaryParticle* CreateParticle(const int id,
                                  const PseudoLorentzTriplet& triplet) {
  return CreateParticle(id, Convert(triplet));
}
//----------------------------------------------------------------------------------------------

//__Generator Messenger Directory Path__________________________________________________________
const std::string Generator::MessengerDirectory = "/gen/";
//----------------------------------------------------------------------------------------------

//__Generator Simulation Setting Prefix_________________________________________________________
const std::string Generator::SimSettingPrefix = "GEN";
//----------------------------------------------------------------------------------------------

//__Generate UI Commands________________________________________________________________________
void Generator::GenerateCommands() {
  _ui_id = CreateCommand<Command::IntegerArg>("id", "Set Particle Id.");
  _ui_id->SetParameterName("id", false);
  _ui_id->AvailableForStates(G4State_PreInit, G4State_Idle);

  _ui_pT = CreateCommand<Command::DoubleUnitArg>("pT", "Set Transverse Momentum.");
  _ui_pT->SetParameterName("pT", false, false);
  _ui_pT->SetRange("pT > 0");
  _ui_pT->SetDefaultUnit("GeV/c");
  _ui_pT->SetUnitCandidates("eV/c keV/c MeV/c GeV/c");
  _ui_pT->AvailableForStates(G4State_PreInit, G4State_Idle);

  _ui_eta = CreateCommand<Command::DoubleArg>("eta", "Set Pseudorapidity.");
  _ui_eta->SetParameterName("eta", false);
  _ui_eta->AvailableForStates(G4State_PreInit, G4State_Idle);

  _ui_phi = CreateCommand<Command::DoubleUnitArg>("phi", "Set Semi-Opening Angle.");
  _ui_phi->SetParameterName("phi", false, false);
  _ui_phi->SetDefaultUnit("deg");
  _ui_phi->SetUnitCandidates("degree deg radian rad milliradian mrad");
  _ui_phi->AvailableForStates(G4State_PreInit, G4State_Idle);

  _ui_ke = CreateCommand<Command::DoubleUnitArg>("ke", "Set Kinetic Energy.");
  _ui_ke->SetParameterName("ke", false, false);
  _ui_ke->SetRange("ke > 0");
  _ui_ke->SetDefaultUnit("GeV");
  _ui_ke->SetUnitCandidates("eV keV MeV GeV");
  _ui_ke->AvailableForStates(G4State_PreInit, G4State_Idle);

  _ui_p = CreateCommand<Command::ThreeVectorArg>("p_unit", "Set Momentum Direction.");
  _ui_p->SetParameterName("px", "py", "pz", false, false);
  _ui_p->AvailableForStates(G4State_PreInit, G4State_Idle);
}
//----------------------------------------------------------------------------------------------

//__Generator Constructor_______________________________________________________________________
Generator::Generator(const std::string& name,
                     const std::string& description,
                     const int id,
                     const double pT,
                     const double eta,
                     const double phi)
    : G4UImessenger(MessengerDirectory + name + '/', description),
      _name(name), _description(description),
      _id(id), _pT(pT), _eta(eta), _phi(phi), _mass(GetMass(id)), _using_pt_eta_phi(true) {
  const auto conversion = Convert(PseudoLorentzTriplet{_pT, _eta, _phi});
  _ke = std::hypot(conversion.mag(), _mass) - _mass;
  _p_unit = conversion.unit();
  GenerateCommands();
}
//----------------------------------------------------------------------------------------------

//__Generator Constructor_______________________________________________________________________
Generator::Generator(const std::string& name,
                     const std::string& description,
                     const int id,
                     const double ke,
                     const G4ThreeVector& p_unit)
    : G4UImessenger(MessengerDirectory + name + '/', description),
      _name(name), _description(description),
      _id(id), _ke(ke), _p_unit(p_unit), _mass(GetMass(id)), _using_pt_eta_phi(false) {
  const auto conversion = Convert(GetMomentum(_mass, _ke, _p_unit));
  _pT = conversion.pT;
  _eta = conversion.eta;
  _phi = conversion.phi;
  GenerateCommands();
}
//----------------------------------------------------------------------------------------------

//__Generate Initial Particles__________________________________________________________________
void Generator::GeneratePrimaryVertex(G4Event* event) {
  auto vertex = DefaultVertex();
  if (_using_pt_eta_phi) {
    vertex->SetPrimary(CreateParticle(_id, _pT, _eta, _phi));
  } else {
    vertex->SetPrimary(CreateParticle(_id, GetMomentum(_mass, _ke, _p_unit)));
  }
  event->AddPrimaryVertex(vertex);
}
//----------------------------------------------------------------------------------------------

//__Generator Messenger Set Value_______________________________________________________________
void Generator::SetNewValue(G4UIcommand* command, G4String value) {
  if (command == _ui_id) {
    _id = _ui_id->GetNewIntValue(value);
    _mass = GetMass(_id);
  } else if (command == _ui_pT) {
    _pT = _ui_pT->GetNewDoubleValue(value);
    _using_pt_eta_phi = true;
  } else if (command == _ui_eta) {
    _eta = _ui_eta->GetNewDoubleValue(value);
    _using_pt_eta_phi = true;
  } else if (command == _ui_phi) {
    _phi = _ui_phi->GetNewDoubleValue(value);
    _using_pt_eta_phi = true;
  } else if (command == _ui_ke) {
    _ke = _ui_ke->GetNewDoubleValue(value);
    _using_pt_eta_phi = false;
  } else if (command == _ui_p) {
    _p_unit = _ui_p->GetNew3VectorValue(value).unit();
    _using_pt_eta_phi = false;
  }

  if (_using_pt_eta_phi) {
    const auto conversion = Convert(PseudoLorentzTriplet{_pT, _eta, _phi});
    _ke = std::hypot(conversion.mag(), _mass) - _mass;
    _p_unit = conversion.unit();
  } else {
    const auto conversion = Convert(GetMomentum(_mass, _ke, _p_unit));
    _pT = conversion.pT;
    _eta = conversion.eta;
    _phi = conversion.phi;
  }
}
//----------------------------------------------------------------------------------------------

//__Total Momentum______________________________________________________________________________
const G4ThreeVector Generator::p() const {
  return GetMomentum(_mass, _ke, _p_unit);
}
//----------------------------------------------------------------------------------------------

//__Generator Information String________________________________________________________________
std::ostream& Generator::Print(std::ostream& os) const {
  return os << "Generator Info:\n  "
            << "Name: "        << _name                       << "\n  "
            << "Description: " << _description                << "\n  "
            << "Particle ID: " << _id                         << "\n  "
            << "pT: "          << G4BestUnit(_pT, "Momentum") << "\n  "
            << "eta: "         << _eta                        << "\n  "
            << "phi: "         << G4BestUnit(_phi, "Angle")   << "\n  "
            << "ke: "          << G4BestUnit(_ke, "Energy")   << "\n  "
            << "p_unit: "      << _p_unit                     << "\n";
}
//----------------------------------------------------------------------------------------------

//__Generator Specifications____________________________________________________________________
const Analysis::SimSettingList Generator::GetSpecification() const {
  return Analysis::Settings(SimSettingPrefix,
    "",        _name,
    "_PDG_ID", std::to_string(_id),
    "_PT",     std::to_string(_pT / Units::Momentum) + " " + Units::MomentumString,
    "_ETA",    std::to_string(_eta),
    "_PHI",    std::to_string(_phi / Units::Angle) + " " + Units::AngleString,
    "_KE",     std::to_string(_ke / Units::Energy) + " " + Units::EnergyString,
    "_P_UNIT", "(" + std::to_string(_p_unit.x()) + ", "
                   + std::to_string(_p_unit.y()) + ", "
                   + std::to_string(_p_unit.z()) + ")");
}
//----------------------------------------------------------------------------------------------

//__Generator Gen Particle Data_________________________________________________________________
const Analysis::ROOT::DataEntryList Generator::GetGenParticleData() const {
  constexpr const std::size_t column_count = 11UL;

  Analysis::ROOT::DataEntryList out;
  out.reserve(column_count);

  const auto size = 1;
  for (std::size_t i = 0; i < column_count; ++i) {
    Analysis::ROOT::DataEntry entry;
    entry.reserve(size);
    out.push_back(entry);
  }

  for (std::size_t i = 0; i < size; ++i) {
    out[0].push_back(_id);
    out[1].push_back(1);
    out[2].push_back(0);
    out[3].push_back(0);
    out[4].push_back(0);
    out[5].push_back(0);
    out[6].push_back(-100 * m / Units::Length);
    out[7].push_back((_ke + _mass) / Units::Energy);

    const auto momentum = GetMomentum(_mass, _ke, _p_unit);
    out[8].push_back(momentum.x() / Units::Momentum);
    out[9].push_back(momentum.y() / Units::Momentum);
    out[10].push_back(momentum.z() / Units::Momentum);
  }

  return out;
}
//----------------------------------------------------------------------------------------------

//__Range Generator Constructor_________________________________________________________________
RangeGenerator::RangeGenerator(const std::string& name,
                               const std::string& description,
                               const int id,
                               const double pT,
                               const double eta,
                               const double phi)
    : RangeGenerator(name, description, id, pT, pT, -eta, eta, -phi, phi) {}
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
    : Generator(name, description, id, pT_min, 0, 0),
      _pT_min(pT_min), _pT_max(pT_max), _eta_min(eta_min), _eta_max(eta_max),
      _phi_min(phi_min), _phi_max(phi_max) {

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
}
//----------------------------------------------------------------------------------------------

//__Generate Initial Particles__________________________________________________________________
void RangeGenerator::GeneratePrimaryVertex(G4Event* event) {
  auto vertex = DefaultVertex();
  _pT  = G4MTRandFlat::shoot(_pT_min, _pT_max);
  _eta = G4MTRandFlat::shoot(_eta_min, _eta_max);
  _phi = G4MTRandFlat::shoot(_phi_min, _phi_max);
  vertex->SetPrimary(CreateParticle(_id, _pT, _eta, _phi));
  event->AddPrimaryVertex(vertex);
}
//----------------------------------------------------------------------------------------------

//__Range Generator Messenger Set Value_________________________________________________________
void RangeGenerator::SetNewValue(G4UIcommand* command, G4String value) {
  if (command == _ui_id) {
    _id = _ui_id->GetNewIntValue(value);

  } else if (command == _ui_pT) {
    _pT = _ui_pT->GetNewDoubleValue(value);
    _pT_min = _pT;
    _pT_max = _pT;
  } else if (command == _ui_pT_min) {
    _pT_min = _ui_pT_min->GetNewDoubleValue(value);
  } else if (command == _ui_pT_max) {
    _pT_max = _ui_pT_max->GetNewDoubleValue(value);

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
  }
}
//----------------------------------------------------------------------------------------------

//__Range Generator Information String__________________________________________________________
std::ostream& RangeGenerator::Print(std::ostream& os) const {
  os << "Generator Info:\n  "
     << "Name: "        << _name                                             << "\n  "
     << "Description: " << _description                                      << "\n  "
     << "Particle ID: " << _id                                               << "\n  "
     << "avg pT: "      << G4BestUnit(0.5 * (_pT_min + _pT_max), "Momentum") << "\n    "
     << "pT min: "      << G4BestUnit(_pT_min, "Momentum")                   << "\n    "
     << "pT max: "      << G4BestUnit(_pT_max, "Momentum")                   << "\n  "
     << "avg eta: "     << 0.5 * (_eta_min + _eta_max)                       << "\n    "
     << "eta min: "     << _eta_min                                          << "\n    "
     << "eta max: "     << _eta_max                                          << "\n  "
     << "avg phi: "     << G4BestUnit(0.5 * (_phi_min + _phi_max), "Angle")  << "\n    "
     << "phi min: "     << G4BestUnit(_phi_min, "Angle")                     << "\n    "
     << "phi max: "     << G4BestUnit(_phi_max, "Angle")                     << "\n";
  return os;
}
//----------------------------------------------------------------------------------------------

//__RangeGenerator Specifications_______________________________________________________________
const Analysis::SimSettingList RangeGenerator::GetSpecification() const {
  return Analysis::Settings(SimSettingPrefix,
    "",        _name,
    "_PDG_ID",  std::to_string(_id),
    "_PT_MIN",  std::to_string(_pT_min / Units::Momentum) + " " + Units::MomentumString,
    "_PT_MAX",  std::to_string(_pT_max / Units::Momentum) + " " + Units::MomentumString,
    "_ETA_MIN", std::to_string(_eta_min),
    "_ETA_MAX", std::to_string(_eta_min),
    "_PHI_MIN", std::to_string(_phi_min / Units::Angle) + " " + Units::AngleString,
    "_PHI_MAX", std::to_string(_phi_min / Units::Angle) + " " + Units::AngleString);
}
//----------------------------------------------------------------------------------------------

//__RangeGenerator Gen Particle Data____________________________________________________________
const Analysis::ROOT::DataEntryList RangeGenerator::GetGenParticleData() const {
  return Generator::GetGenParticleData();
}
//----------------------------------------------------------------------------------------------

} /* namespace Physics */ //////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */
