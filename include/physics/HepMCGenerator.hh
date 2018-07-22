/*
 * include/physics/HepMCGenerator.hh
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

#ifndef MU__PHYSICS_HEPMCGENERATOR_HH
#define MU__PHYSICS_HEPMCGENERATOR_HH
#pragma once

// #include <HepMC/HepMC.h>
// #include <HepMC/ReaderAscii.h>
// #include <HepMC/ReaderRoot.h>

#include "physics/Generator.hh"

namespace MATHUSLA { namespace MU {

namespace Physics { ////////////////////////////////////////////////////////////////////////////

class HepMCGenerator : public Generator {
public:
  HepMCGenerator(const PropagationList& propagation,
                 bool unique_events=true);

  const PropagationList GetPropagationList() const { return _propagation_list; };
  bool UsingUniqueEvents() const { return _unique; }
  void SetNewValue(G4UIcommand* command, G4String value);
  void GeneratePrimaryVertex(G4Event* event);

  virtual const Analysis::SimSettingList GetSpecification() const;

protected:
  // TODO: union { HepMC::ReaderAscii* ascii; HepMC::ReaderRoot* root; } _reader;
  // HepMC::ReaderAscii* _reader;
  bool _using_root_reader;
  PropagationList _propagation_list;
  // HepMC::GenEvent _current_event;
  bool _unique;
  Command::StringArg* _read_file;
};

} /* namespace Physics */ //////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */

#endif /* MU__PHYSICS_HEPMCGENERATOR_HH */
