/*
 * src/physics/PythiaGenerator.cc
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

#include "physics/PythiaGenerator.hh"

#include <Pythia8/ParticleData.h>

#include "physics/Units.hh"
#include "util/string.hh"

namespace MATHUSLA { namespace MU {

namespace Physics { ////////////////////////////////////////////////////////////////////////////

//__G4ThreadLocal Static Variables______________________________________________________________
G4ThreadLocal Pythia8::Pythia* PythiaGenerator::_pythia = nullptr;
G4ThreadLocal std::vector<std::string>* PythiaGenerator::_pythia_settings = nullptr;
G4ThreadLocal bool PythiaGenerator::_settings_on = false;
//----------------------------------------------------------------------------------------------

//__Pythia Generator Construction_______________________________________________________________
PythiaGenerator::PythiaGenerator(const PropagationList& propagation,
                                 Pythia8::Pythia* pythia)
    : Generator("pythia", "Pythia8 Generator."), _propagation_list(propagation) {
  _pythia_settings = new std::vector<std::string>();
  SetPythia(pythia);

  _read_string = CreateCommand<Command::StringArg>("read_string", "Read Pythia String.");
  _read_string->SetParameterName("string", false);
  _read_string->AvailableForStates(G4State_PreInit, G4State_Idle);

  _read_file = CreateCommand<Command::StringArg>("read_file", "Read Pythia File.");
  _read_file->SetParameterName("file", false);
  _read_file->AvailableForStates(G4State_PreInit, G4State_Idle);

  _add_cut = CreateCommand<Command::StringArg>("cuts/add", "Add Cut to Pythia Filter");
  _add_cut->SetParameterName("cut", false);
  _add_cut->AvailableForStates(G4State_PreInit, G4State_Idle);

  _clear_cuts = CreateCommand<Command::NoArg>("cuts/clear", "Clear Cuts from Pythia Filter");
  _clear_cuts->AvailableForStates(G4State_PreInit, G4State_Idle);

  _process = CreateCommand<Command::StringArg>("process", "Specify Pythia Process.");
  _process->SetParameterName("process", false);
  _process->AvailableForStates(G4State_PreInit, G4State_Idle);
}
//----------------------------------------------------------------------------------------------

//__Pythia Generator Construction_______________________________________________________________
PythiaGenerator::PythiaGenerator(const PropagationList& propagation,
                                 const std::vector<std::string>& settings)
    : PythiaGenerator(propagation) {
  SetPythia(settings);
}
//----------------------------------------------------------------------------------------------

//__Pythia Generator Construction_______________________________________________________________
PythiaGenerator::PythiaGenerator(const PropagationList& propagation,
                                 const std::string& path)
    : PythiaGenerator(propagation) {
  SetPythia(path);
}
//----------------------------------------------------------------------------------------------

//__Pythia Generator Construction_______________________________________________________________
PythiaGenerator::PythiaGenerator(Pythia8::Pythia* pythia) : PythiaGenerator({}, pythia) {}
//----------------------------------------------------------------------------------------------

//__Pythia Generator Construction_______________________________________________________________
PythiaGenerator::PythiaGenerator(const std::vector<std::string>& settings) : PythiaGenerator({}, settings) {}
//----------------------------------------------------------------------------------------------

//__Pythia Generator Construction_______________________________________________________________
PythiaGenerator::PythiaGenerator(const std::string& path) : PythiaGenerator({}, path) {}
//----------------------------------------------------------------------------------------------

namespace { ////////////////////////////////////////////////////////////////////////////////////

//__Setup Pythia Randomness_____________________________________________________________________
Pythia8::Pythia* _setup_random(Pythia8::Pythia* pythia) {
  pythia->readString("Random:setSeed = on");
  pythia->readString("Random:seed = 0");
  return pythia;
}
//----------------------------------------------------------------------------------------------

//__Reconstruct Pythia Object from Old Object___________________________________________________
Pythia8::Pythia* _reconstruct_pythia(Pythia8::Pythia* pythia) {
  if (!pythia) {
    return new Pythia8::Pythia();
  } else {
    auto out = new Pythia8::Pythia(pythia->settings, pythia->particleData);
    _setup_random(out);
    return out;
  }
}
//----------------------------------------------------------------------------------------------

//__Create Pythia from Settings_________________________________________________________________
Pythia8::Pythia* _create_pythia(std::vector<std::string>* settings,
                                bool& settings_on) {
  auto pythia = new Pythia8::Pythia();
  for (const auto& setting : *settings)
    pythia->readString(setting);
  _setup_random(pythia);
  pythia->init();
  settings_on = true;
  return pythia;
}
//----------------------------------------------------------------------------------------------

//__Get Pythia Event Type_______________________________________________________________________
Pythia8::Event _get_event(Pythia8::Pythia* pythia, const std::string& type) {
  const auto type_string = util::string::strip(type);
  if (type_string == "hard") {
    return pythia->process;
  } else if (type_string == "soft") {
    const auto& process = pythia->process;
    const auto process_size = static_cast<std::size_t>(process.size());
    const auto& event = pythia->event;
    const auto event_size = static_cast<std::size_t>(event.size());
    Pythia8::Event out;
    for (std::size_t i{process_size}; i < event_size; ++i)
      out.append(event[i]);
    return out;
  }
  return pythia->event;
}
//----------------------------------------------------------------------------------------------

//__Convert Pythia Particle to Particle_________________________________________________________
Particle _convert_particle(Pythia8::Particle& particle) {
  Particle out{particle.id(),
               particle.tProd() * mm / c_light,
               particle.zProd() * mm,
               particle.yProd() * mm,
              -particle.xProd() * mm + 81*m};
  out.set_pseudo_lorentz_triplet(particle.pT() * GeVperC, particle.eta(), particle.phi() * rad);
  return out;
}
//----------------------------------------------------------------------------------------------

//__Convert and Pushback Pythia8 Particle if Predicate__________________________________________
template<class Predicate>
bool _push_back_convert_if(ParticleVector& out,
                           Pythia8::Particle& particle,
                           Predicate predicate) {
  const auto next = _convert_particle(particle);
  const auto passed_selection = predicate(next);
  if (passed_selection)
    out.push_back(next);
  return passed_selection;
}
//----------------------------------------------------------------------------------------------

//__Convert Pythia Hard and Soft Processes______________________________________________________
template<class Predicate>
ParticleVector _convert_pythia_event(Pythia8::Event event,
                                     Predicate predicate) {
  ParticleVector out;
  for (int i = 0; i < event.size(); ++i) {
    if (!event[i].isFinal())
      continue;
    _push_back_convert_if(out, event[i], predicate);
  }
  return out;
}
//----------------------------------------------------------------------------------------------

} /* anonymous namespace */ ////////////////////////////////////////////////////////////////////

//__Generate Initial Particles__________________________________________________________________
void PythiaGenerator::GeneratePrimaryVertex(G4Event* event) {
  if (!_settings_on && !_pythia_settings->empty()) {
    _pythia = _create_pythia(_pythia_settings, _settings_on);
  } else if (!_pythia) {
    std::cout << "\n[ERROR] No Pythia Configuration Specified.\n";
  }

  ++_counter;
  _pythia->next();

  _last_event = _convert_pythia_event(_get_event(_pythia, _process_string), [&](const auto& next) {
    for (const auto& entry : _propagation_list)
      if (next.id == entry.id)
        return true;
    return false;
  });

  for (const auto& particle : _last_event)
    if (InPropagationList(_propagation_list, particle))
      AddParticle(particle, *event);
}
//----------------------------------------------------------------------------------------------

//__Get Last Event Data_________________________________________________________________________
ParticleVector PythiaGenerator::GetLastEvent() const {
  return _last_event;
}
//----------------------------------------------------------------------------------------------

//__Messenger Set Value_________________________________________________________________________
void PythiaGenerator::SetNewValue(G4UIcommand* command,
                                  G4String value) {
  if (command == _read_string) {
    _pythia_settings->push_back(value);
    _settings_on = true;
  } else if (command == _read_file) {
    SetPythia(value);
  } else if (command == _add_cut) {
    for (const auto& cut : ParsePropagationList(value))
      _propagation_list.push_back(cut);
  } else if (command == _clear_cuts) {
    _propagation_list.clear();
  } else if (command == _process) {
    _process_string = value;
  } else {
    Generator::SetNewValue(command, value);
  }
}
//----------------------------------------------------------------------------------------------

//__Set Pythia Object from Copy_________________________________________________________________
void PythiaGenerator::SetPythia(Pythia8::Pythia* pythia) {
  if (!pythia)
    return;
  _counter = 0ULL;
  _pythia_settings->clear();
  _settings_on = false;
  _pythia = _reconstruct_pythia(pythia);
  _pythia->init();
}
//----------------------------------------------------------------------------------------------

//__Set Pythia Object from Settings_____________________________________________________________
void PythiaGenerator::SetPythia(const std::vector<std::string>& settings) {
  *_pythia_settings = settings;
  _counter = 0ULL;
  _pythia = _create_pythia(_pythia_settings, _settings_on);
}
//----------------------------------------------------------------------------------------------

//__Set Pythia Object from Settings_____________________________________________________________
void PythiaGenerator::SetPythia(const std::string& path) {
  _counter = 0ULL;
  _pythia_settings->clear();
  _settings_on = false;
  _path = path;
  _pythia = new Pythia8::Pythia();
  _pythia->readFile(_path);
  _setup_random(_pythia);
  _pythia->init();
}
//----------------------------------------------------------------------------------------------

//__PythiaGenerator Specifications______________________________________________________________
const Analysis::SimSettingList PythiaGenerator::GetSpecification() const {
  Analysis::SimSettingList config;
  if (_path.empty() && !_pythia_settings->empty()) {
    config = Analysis::IndexedSettings(SimSettingPrefix, "_SETTING_", *_pythia_settings);
  } else if (!_path.empty()) {
    config.emplace_back(SimSettingPrefix, "_CONFIG", _path);
  }

  Analysis::SimSettingList out;
  out.reserve(2UL + config.size() + _propagation_list.size());
  out.emplace_back(SimSettingPrefix, "", _name);
  out.insert(out.cend(),
             std::make_move_iterator(config.begin()),
             std::make_move_iterator(config.end()));

  std::vector<std::string> cut_strings;
  cut_strings.reserve(_propagation_list.size());
  for (const auto& cut : _propagation_list)
    cut_strings.push_back(GetParticleCutString(cut));

  auto cuts = Analysis::IndexedSettings(SimSettingPrefix, "_CUTS_", cut_strings);
  out.insert(out.cend(),
             std::make_move_iterator(cuts.begin()),
             std::make_move_iterator(cuts.end()));
  out.emplace_back(SimSettingPrefix, "_EVENTS", std::to_string(_counter));

  return out;
}
//----------------------------------------------------------------------------------------------

} /* namespace Physics */ //////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */
