# MATHUSLA Mu-Simulation
_simulation of muons through earth material_

## Build & Run

This repository comes with a simple build script called `install` that will allow you to customize the build process and execution of the muon simulation.

Here is a list of useful commands:

| Action | Options after `./install` |
|:-:|:-:|
| Build Only | `(none)`  |
| Build and Auto Run | `--run`  |
| Clean CMake Build | `--cmake --clean` |
| More Options | `--help` |

The simulation executable also comes with several configuration parameters:

| Action | Short Options after `./simulation` | Long Options after `./simulation` |
|:-:|:-:|:-:|
| Event Count | `-e <count>` | `--events=<count>`  |
| Particle Generator | `-g <generator>` | `--gen=<generator>` |
| Number of Threads | `-j <count>`  | `--threads=<count>`  |
| Visualization  | `-v` | `--vis` |
| Custom Script  | `-s <file>`  | `--script=<file>`  |
| More Options | `-h` | `--help` |