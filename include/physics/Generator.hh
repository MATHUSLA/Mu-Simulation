/*
 * include/physics/Generator.hh
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

#ifndef MU__PHYSICS_GENERATOR_HH
#define MU__PHYSICS_GENERATOR_HH
#pragma once

#include <Geant4/G4Event.hh>
#include <Geant4/G4PrimaryVertex.hh>
#include <Geant4/G4PrimaryParticle.hh>

#include "analysis.hh"
#include "ui.hh"

#include "physics/Particle.hh"

namespace MATHUSLA { namespace MU {

namespace Physics { ////////////////////////////////////////////////////////////////////////////

//__Cut on Particle_____________________________________________________________________________
using ParticleCut = Filter::All<Filter::Id, Filter::PseudoLorentz>;
//----------------------------------------------------------------------------------------------

//__Equality Between Cuts_______________________________________________________________________
constexpr bool operator==(const ParticleCut& left,
                          const ParticleCut& right) {
  return left.id == right.id && left.min == right.min && left.max == right.max;
}
//----------------------------------------------------------------------------------------------

//__Get ParticleCut String______________________________________________________________________
const std::string GetParticleCutString(const ParticleCut& cut);
//----------------------------------------------------------------------------------------------

//__Stream Cut Object___________________________________________________________________________
inline std::ostream& operator<<(std::ostream& os,
                                const ParticleCut& cut) {
  return os << GetParticleCutString(cut);
}
//----------------------------------------------------------------------------------------------

//__List of Particles to Propagate______________________________________________________________
using PropagationList = std::vector<ParticleCut>;
//----------------------------------------------------------------------------------------------

//__Parse Propagation List from String__________________________________________________________
const PropagationList ParsePropagationList(const std::string& cut_string);
//----------------------------------------------------------------------------------------------

//__Check if Particle Matches Any Cut in PropagationList________________________________________
bool InPropagationList(const PropagationList& list,
                       const BasicParticle& particle);
//----------------------------------------------------------------------------------------------

//__Default Vertex for IP_______________________________________________________________________
G4PrimaryVertex* DefaultVertex();
//----------------------------------------------------------------------------------------------

//__Default Particle Generator__________________________________________________________________
class Generator : public G4UImessenger {
public:
  Generator(const std::string& name,
            const std::string& description,
            const Particle& particle);

  virtual ~Generator() = default;

  virtual void GeneratePrimaryVertex(G4Event* event);
  virtual void SetNewValue(G4UIcommand* command,
                           G4String value);
  virtual std::ostream& Print(std::ostream& os=std::cout) const;
  virtual const Analysis::SimSettingList GetSpecification() const;

  const Particle& particle() const { return _particle; }
  const std::string& name() const { return _name; }
  const std::string& description() const { return _description; }

  static const std::string MessengerDirectory;
  static const std::string SimSettingPrefix;

protected:
  Generator(const std::string& name,
            const std::string& description);
  virtual void GenerateCommands();

  std::string _name, _description;
  Particle _particle;
  Command::IntegerArg*         _ui_id;
  Command::DoubleUnitArg*      _ui_pT;
  Command::DoubleArg*          _ui_eta;
  Command::DoubleUnitArg*      _ui_phi;
  Command::DoubleUnitArg*      _ui_ke;
  Command::ThreeVectorArg*     _ui_p;
  Command::DoubleUnitArg*      _ui_t0;
  Command::ThreeVectorUnitArg* _ui_vertex;
};
//----------------------------------------------------------------------------------------------

//__Default Range Particle Generator____________________________________________________________
class RangeGenerator : public Generator {
public:
  RangeGenerator(const std::string& name,
                 const std::string& description,
                 const Particle& particle);
  RangeGenerator(const std::string& name,
                 const std::string& description,
                 const Particle& min,
                 const Particle& max);

  virtual ~RangeGenerator() = default;

  virtual void GeneratePrimaryVertex(G4Event* event);
  virtual void SetNewValue(G4UIcommand* command,
                           G4String value);
  virtual std::ostream& Print(std::ostream& os=std::cout) const;
  virtual const Analysis::SimSettingList GetSpecification() const;

  const Particle& min() const { return _min; }
  const Particle& max() const { return _max; }

protected:
  virtual void GenerateCommands();

  Particle _min, _max;
  bool _using_range_ke;
  Command::DoubleUnitArg* _ui_pT_min;
  Command::DoubleUnitArg* _ui_pT_max;
  Command::DoubleArg*     _ui_eta_min;
  Command::DoubleArg*     _ui_eta_max;
  Command::DoubleUnitArg* _ui_phi_min;
  Command::DoubleUnitArg* _ui_phi_max;
  Command::DoubleUnitArg* _ui_ke_min;
  Command::DoubleUnitArg* _ui_ke_max;
};
//----------------------------------------------------------------------------------------------


/*
//__Default Range Particle Generator____________________________________________________________
class RangeGenerator : public Generator {
public:
  RangeGenerator(const std::string& name,
                 const std::string& description,
                 const int id,
                 const double pT,
                 const double eta,
                 const double phi);

  RangeGenerator(const std::string& name,
                 const std::string& description,
                 const int id,
                 const double pT_min,
                 const double pT_max,
                 const double eta_min,
                 const double eta_max,
                 const double phi_min,
                 const double phi_max);

  RangeGenerator(const std::string& name,
                 const std::string& description,
                 const int id,
                 const double pT,
                 const double eta,
                 const double phi,
                 const G4ThreeVector& vertex);

  RangeGenerator(const std::string& name,
                 const std::string& description,
                 const int id,
                 const double pT_min,
                 const double pT_max,
                 const double eta_min,
                 const double eta_max,
                 const double phi_min,
                 const double phi_max,
                 const G4ThreeVector& vertex);

  RangeGenerator(const std::string& name,
                 const std::string& description,
                 const int id,
                 const double pT,
                 const double eta,
                 const double phi,
                 const double t0,
                 const G4ThreeVector& vertex);

  RangeGenerator(const std::string& name,
                 const std::string& description,
                 const int id,
                 const double pT_min,
                 const double pT_max,
                 const double eta_min,
                 const double eta_max,
                 const double phi_min,
                 const double phi_max,
                 const double t0,
                 const G4ThreeVector& vertex);

  virtual ~RangeGenerator() = default;

  virtual void GeneratePrimaryVertex(G4Event* event);
  virtual void SetNewValue(G4UIcommand* command,
                           G4String value);

  double pT_min()  const { return _pT_min;  }
  double pT_max()  const { return _pT_max;  }
  double eta_min() const { return _eta_min; }
  double eta_max() const { return _eta_max; }
  double phi_min() const { return _phi_min; }
  double phi_max() const { return _phi_max; }
  double ke_min()  const { return _ke_min;  }
  double ke_max()  const { return _ke_max;  }

  virtual std::ostream& Print(std::ostream& os=std::cout) const;
  virtual const Analysis::SimSettingList GetSpecification() const;

protected:
  double _pT_min;
  double _pT_max;
  double _eta_min;
  double _eta_max;
  double _phi_min;
  double _phi_max;

  double _ke_min;
  double _ke_max;

  bool _using_range_ke;

  virtual void GenerateCommands();

  Command::DoubleUnitArg* _ui_pT_min;
  Command::DoubleUnitArg* _ui_pT_max;
  Command::DoubleArg*     _ui_eta_min;
  Command::DoubleArg*     _ui_eta_max;
  Command::DoubleUnitArg* _ui_phi_min;
  Command::DoubleUnitArg* _ui_phi_max;
  Command::DoubleUnitArg* _ui_ke_min;
  Command::DoubleUnitArg* _ui_ke_max;
};
//----------------------------------------------------------------------------------------------
*/

//__Stream Operator for Generators______________________________________________________________
inline std::ostream& operator<<(std::ostream& os,
                                const Generator& generator) {
  return generator.Print(os);
}
//----------------------------------------------------------------------------------------------

} /* namespace Physics */ //////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */

#endif /* MU__PHYSICS_GENERATOR_HH */
