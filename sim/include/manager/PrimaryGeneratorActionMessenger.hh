#pragma once

#include "G4UImessenger.hh"

class PrimaryGeneratorAction;
class G4UIdirectory;
class G4UIcmdWithABool;
class G4UIcmdWithAnInteger;


class PrimaryGeneratorActionMessenger : public G4UImessenger {
  public:
    PrimaryGeneratorActionMessenger(PrimaryGeneratorAction* PGA);
    ~PrimaryGeneratorActionMessenger();

    void SetNewValue(G4UIcommand* command, G4String newValues);

  private:
    PrimaryGeneratorAction* myPGA;

    G4UIdirectory* GenerateDir;
    G4UIcmdWithABool* GenerateCmd;
    G4UIcmdWithABool* SpinCmd;
    G4UIcmdWithAnInteger* AtomicStateCmd;
};
