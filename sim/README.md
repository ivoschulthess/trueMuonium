# True Muonium Simulation Framework


## Usage

The setup scripts are intended for use on a CVMFS-enabled machine, such as CERN's LXPLUS or DESY's NAF. To set up the required environment, configure the project, and compile the program, run

```bash
source rebuild.sh
```

After compilation, a simulation can be started with

```bash
./mkgeant4 macrofile.g4mac
```

Example macro files are provided in the `macros/` directory.


## Structure


### Geometry Construction

- `DetectorConstruction` sets up the world and calls subdetector constructors.
- `ScintConstruction` and `TargetConstruction` define sensitive detector volumes and inherit from `UserSD`, which manages hit collection.


### Sensitive Detectors and Hits

- `UserSD` is the base class for sensitive detectors, handling hit registration.
- `UserHit` defines the hit data structure (energy deposit, position, etc.).


### Event and Run Management

- `EventAction` and `RunAction` manage per-event and per-run actions, including initializing and finalizing ROOT output.
- `EventActionMessenger` and `PrimaryGeneratorActionMessenger` handle user commands and configuration.


### Physics

- Physics files (e.g., `G4ExoticPhysics`) define the physics processes and particles simulated.


### ROOT Output

- During event processing, hits collected by sensitive detectors are passed to `EventAction`, which fills ROOT trees or histograms.
- At the end of each run, `RunAction` writes the accumulated data to the ROOT file.


### File Relationships

- Geometry files define what can be hit.
- Sensitive detector classes collect hit data.
- Manager classes organize event/run flow and handle output.
- Physics files define what happens during simulation.
- ROOT output is managed by the event/run actions, using data from hits.


### Key files for ROOT output:

These files work together to collect, organize, and save simulation data to the ROOT file.

- EventAction.cc
- RunAction.cc
- src/geometry/UserHit.hh
- src/geometry/UserSD.hh

