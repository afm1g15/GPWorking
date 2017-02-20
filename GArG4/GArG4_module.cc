////////////////////////////////////////////////////////////////////////
/// \file  GArG4.cxx
/// \brief Use Geant4 to run the LArSoft detector simulation
///
/// \version $Id: GArG4.cxx,v 1.22 2010/07/20 06:08:30 bjpjones Exp $
/// \author  seligman@nevis.columbia.edu
////////////////////////////////////////////////////////////////////////

/// This a module.  It has the following functions:
///
/// - Initialize Geant4 physics, detector geometry, and other
///   processing.
///
/// - Accept sim::MCTruth objects from the MC branch of the FMWK
///   Event structure.
///
/// - Pass the primary particles to the Geant4 simulation to calculate
///   "truth" information for the detector response.
///
/// - Pass the truth information to the DetSim branch of the FMWK event.

#ifndef GARG4GARG4H
#define GARG4GARG4H 1

#include "nutools/G4Base/G4Helper.h"
#include "nutools/G4Base/ConvertMCTruthToG4.h"

// C++ Includes
#include <sstream> // std::ostringstream
#include <vector> // std::ostringstream
#include <map> // std::ostringstream
#include <set> // std::ostringstream
#include <iostream>
// #include <cstring>
#include <sys/stat.h>

// Framework includes
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/Optional/RandomNumberGenerator.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "canvas/Persistency/Common/Ptr.h"
#include "canvas/Persistency/Common/Assns.h"
#include "cetlib/exception.h"
#include "cetlib/search_path.h"

// nutools extensions
#include "nusimdata/SimulationBase/MCTruth.h"
#include "nutools/ParticleNavigation/ParticleList.h"
#include "nutools/G4Base/DetectorConstruction.h"
#include "nutools/G4Base/UserActionManager.h"
#include "nutools/RandomUtils/NuRandomService.h"

// GArSoft Includes
#include "GArG4/AuxDetAction.h"
#include "GArG4/GArAction.h"
#include "GArG4/PhysicsList.h"
#include "GArG4/ParticleListAction.h"
#include "GArG4/MaterialPropertyLoader.h"
#include "GArG4/ParticleFilters.h" // garg4::PositionInVolumeFilter
#include "GArG4/G4SimulationParameters.h"
#include "Utilities/AssociationUtil.h"
#include "SimulationDataProducts/EnergyDeposit.h"
#include "SimulationDataProducts/AuxDetSimChannel.h"
#include "Geometry/Geometry.h"

// G4 Includes
#include "Geant4/G4RunManager.hh"
#include "Geant4/G4UImanager.hh"
#include "Geant4/G4VUserDetectorConstruction.hh"
#include "Geant4/G4VUserPrimaryGeneratorAction.hh"
#include "Geant4/G4VUserPhysicsList.hh"
#include "Geant4/G4UserRunAction.hh"
#include "Geant4/G4UserEventAction.hh"
#include "Geant4/G4UserTrackingAction.hh"
#include "Geant4/G4UserSteppingAction.hh"
#include "Geant4/G4UserStackingAction.hh"
#include "Geant4/G4VisExecutive.hh"
#include "Geant4/G4VUserPhysicsList.hh"
#include "Geant4/G4SDManager.hh"
#include "Geant4/G4LogicalVolumeStore.hh"
#include "Geant4/Randomize.hh"
#include "Geant4/G4SDManager.hh"
#include "Geant4/G4VSensitiveDetector.hh"
#include "Geant4/globals.hh"

// ROOT Includes
#include "TGeoManager.h"

// Forward declarations
class G4RunManager;
class G4UImanager;
class G4VisExecutive;

///Geant4 interface
namespace gar {
  namespace garg4 {
    
    // Forward declarations within namespace.
    class ParticleListAction;
    
    /**
     * @brief Runs Geant4 simulation and propagation of electrons and photons to readout
     *
     *
     * Randomness
     * -----------
     *
     * The random number generators used by this process are:
     * - 'GEANT' instance: used by Geant4
     * - 'propagation' instance: used in electron propagation
     * - 'radio' instance: used for radiological decay
     *
     *
     * Configuration parameters
     * -------------------------
     *
     * - <b>G4PhysListName</b> (string, default "garg4::PhysicsList"):
     *     whether to use the G4 overlap checker, which catches different issues than ROOT
     * - <b>CheckOverlaps</b> (bool, default false):
     *     whether to use the G4 overlap checker
     * - <b>DumpParticleList</b> (bool, default false):
     *     whether to print all MCParticles tracked
     * - <b>SmartStacking</b> (int, default 0):
     *     whether to use class to dictate how tracks are put on stack (nonzero is on)
     * - <b>KeepParticlesInVolumes</b> (vector<string>, default empty):
     *     list of volumes in which to keep MCParticles (empty keeps all)
     * - <b>GeantCommandFile</b> (string, required):
     *     G4 macro file to pass to G4Helper for setting G4 command
     * - <b>Seed</b> (pset key, not defined by default): if defined, override the seed for
     *     random number generator used in Geant4 simulation (which is obtained from
     *     NuRandomService by default)
     * - <b>PropagationSeed</b> (pset key, not defined by default): if defined,
     *     override the seed for the random generator used for electrons propagation
     *     to the wire planes (obtained from the NuRandomService by default)
     * - <b>RadioSeed</b> (pset key, not defined by default): if defined,
     *     override the seed for the random generator used for radiological decay
     *     (obtained from the NuRandomService by default)
     * - <b>InputLabels</b> (vector<string>, defualt unnecessary):
     *     optional list of generator labels which produce MCTruth;
     *     otherwise look for anything that has made MCTruth
     */
    class GArG4 : public ::art::EDProducer{
    public:
      
      /// Standard constructor and destructor for an FMWK module.
      explicit GArG4(fhicl::ParameterSet const& pset);
      virtual ~GArG4();
      
      /// The main routine of this module: Fetch the primary particles
      /// from the event, simulate their evolution in the detctor, and
      /// produce the detector response.
      void produce (::art::Event& evt);
      void beginJob();
      void beginRun(::art::Run& run);
      
    private:
      g4b::G4Helper*             fG4Help;             ///< G4 interface object
      garg4::GArAction*          fGArAction;          ///< Geant4 user action to handle GAr energy depositions
      garg4::AuxDetAction*       fAuxDetAction;       ///< Geant4 user action to handle GAr energy depositions
      garg4::ParticleListAction* fParticleListAction; ///< Geant4 user action to particle information.
      fhicl::ParameterSet        fGArActionPSet;      ///< configuration for GArAction
      std::string                fGArVolumeName;      ///< Name of the volume containing gaseous argon
      std::string                fG4PhysListName;     ///< predefined physics list to use if not making a custom one
      std::string                fG4MacroPath;        ///< directory path for Geant4 macro file to be
                                                      ///< executed before main MC processing.
      bool                       fCheckOverlaps;      ///< Whether to use the G4 overlap checker
      bool                       fdumpParticleList;   ///< Whether each event's sim::ParticleList will be displayed.
      int                        fSmartStacking;      ///< Whether to instantiate and use class to
                                                      ///< dictate how tracks are put on stack.
      float                      fMaxStepSize;        ///< maximum argon step size in cm
      std::vector<std::string>   fInputLabels;
      std::vector<std::string>   fKeepParticlesInVolumes; ///<Only write particles that have trajectories through these volumes
      
      /// Configures and returns a particle filter
      std::unique_ptr<PositionInVolumeFilter> CreateParticleVolumeFilter
      (std::set<std::string> const& vol_names) const;
      
    };
    
  } // namespace garg4
  
  namespace garg4 {
    
    //----------------------------------------------------------------------
    // Constructor
    GArG4::GArG4(fhicl::ParameterSet const& pset)
    : fG4Help                (nullptr)
    , fGArAction             (nullptr)
    , fAuxDetAction          (nullptr)
    , fParticleListAction    (nullptr)
    , fGArActionPSet         (pset.get<fhicl::ParameterSet>("GArActionPSet")                         )
    , fG4PhysListName        (pset.get< std::string       >("G4PhysListName",   "garg4::PhysicsList"))
    , fCheckOverlaps         (pset.get< bool              >("CheckOverlaps",    false)               )
    , fdumpParticleList      (pset.get< bool              >("DumpParticleList", false)               )
    , fSmartStacking         (pset.get< int               >("SmartStacking",    0)                   )
    , fMaxStepSize           (pset.get< float             >("MaxStepSize",      0.2)                 )
    , fKeepParticlesInVolumes(pset.get< std::vector< std::string > >("KeepParticlesInVolumes",{}))
    
    {
      // Set the volume for where we will record energy deposition in the GAr
      fGArVolumeName = fGArActionPSet.get< std::string >("GArVolumeName",    "volTPCActive");

      // initialize the GArSimulationParameters singleton
      G4SimulationParameters::CreateInstance(pset.get<fhicl::ParameterSet>("GArSimParsPSet"));

      LOG_DEBUG("GArG4") << "Debug: GArG4()";
      ::art::ServiceHandle<::art::RandomNumberGenerator> rng;
      
      if (pset.has_key("Seed")) {
        throw ::art::Exception(::art::errors::Configuration)
        << "The configuration of GArG4 module has the discontinued 'Seed' parameter.\n"
        "Seeds are now controlled by three parameters: 'GEANTSeed', 'PropagationSeed' and 'RadioSeed'.";
      }
      
      // setup the random number service for Geant4, the "G4Engine" label is a
      // special tag setting up a global engine for use by Geant4/CLHEP;
      // obtain the random seed from NuRandomService,
      // unless overridden in configuration with key "Seed" or "GEANTSeed"
      // same thing for the propagation engine:
      // and again for radio decay
      ::art::ServiceHandle<rndm::NuRandomService>()->createEngine(*this, "G4Engine",       "GEANT",       pset, "GEANTSeed");
      ::art::ServiceHandle<rndm::NuRandomService>()->createEngine(*this, "HepJamesRandom", "propagation", pset, "PropagationSeed");
      ::art::ServiceHandle<rndm::NuRandomService>()->createEngine(*this, "HepJamesRandom", "radio",       pset, "RadioSeed");
      
      //get a list of generators to use, otherwise, we'll end up looking for anything that's
      //made an MCTruth object
      bool useInputLabels = pset.get_if_present< std::vector<std::string> >("InputLabels", fInputLabels);
      if(!useInputLabels) fInputLabels.resize(0);
      
      produces< std::vector<simb::MCParticle>                 >();
      produces< std::vector<sdp::EnergyDeposit>               >();
      produces< std::vector<sdp::AuxDetSimChannel>            >();
      produces< ::art::Assns<simb::MCTruth, simb::MCParticle> >();
      
      // constructor decides if initialized value is a path or an environment variable
      cet::search_path sp("FW_SEARCH_PATH");
      
      sp.find_file(pset.get< std::string >("GeantCommandFile"), fG4MacroPath);
      struct stat sb;
      if (fG4MacroPath.empty() || stat(fG4MacroPath.c_str(), &sb)!=0)
        // failed to resolve the file name
        throw cet::exception("NoG4Macro")
        << "G4 macro file "
        << fG4MacroPath
        << " not found!\n";
      
    }
    
    //----------------------------------------------------------------------
    // Destructor
    GArG4::~GArG4()
    {
      if(fG4Help) delete fG4Help;
    }
    
    //----------------------------------------------------------------------
    void GArG4::beginJob()
    {
      ::art::ServiceHandle<geo::Geometry> geom;
      auto* rng = &*(::art::ServiceHandle<::art::RandomNumberGenerator>());
      
      fG4Help = new g4b::G4Helper(fG4MacroPath, fG4PhysListName);
      if(fCheckOverlaps) fG4Help->SetOverlapCheck(true);
      fG4Help->ConstructDetector(geom->GDMLFile());
      
      // Get the logical volume store and assign material properties
      garg4::MaterialPropertyLoader* MPL = new garg4::MaterialPropertyLoader();
      MPL->GetPropertiesFromServices();
      MPL->UpdateGeometry(G4LogicalVolumeStore::GetInstance());
      
      // Set the step size limits for the gaseous argon volume
      // get the logical volume for the desired volume name
      G4LogicalVolume* logVol = G4LogicalVolumeStore::GetInstance()->GetVolume(fGArVolumeName);

      LOG_DEBUG("GArG4")
      << "setting step limit size to be "
      << fMaxStepSize * CLHEP::cm
      << " in volume "
      << fGArVolumeName
      << " volume has address "
      << logVol;
      
      
      fG4Help->SetVolumeStepLimit(fGArVolumeName, fMaxStepSize * CLHEP::cm);
      
      // Intialize G4 physics and primary generator action
      fG4Help->InitPhysics();
      
      // Use the UserActionManager to handle all the Geant4 user hooks.
      g4b::UserActionManager* uaManager = g4b::UserActionManager::Instance();
      
      // User-action class for accumulating particles and trajectories
      // produced in the detector.
      auto g4SimPars = gar::garg4::G4SimulationParameters::Instance();
      
      fParticleListAction = new garg4::ParticleListAction(g4SimPars->KineticEnergyCut(),
                                                          g4SimPars->StoreTrajectories(),
                                                          g4SimPars->KeepEMShowerDaughters());
      
      uaManager->AddAndAdoptAction(fParticleListAction);
      
      
      // add UserAction for handling steps in gaseous argon
      fGArAction = new garg4::GArAction(&rng->getEngine("propagation"),
                                        fGArActionPSet);
      uaManager->AddAndAdoptAction(fGArAction);

      // add UserAction for handling steps in auxiliary detectors
      fAuxDetAction = new garg4::AuxDetAction(&rng->getEngine("propagation"),
                                              fGArActionPSet);
      uaManager->AddAndAdoptAction(fAuxDetAction);

      
      // UserActionManager is now configured so continue G4 initialization
      fG4Help->SetUserAction();
      
      return;
    }

    //--------------------------------------------------------------------------
    void GArG4::beginRun(::art::Run& run)
    {
      // prepare the filter object (null if no filtering)
      
      std::set<std::string> volnameset(fKeepParticlesInVolumes.begin(), fKeepParticlesInVolumes.end());
      fParticleListAction->ParticleFilter(CreateParticleVolumeFilter(volnameset));

      return;
    }
    
    //--------------------------------------------------------------------------
    std::unique_ptr<PositionInVolumeFilter> GArG4::CreateParticleVolumeFilter(std::set<std::string> const& vol_names) const
    {
      
      // if we don't have favourite volumes, don't even bother creating a filter
      if (vol_names.empty()) return {};
      
      auto const& geom = *::art::ServiceHandle<geo::Geometry>();
      
      std::vector<std::vector<TGeoNode const*>> node_paths = geom.FindAllVolumePaths(vol_names);
      
      // collection of interesting volumes
      PositionInVolumeFilter::AllVolumeInfo_t GeoVolumePairs;
      GeoVolumePairs.reserve(node_paths.size()); // because we are obsessed
      
      //for each interesting volume, follow the node path and collect
      //total rotations and translations
      for(auto const& path : node_paths){
        
        TGeoTranslation* pTransl = new TGeoTranslation(0.,0.,0.);
        TGeoRotation* pRot = new TGeoRotation();
        for (TGeoNode const* node: path) {
          TGeoTranslation thistranslate(*node->GetMatrix());
          TGeoRotation thisrotate(*node->GetMatrix());
          pTransl->Add(&thistranslate);
          *pRot=*pRot * thisrotate;
        }
        
        //for some reason, pRot and pTransl don't have tr and rot bits set correctly
        //make new translations and rotations so bits are set correctly
        TGeoTranslation* pTransl2 = new TGeoTranslation(pTransl->GetTranslation()[0],
                                                        pTransl->GetTranslation()[1],
                                                        pTransl->GetTranslation()[2]);
        double phi=0.,theta=0.,psi=0.;
        pRot->GetAngles(phi,theta,psi);
        TGeoRotation* pRot2 = new TGeoRotation();
        pRot2->SetAngles(phi,theta,psi);
        
        TGeoCombiTrans* pTransf = new TGeoCombiTrans(*pTransl2,*pRot2);
        
        GeoVolumePairs.emplace_back(path.back()->GetVolume(), pTransf);
        
      }
      
      return std::make_unique<PositionInVolumeFilter>(std::move(GeoVolumePairs));
      
    } // CreateParticleVolumeFilter()
    
    
    //--------------------------------------------------------------------------
    void GArG4::produce(::art::Event& evt)
    {
      LOG_DEBUG("GArG4") << "produce()";
      
      // loop over the lists and put the particles and voxels into the event as collections
      std::unique_ptr< std::vector<simb::MCParticle> >                 partCol(new std::vector<simb::MCParticle>                );
      std::unique_ptr< std::vector<sdp::EnergyDeposit>  >              edCol  (new std::vector<sdp::EnergyDeposit>              );
      std::unique_ptr< ::art::Assns<simb::MCTruth, simb::MCParticle> > tpassn (new ::art::Assns<simb::MCTruth, simb::MCParticle>);
      std::unique_ptr< std::vector< sdp::AuxDetSimChannel > >          adCol  (new std::vector<sdp::AuxDetSimChannel>           );
      
      // reset the track ID offset as we have a new collection of interactions
      fParticleListAction->ResetTrackIDOffset();
      
      // look to see if there is any MCTruth information for this
      // event
      std::vector< ::art::Handle< std::vector<simb::MCTruth> > > mclists;
      if(fInputLabels.size() < 1)
        evt.getManyByType(mclists);
      else{
        mclists.resize(fInputLabels.size());
        for(size_t i = 0; i < fInputLabels.size(); ++i)
          evt.getByLabel(fInputLabels[i], mclists[i]);
      }

      // create a vector of art::Ptrs to MCTruths for use with the
      // association creation and a vector of const* for use with
      // G4Helper
      std::vector< ::art::Ptr<simb::MCTruth> > mctPtrs;
      std::vector< const simb::MCTruth*      > mcts;
      
      for(size_t mc = 0; mc < mclists.size(); ++mc){
        for(size_t i = 0; i < mclists[mc]->size(); ++i){
          art::Ptr<simb::MCTruth> ptr(mclists[mc], i);
          mctPtrs.push_back(ptr);
          mcts   .push_back(ptr.get());
        }
      }
      
      // Process Geant4 simulation for the mctruths
      fG4Help->G4Run(mcts);
      
      // receive the particle list
      auto       particleList = fParticleListAction->YieldList();
      auto const trackIDToMCT = fParticleListAction->TrackIDToMCTruthIndexMap();

      int    trackID             = std::numeric_limits<int>::max();
      size_t mctidx              = 0;
      size_t nGeneratedParticles = 0;
      
      // Has the user request a detailed dump of the output objects?
      if (fdumpParticleList){
        LOG_INFO("GArG4")
        << "Dump sim::ParticleList; size() = "
        << particleList.size()
        << "\n"
        << particleList;
      }
      
      auto iPartPair = particleList.begin();
      while (iPartPair != particleList.end()) {
        simb::MCParticle& p = *(iPartPair->second);
     
        trackID = p.TrackId();
        
        partCol->push_back(std::move(p));
        if( trackIDToMCT.count(trackID) > 0){
          
          mctidx = trackIDToMCT.find(trackID)->second;

          util::CreateAssn(*this,
                           evt,
                           *partCol,
                           mctPtrs[mctidx],
                           *tpassn,
                           nGeneratedParticles);
        }
        else
          throw cet::exception("GArG4")
          << "Cannot find MCTruth for Track Id: "
          << trackID
          << " to create association between Particle and MCTruth";

        // FIXME workaround until https://cdcvs.fnal.gov/redmine/issues/12067
        // is solved and adopted in LArSoft, after which moving will suffice
        // to avoid dramatic memory usage spikes;
        // for now, we immediately disposed of used particles
        iPartPair = particleList.erase(iPartPair);
        
        ++nGeneratedParticles;
      } // while(particleList)
      
      
      // Now for the sdp::EnergyDepositions
      for(auto const& ed : fGArAction->EnergyDeposits()){
        LOG_DEBUG("GArG4")
        << "adding deposits for track id: "
        << ed.TrackID();
        
        edCol->emplace_back(ed);
      }
      
      // And finally the AuxDetSimChannels
      for(auto const& ad : fAuxDetAction->AuxDetSimChannels()) adCol->emplace_back(ad);
      
      evt.put(std::move(edCol));
      evt.put(std::move(adCol));
      evt.put(std::move(partCol));
      evt.put(std::move(tpassn));
      
      return;
    } // GArG4::produce()
    
  } // namespace garg4
  
  namespace garg4 {
    
    DEFINE_ART_MODULE(GArG4)
    
  } // namespace garg4
} // gar
#endif // GARG4GARG4H
