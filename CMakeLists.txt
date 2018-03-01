cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

project(MATHUSLA_MU)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

find_package(Geant4 REQUIRED ui_all vis_all)
find_package(Pythia8 REQUIRED)
find_package(ROOT REQUIRED)

include(${Geant4_USE_FILE})

set(SOURCES
    src/simulation.cc

    src/action/ActionInitialization.cc
    src/action/EventAction.cc
    src/action/PrimaryGeneratorAction.cc
    src/action/RunAction.cc
    src/action/StackingAction.cc
    src/action/SteppingAction.cc
    src/action/TrackingAction.cc

    src/analysis/HistoManager.cc

    src/detector/Construction.cc
    src/detector/Earth.cc
    src/detector/Envelope.cc
    src/detector/Scintillator.cc
    src/detector/Prototype.cc

    src/tracking/PrototypeHit.cc

    src/physics/PhysicsList.cc

    src/ui/Messenger.cc
)

add_executable(simulation ${SOURCES})
target_link_libraries(simulation ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})

target_include_directories(simulation PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_SOURCE_DIR}/include>)

install(DIRECTORY scripts DESTINATION bin)
install(TARGETS simulation DESTINATION bin)
