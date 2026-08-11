#pragma once

#include <G4ParticleDefinition.hh>

class G4OTrueMuonium1S : public G4ParticleDefinition {
  private:
    static G4OTrueMuonium1S * theInstance;
    G4OTrueMuonium1S();
    ~G4OTrueMuonium1S();
  public:
    static G4OTrueMuonium1S* Definition();
};
