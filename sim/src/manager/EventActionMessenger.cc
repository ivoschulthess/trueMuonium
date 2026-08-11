#include "EventActionMessenger.hh"

#include "EventAction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UImessenger.hh"
#include "G4SystemOfUnits.hh"

#include "G4Tokenizer.hh"
#include "globals.hh"


EventActionMessenger::EventActionMessenger(EventAction* action) // @suppress("Class members should be properly initialized")
:myAction(action)
{ 
  // ---
  EventDir = new G4UIdirectory("/UserEvent/");
  EventDir->SetGuidance("set event parameters");

  // ---
  VerboseLevelCmd = new G4UIcmdWithAnInteger("/UserEvent/VerboseLevel", this);
  VerboseLevelCmd->SetGuidance("Set verbose level");
  VerboseLevelCmd->SetGuidance("  0: No verbose output");
  VerboseLevelCmd->SetGuidance("  1: Additional verbose output");
  VerboseLevelCmd->SetGuidance("  2: Detailes verbose output");
  VerboseLevelCmd->SetParameterName("VerboseLevel", true);
  VerboseLevelCmd->SetDefaultValue(1);
  VerboseLevelCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  // ---
  TriggerLevelCmd = new G4UIcmdWithAnInteger("/UserEvent/TriggerLevel", this);
  TriggerLevelCmd->SetGuidance("Set trigger level");
  TriggerLevelCmd->SetGuidance("  0: No events saved to output");
  TriggerLevelCmd->SetGuidance("  1: Events with muon pair produced saved");
  TriggerLevelCmd->SetGuidance("  2: Events with muon pair produced and hit in detector saved");
  TriggerLevelCmd->SetGuidance("  3: All events saved");
  TriggerLevelCmd->SetDefaultValue(3);
  TriggerLevelCmd->SetParameterName("TriggerLevel", true);
  TriggerLevelCmd->AvailableForStates(G4State_PreInit, G4State_Idle, G4State_GeomClosed);

  // ---
  ROOTFileNameCmd = new G4UIcmdWithAString("/UserEvent/ROOTFileName", this);
  ROOTFileNameCmd->SetGuidance("Set output name for ROOT file");
  ROOTFileNameCmd->SetParameterName("ROOTFileName", false);
  ROOTFileNameCmd->AvailableForStates(G4State_PreInit, G4State_Idle, G4State_GeomClosed);

  // ---
  RNGFileNameCmd = new G4UIcmdWithAString("/UserEvent/RNGFileName", this);
  RNGFileNameCmd->SetGuidance("Set input name for RNG ROOT file");
  RNGFileNameCmd->SetParameterName("RNGFileName", false);
  RNGFileNameCmd->AvailableForStates(G4State_PreInit, G4State_Idle, G4State_GeomClosed);

  // ---
  RNGEventIdxCmd = new G4UIcmdWithAnInteger("/UserEvent/RNGEventIdx", this);
  RNGEventIdxCmd->SetGuidance("Set event index for RNG ROOT file");
  RNGEventIdxCmd->SetParameterName("RNGEventIdx", false);
  RNGEventIdxCmd->AvailableForStates(G4State_PreInit, G4State_Idle, G4State_GeomClosed);
}


EventActionMessenger::~EventActionMessenger()
{
  delete EventDir;
  delete VerboseLevelCmd;
  delete TriggerLevelCmd;
  delete ROOTFileNameCmd;
  delete RNGFileNameCmd;
  delete RNGEventIdxCmd;
}




void EventActionMessenger::SetNewValue(G4UIcommand *command, G4String newValues) {
  if (command == VerboseLevelCmd) {
    myAction->SetVerboseLevel(VerboseLevelCmd->GetNewIntValue(newValues));
  }

  if (command == TriggerLevelCmd){
    myAction->SetTriggerLevel(TriggerLevelCmd->GetNewIntValue(newValues));
  }

  if (command == ROOTFileNameCmd) {
    myAction->SetROOTFileName(newValues);
  }

  if (command == RNGFileNameCmd) {
    myAction->SetRNGFileName(newValues);
  }

  if (command == RNGEventIdxCmd) {
    myAction->SetRNGEventIdx(RNGEventIdxCmd->GetNewIntValue(newValues));
  }

}
