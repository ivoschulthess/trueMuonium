#include "PrimaryGeneratorActionMessenger.hh"

#include "PrimaryGeneratorAction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithAnInteger.hh"

PrimaryGeneratorActionMessenger::PrimaryGeneratorActionMessenger(PrimaryGeneratorAction* PGA)
  : myPGA(PGA)
{ 
  GenerateDir = new G4UIdirectory("/true-muonium/");
  GenerateDir->SetGuidance("enable true muonium generation");

  // generate signal
  GenerateCmd = new G4UIcmdWithABool("/true-muonium/generate", this);
  GenerateCmd->SetGuidance("Set generate flag");
  GenerateCmd->SetParameterName("generate", true);
  GenerateCmd->SetDefaultValue(false);
  GenerateCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  // generate spin state
  SpinCmd = new G4UIcmdWithABool("/true-muonium/spin", this);
  SpinCmd->SetGuidance("Set spin state");
  SpinCmd->SetParameterName("spin", true);
  SpinCmd->SetDefaultValue(true);
  SpinCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  // generate excited state
  AtomicStateCmd = new G4UIcmdWithAnInteger("/true-muonium/state", this);
  AtomicStateCmd->SetGuidance("Set atomic state");
  AtomicStateCmd->SetParameterName("state", true);
  AtomicStateCmd->SetDefaultValue(0);
  AtomicStateCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}


PrimaryGeneratorActionMessenger::~PrimaryGeneratorActionMessenger()
{
  delete GenerateCmd;
  delete SpinCmd;
  delete AtomicStateCmd;
  delete GenerateDir;
}


void PrimaryGeneratorActionMessenger::SetNewValue(G4UIcommand* command, G4String newValues)
{
  if (command == GenerateCmd) {
    myPGA->SetGenerateTM(GenerateCmd->GetNewBoolValue(newValues));
  }
  if (command == SpinCmd) {
    myPGA->SetSpinAllignTM(SpinCmd->GetNewBoolValue(newValues));
  }
  if (command == AtomicStateCmd) {
    myPGA->SetAtomicState(AtomicStateCmd->GetNewIntValue(newValues));
  }
}

