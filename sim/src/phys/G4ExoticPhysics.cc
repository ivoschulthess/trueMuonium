#include "G4ExoticPhysics.hh"

#include "G4OTrueMuonium1S.hh"
#include "G4OTrueMuonium2S.hh"
#include "G4PTrueMuonium1S.hh"
#include "G4PTrueMuonium2S.hh"

#include "G4BuilderType.hh"
#include "G4SystemOfUnits.hh"

G4ExoticPhysics::G4ExoticPhysics() 
  : G4VPhysicsConstructor("ExoticPhysics")
{
  SetPhysicsType(bUnknown);
}

G4ExoticPhysics::~G4ExoticPhysics()
{
  // ...
}

void G4ExoticPhysics::ConstructParticle()
{
  G4OTrueMuonium1S::Definition();
  G4OTrueMuonium2S::Definition();
  G4PTrueMuonium1S::Definition();
  G4PTrueMuonium2S::Definition();
}

void G4ExoticPhysics::ConstructProcess()
{
  // ...
}
