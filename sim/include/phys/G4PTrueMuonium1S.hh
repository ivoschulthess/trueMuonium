#pragma once

#include <G4ParticleDefinition.hh>

class G4PTrueMuonium1S : public G4ParticleDefinition {
  private:
    static G4PTrueMuonium1S * theInstance;
    G4PTrueMuonium1S();
    ~G4PTrueMuonium1S();
  public:
    static G4PTrueMuonium1S* Definition();
};
