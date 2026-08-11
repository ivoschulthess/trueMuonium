#include "G4OTrueMuonium1S.hh"
#include "G4ParticleTable.hh"

#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhaseSpaceDecayChannel.hh"
#include "G4DalitzDecayChannel.hh"
#include "G4DecayTable.hh"

#include "G4VDecayChannel.hh"
#include "G4MuonMinus.hh"

G4OTrueMuonium1S * G4OTrueMuonium1S::theInstance = nullptr;

G4OTrueMuonium1S * G4OTrueMuonium1S::Definition()
{
  if ( theInstance ) {
    return theInstance;
  }
  const G4String name = "otm-1s";

  // search in particle table
  G4ParticleTable * pTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition * anInstance = pTable->FindParticle(name);

  // get width 
  const G4double muMass = 
    G4MuonMinus::MuonMinusDefinition()->GetPDGMass()/MeV;
  const G4double alpha = CLHEP::fine_structure_const;
  const G4double width = pow(alpha,5.)*muMass/6.;      // in MeV

  if ( !anInstance ) {
    anInstance = new G4ParticleDefinition(
        /* Name ..................... */ name,
        /* Mass ..................... */ 2*muMass,
        /* Decay width .............. */ width,
        /* Charge ................... */ 0.*eplus,
        /* 2*spin ................... */ 1,
        /* parity ................... */ 0,
        /* C-conjugation ............ */ 0,
        /* 2*Isospin ................ */ 0,
        /* 2*Isospin3 ............... */ 0,
        /* G-parity ................. */ 0,
        /* type ..................... */ "atom",
        /* lepton number ............ */ 0,
        /* baryon number ............ */ 0,
        /* PDG encoding ............. */ 998, // https://pdg.lbl.gov/2019/reviews/rpp2019-rev-monte-carlo-numbering.pdf
        /* stable ................... */ false, // allow decay
        /* lifetime.................. */ 0, 
        /* decay table .............. */ NULL,
        /* shortlived ............... */ false,
        /* subType .................. */ "otm-1s",
        /* anti particle encoding ... */ 998
          );

    anInstance->SetPDGLifeTime(CLHEP::hbar_Planck/anInstance->GetPDGWidth()*ns);  // life time in ns

    // create decay table and add mode
    G4DecayTable* table = new G4DecayTable();
    G4VDecayChannel* mode = 
      new G4PhaseSpaceDecayChannel(name, 1., 2, "e+", "e-");
    table->Insert(mode);
    anInstance->SetDecayTable(table);
    anInstance->DumpTable();

  }

  theInstance = reinterpret_cast<G4OTrueMuonium1S*>(anInstance);
  return theInstance;
}


