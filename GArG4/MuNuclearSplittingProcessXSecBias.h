////////////////////////////////////////////////////////////////////////
/// \file  MuNuclearSplittingProcessXSecBias.h
///
/// \version $Id: GArG4.h,v 1.11 2010/06/04 21:47:27 bjpjones Exp $
/// \author  brebel@fnal.gov
////////////////////////////////////////////////////////////////////////

#ifndef GARG4MNXS_H
#define GARG4MNXS_H 

#include "Geant4/globals.hh"
#include "Geant4/G4ParticleDefinition.hh"
#include "Geant4/G4ParticleWithCuts.hh"
#include "Geant4/G4ProcessManager.hh"
#include "Geant4/G4ProcessVector.hh"
#include "Geant4/G4ParticleTypes.hh"
#include "Geant4/G4ParticleTable.hh"
#include "Geant4/G4WrapperProcess.hh"

#include "Geant4/G4MuonNuclearProcess.hh"
#include "Geant4/G4Material.hh"
#include "Geant4/G4MaterialTable.hh"
#include "Geant4/G4ios.hh"
#include <iomanip>   

#include "Geant4/G4DataQuestionaire.hh"

namespace gar {
  namespace garg4 {
    
    class MuNuclearSplittingProcessXSecBias : public G4WrapperProcess {
        // Override PostStepDoIt method
    public:
      MuNuclearSplittingProcessXSecBias() {};
      ~MuNuclearSplittingProcessXSecBias() {};
      
      void SetNSplit(G4int nTrx, G4int xB=0, G4double xFac=1) {fNSplit = nTrx, eFactor = (G4double) xFac, xBiasMode = xB;};
      void SetIsActive(G4bool doIt) {fActive = doIt;};
      
      G4VParticleChange* PostStepDoIt(const G4Track& track, const G4Step& step);
      G4VParticleChange* AlongStepDoIt(const G4Track& track, const G4Step& step);
        //////////////////////////
        // GPIL    //////////////
        /////////////////////////
      virtual G4double AlongStepGetPhysicalInteractionLength(
                                                             const G4Track& track,
                                                             G4double  previousStepSize,
                                                             G4double  currentMinimumStep,
                                                             G4double& proposedSafety,
                                                             G4GPILSelection* selection
                                                             );
      virtual G4double PostStepGetPhysicalInteractionLength(
                                                            const G4Track& track,
                                                            G4double   previousStepSize,
                                                            G4ForceCondition* condition
                                                            );
    protected:
      
      virtual void ResetNumberOfInteractionLengthLeft()
      {
        G4VProcess::theNumberOfInteractionLengthLeft =  -std::log( G4UniformRand() );
        theInitialNumberOfInteractionLength = G4VProcess::theNumberOfInteractionLengthLeft;
      }
      
      
    private:
        // Data members
      G4int fNSplit;
      G4bool fActive;
      G4int xBiasMode;
      G4double eFactor; // enhancement factor to the cross-setion
      
      G4VParticleChange fParticleChange;
        // weight change applied at AlongStepDoIt()
      G4double wc;
      G4double theInitialNumberOfInteractionLength; 
      
      G4double XBiasSurvivalProbability();
      G4double XBiasSecondaryWeight();
      G4double GetTotalNumberOfInteractionLengthTraversed();
      
      
    }; 
    
    
  }// end namespace
} // gar

#endif // MNSP
