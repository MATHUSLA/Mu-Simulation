/*
 * src/physics/HepMCGenerator.cc
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

#include "physics/HepMCGenerator.hh"

#include "physics/Units.hh"

namespace MATHUSLA { namespace MU {

namespace Physics { ////////////////////////////////////////////////////////////////////////////

namespace { ////////////////////////////////////////////////////////////////////////////////////

//__Convert HepMC Four Vector to Geant4 ThreeVector_____________________________________________
const G4ThreeVector _to_G4ThreeVector(const HepMC::FourVector& vector) {
  return G4ThreeVector(vector.x(), vector.y(), vector.z());
}
//----------------------------------------------------------------------------------------------

//__Add HepMC Particles to Vertex_______________________________________________________________
void _add_to_vertex(G4PrimaryVertex* vertex,
                    const PropagationList& propagation,
                    const std::vector<HepMC::GenParticlePtr>& particles) {
  for (const auto& particle : particles) {
    const auto momentum = _to_G4ThreeVector(particle->momentum());
    if (InPropagationList(propagation, particle->pid(), momentum)) {
      vertex->SetPrimary(CreateParticle(particle->pid(), momentum));
    }
  }
}
//----------------------------------------------------------------------------------------------

} /* anonymous namespace */ ////////////////////////////////////////////////////////////////////

//__HepMC Generator Constructor_________________________________________________________________
HepMCGenerator::HepMCGenerator(const PropagationList& propagation,
                               bool unique_events)
    : Generator("hepmc", "HepMC ROOT Reader", 0, 0, 0, 0),
      _reader(nullptr), _propagation_list(propagation), _unique(unique_events) {
  _read_file = CreateCommand<Command::StringArg>("readFile", "Read HepMC ROOT File.");
  _read_file->SetParameterName("file", false);
  _read_file->AvailableForStates(G4State_PreInit, G4State_Idle);
}
//----------------------------------------------------------------------------------------------

//__Messenger Set Value_________________________________________________________________________
void HepMCGenerator::SetNewValue(G4UIcommand* command, G4String value) {
  if (command == _read_file) {
    if (_reader) {
      //_reader->close();
      delete _reader;
    }
    _reader = new HepMC::ReaderRoot(value);
  }
}
//----------------------------------------------------------------------------------------------

//__Generate Primary Verticies__________________________________________________________________
void HepMCGenerator::GeneratePrimaryVertex(G4Event* event) {
  if (_reader && !_reader->failed()) {
    _reader->read_event(_current_event);

    if (_reader->failed())
      return;

    for (const auto& vertex : _current_event.vertices()) {
      auto propagated_vertex = DefaultVertex(); // FIXME: is this correct?
      _add_to_vertex(propagated_vertex, _propagation_list, vertex->particles_in());
      _add_to_vertex(propagated_vertex, _propagation_list, vertex->particles_out());
      if (propagated_vertex->GetNumberOfParticle() != 0)
        event->AddPrimaryVertex(propagated_vertex);
    }
  }
}
//----------------------------------------------------------------------------------------------

} /* namespace Physics */ //////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */
