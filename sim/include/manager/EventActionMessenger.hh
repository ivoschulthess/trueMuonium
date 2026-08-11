#pragma once

#include "globals.hh"
#include "G4UImessenger.hh"

class G4UIdirectory;
class G4UIcmdWithABool;
class G4UIcmdWithAnInteger;
class G4UIcmdWithAString;
class EventAction;

class EventActionMessenger: public G4UImessenger
{
  public:
    EventActionMessenger(EventAction* action);
    ~EventActionMessenger();

    void SetNewValue(G4UIcommand* command, G4String newValues);

  private:
    EventAction* myAction;
    
    G4UIdirectory*         EventDir;
    G4UIcmdWithAnInteger*  VerboseLevelCmd;
    G4UIcmdWithAnInteger*  TriggerLevelCmd;
    G4UIcmdWithAString*    ROOTFileNameCmd;
    G4UIcmdWithAString*    RNGFileNameCmd;
    G4UIcmdWithAnInteger*  RNGEventIdxCmd;

};
