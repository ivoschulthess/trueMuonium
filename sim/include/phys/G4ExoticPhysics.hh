#pragma once

#include <G4VPhysicsConstructor.hh>

class G4ExoticPhysics : public G4VPhysicsConstructor {
  public:
    G4ExoticPhysics();
    ~G4ExoticPhysics();

    virtual void ConstructParticle() override;
    virtual void ConstructProcess() override;

  private:
    G4ExoticPhysics(const G4ExoticPhysics &) = delete;
    G4ExoticPhysics & operator=(const G4ExoticPhysics &) = delete;
};
