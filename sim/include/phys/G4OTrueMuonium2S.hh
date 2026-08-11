#pragma once

#include <G4ParticleDefinition.hh>

class G4OTrueMuonium2S : public G4ParticleDefinition {
  private:
    static G4OTrueMuonium2S * theInstance;
    G4OTrueMuonium2S();
    ~G4OTrueMuonium2S();
  public:
    static G4OTrueMuonium2S* Definition();
};
