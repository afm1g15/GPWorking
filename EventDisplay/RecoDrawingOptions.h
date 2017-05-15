////////////////////////////////////////////////////////////////////////
// $Id: RecoDrawingOption.h,v 1.15 2010/08/30 21:33:24 spitz7 Exp $
//
// Display parameters for the raw data
//
// \author brebel@fnal.gov
////////////////////////////////////////////////////////////////////////
#ifndef RECODRAWINGOPTIONS_H
#define RECODRAWINGOPTIONS_H
#ifndef __CINT__
#include <string>
#include <vector>

#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"

namespace gar {
namespace evd {
  
class RecoDrawingOptions
{
public:
    RecoDrawingOptions(fhicl::ParameterSet const& pset, art::ActivityRegistry& reg);
    ~RecoDrawingOptions();
    
    void reconfigure(fhicl::ParameterSet const& pset);

    int  fDrawHits;
    int  fDrawTracks;
    int  fDrawTrackTrajectoryPoints;
    int  fDrawShowers;
    int  fDrawVertices;
    int  fDrawEvents;
  
    std::vector<std::string> fHitLabels;     		        ///< module labels that produced hits
    std::vector<std::string> fTrackLabels;   		        ///< module labels that produced tracks
    std::vector<std::string> fShowerLabels;  		        ///< module labels that produced showers
    std::vector<std::string> fVertexLabels;  		        ///< module labels that produced vertices
    std::vector<std::string> fEventLabels;          		///< module labels that produced events
    int                      fColorProngsByLabel;       ///< Generate prong colors by label or id?
    int                      fColorSpacePointsByChisq;  ///< Generate space point colors by chisquare?

  };
}
}//namespace
#endif // __CINT__
DECLARE_ART_SERVICE(gar::evd::RecoDrawingOptions, LEGACY)
#endif

