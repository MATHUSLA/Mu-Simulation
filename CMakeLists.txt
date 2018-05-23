cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

project(MATHUSLA_MU)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)
set(CMAKE_CXX_COMPILER "/usr/bin/g++")

find_package(Geant4  REQUIRED gdml ui_all vis_all)
find_package(Pythia8 REQUIRED)

include(${Geant4_USE_FILE})

add_executable(simulation
    src/simulation.cc

    src/action/ActionInitialization.cc
    src/action/EventAction.cc
    src/action/GeneratorAction.cc
    src/action/RunAction.cc

    src/analysis.cc

    src/detector/Construction.cc
    src/detector/Earth.cc

    src/detector/flat/Flat.cc
    src/detector/flat/Layer.cc
    src/detector/flat/Scintillator.cc

    src/detector/prototype/Envelope.cc
    src/detector/prototype/Prototype.cc
    src/detector/prototype/RPC.cc
    src/detector/prototype/Scintillator.cc

    src/physics/Generator.cc
    src/physics/PythiaGenerator.cc

    src/tracking.cc

    src/util/command_line_parser.cc
)

target_link_libraries(simulation
    ${Geant4_LIBRARIES}
    ${PYTHIA8_LIBRARY})

target_include_directories(simulation PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_SOURCE_DIR}/include>)

install(DIRECTORY scripts DESTINATION bin)
install(TARGETS simulation DESTINATION bin)
