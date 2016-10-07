//
//  Shower.cpp
//  garsoft-mrb
//
//  Created by Brian Rebel on 10/6/16.
//  Copyright © 2016 Brian Rebel. All rights reserved.
//

#include "ReconstructionDataProducts/Shower.h"

namespace gar {
  namespace rec {
    
    //--------------------------------------------------------------------------
    Shower::Shower()
    {
      return;
    }
    
    //--------------------------------------------------------------------------
    Shower::Shower(float  energy,
                   float *vtx,
                   float *vtxDir)
    : fEnergy  (energy  )
    {
      fVertex[0] = vtx[0];
      fVertex[1] = vtx[1];
      fVertex[2] = vtx[2];
      
      fVtxDir[0] = vtxDir[0];
      fVtxDir[1] = vtxDir[1];
      fVtxDir[2] = vtxDir[2];
      
      return;
    }
    
  } // rec
} // gar
