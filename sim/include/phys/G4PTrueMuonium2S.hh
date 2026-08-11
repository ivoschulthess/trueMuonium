#pragma once

#include <G4ParticleDefinition.hh>

class G4PTrueMuonium2S : public G4ParticleDefinition {
  private:
    static G4PTrueMuonium2S * theInstance;
    G4PTrueMuonium2S();
    ~G4PTrueMuonium2S();
  public:
    static G4PTrueMuonium2S* Definition();
};
