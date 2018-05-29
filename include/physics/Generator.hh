/* include/physics/Generator.hh
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

#include "Geant4/G4Event.hh"
#include "Geant4/G4PrimaryVertex.hh"
#include "Geant4/G4PrimaryParticle.hh"

#include "ui.hh"

namespace MATHUSLA { namespace MU {

class Generator : public G4UImessenger {
public:
  struct PseudoLorentzTriplet { double pT, eta, phi; };

  Generator(const std::string& name,
            const std::string& description,
            const int id,
            const double pT,
            const double eta,
            const double phi);

  Generator(const std::string& name,
            const std::string& description,
            const int id,
            const double ke,
            const G4ThreeVector& p_unit);

  virtual ~Generator() = default;

  virtual void GeneratePrimaryVertex(G4Event* event);
  virtual void SetNewValue(G4UIcommand* command, G4String value);

  const std::string&   GetName() const { return _name;   }
  int                  id()      const { return _id;     }
  double               pT()      const { return _pT;     }
  double               eta()     const { return _eta;    }
  double               phi()     const { return _phi;    }
  double               ke()      const { return _ke;     }
  const G4ThreeVector& p_unit()  const { return _p_unit; }

  const G4ThreeVector p() const;

  virtual std::ostream& Print(std::ostream& os=std::cout) const;

  static G4PrimaryVertex* DefaultVertex();

  static G4PrimaryParticle* CreateParticle(const int id,
                                           const G4ThreeVector& p);

  static G4PrimaryParticle* CreateParticle(const int id,
                                           const double pT,
                                           const double eta,
                                           const double phi);

  static double GetMass(const int id);
  static G4ThreeVector GetMomentum(const double mass,
                                   const double ke,
                                   const G4ThreeVector& p_unit);

  static PseudoLorentzTriplet Convert(const G4ThreeVector& momentum);
  static G4ThreeVector Convert(const PseudoLorentzTriplet& triplet);

  static const std::string MessengerDirectory;

protected:
  std::string _name;
  std::string _description;

  int    _id;
  double _pT;
  double _eta;
  double _phi;

  double _ke;
  G4ThreeVector _p_unit;
  double _mass;

  bool _using_pt_eta_phi;

  void GenerateCommands();

  Command::IntegerArg*     _ui_id;
  Command::DoubleUnitArg*  _ui_pT;
  Command::DoubleArg*      _ui_eta;
  Command::DoubleUnitArg*  _ui_phi;
  Command::DoubleUnitArg*  _ui_ke;
  Command::ThreeVectorArg* _ui_p;
};

////////////////////////////////////////////////////////////////////////////////////////////////

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

  virtual ~RangeGenerator() = default;

  virtual void GeneratePrimaryVertex(G4Event* event);
  virtual void SetNewValue(G4UIcommand* command, G4String value);

  double pT_min()  const { return _pT_min;  }
  double pT_max()  const { return _pT_max;  }
  double eta_min() const { return _eta_min; }
  double eta_max() const { return _eta_max; }
  double phi_min() const { return _phi_min; }
  double phi_max() const { return _phi_max; }

  virtual std::ostream& Print(std::ostream& os=std::cout) const;

protected:
  double _pT_min;
  double _pT_max;
  double _eta_min;
  double _eta_max;
  double _phi_min;
  double _phi_max;

  Command::DoubleUnitArg* _ui_pT_min;
  Command::DoubleUnitArg* _ui_pT_max;
  Command::DoubleArg*     _ui_eta_min;
  Command::DoubleArg*     _ui_eta_max;
  Command::DoubleUnitArg* _ui_phi_min;
  Command::DoubleUnitArg* _ui_phi_max;
};

////////////////////////////////////////////////////////////////////////////////////////////////

//__Stream Operator for Generators______________________________________________________________
inline std::ostream& operator<<(std::ostream& os,
                                const Generator& generator) {
  return generator.Print(os);
}
//----------------------------------------------------------------------------------------------

} } /* namespace MATHUSLA::MU */

#endif /* MU__PHYSICS_GENERATOR_HH */
