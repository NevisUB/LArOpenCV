#ifndef __EMPTYANALYSIS_CXX__
#define __EMPTYANALYSIS_CXX__

#include "EmptyAnalysis.h"

namespace larocv {
  
  static EmptyAnalysisFactory __global_EmptyAnalysisFactory__;
  
  void EmptyAnalysis::_Configure_(const Config_t &pset)
  {
    _tree = new TTree("EmptyAnalysis","");
    AttachIDs(_tree);
    _tree->Branch("roid" , &_roid  , "roid/I");
    _tree->Branch("vtxid", &_vtxid , "vtxid/I");
    
    _roid = 0;
  }

  void EmptyAnalysis::_Process_() {
    
    if(NextEvent()) _roid=0;

    auto& ass_man = AssManager();
    
    // Get images
    auto adc_img_v = ImageArray(ImageSetID_t::kImageSetWire);
    const auto& meta_v = MetaArray();
    
    _vtxid = -1;

    size_t nvertex=0;
    for(size_t vtxid = 0; vtxid < nvertex; ++vtxid) {
      _vtxid += 1;

      //
      // Do something
      //
      
      _tree->Fill();
    } // end this vertex
    
    _roid += 1;
  }
}
#endif
