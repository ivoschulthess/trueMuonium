#include "RunAction.hh"

#include "G4Run.hh"

#include "G4ProcessManager.hh"

#include "G4Gamma.hh"
#include "G4Positron.hh"
#include "G4GammaConversionToMuons.hh"
#include "G4AnnihiToMuPair.hh"

#include "G4SystemOfUnits.hh"


RunAction::RunAction()
{;}


RunAction::~RunAction()
{;}


void RunAction::BeginOfRunAction(const G4Run* aRun)
{
  // Bias it (comment or uncomment just one next line)
  G4double BiasFactor = 1.e6;

  // Turn on gamma to muons conversion:
  G4ProcessManager* pManager = G4Gamma::Gamma()->GetProcessManager();
  G4GammaConversionToMuons* mygammatomu = new G4GammaConversionToMuons;
  const G4ParticleDefinition* myGamma = G4Gamma::GammaDefinition();
  if(fabs(BiasFactor - 1.) > 0.001) {
    mygammatomu->SetCrossSecFactor(BiasFactor);
  }
  pManager->AddDiscreteProcess(mygammatomu);
  mygammatomu->BuildPhysicsTable(*myGamma);

  // Turn on e+e- --> mu+ mu-
  // --> Not relevant for photoproduction channel
  //
  /*G4AnnihiToMuPair* epemTOmupmum = new G4AnnihiToMuPair;*/
  /*pManager = G4Positron::Positron()->GetProcessManager();*/
  /*if(fabs(BiasFactor - 1.) > 0.001) {*/
  /*    epemTOmupmum->SetCrossSecFactor(BiasFactor);*/
  /*}*/
  /*pManager->AddDiscreteProcess(epemTOmupmum);*/
  /*epemTOmupmum->BuildPhysicsTable(*myGamma);*/
}


void RunAction::EndOfRunAction(const G4Run* thisRun)
{;}
