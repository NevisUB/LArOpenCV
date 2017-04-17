#ifndef __SHOWERPLANESEEDS_CXX__
#define __SHOWERPLANESEEDS_CXX__

#include "ShowerPlaneSeeds.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoData/VertexSeed.h"

namespace larocv {

  /// Global larocv::ShowerPlaneSeedsFactory to register AlgoFactory
  static ShowerPlaneSeedsFactory __global_ShowerPlaneSeedsFactory__;
  
  void ShowerPlaneSeeds::_Configure_(const Config_t &pset)
  {

    _threshold = pset.get<float>("Threshold");
    // Set algo verbosity
    _OneTrackOneShower.set_verbosity(this->logger().level());
    // Get algo config
    _OneTrackOneShower.Configure(pset.get<Config_t>("OneTrackOneShower"));
    // Prep output data rep
    for(short plane=0;plane<3;++plane)
      Register(new data::VertexSeed2DArray);
  }
  
  void ShowerPlaneSeeds::_Process_()
  {

    //
    // 0) Prep
    //
    
    // Get track/shower image
    auto track_img_v   = ImageArray(ImageSetID_t::kImageSetTrack);
    auto shower_img_v  = ImageArray(ImageSetID_t::kImageSetShower);

    // Make sure they are same sizes
    assert(track_img_v.size() == shower_img_v.size());
    // Get track image count as # planes
    auto nplanes = track_img_v.size();
    // Set plane-wise info for algorithm via meta
    for(auto const& meta : MetaArray())
      _OneTrackOneShower.SetPlaneInfo(meta);

    //
    // 1) Loop over image and process
    //

    for(size_t img_idx=0; img_idx<nplanes; ++img_idx) {

      // Get data rep to be filled per plane
      auto& vertexseed2darray = AlgoData<data::VertexSeed2DArray>(img_idx);

      // Track/shower image on this plane
      auto& track_img  = track_img_v  [img_idx];
      auto& shower_img = shower_img_v [img_idx];

      // 2D vertex seed list to be filled 
      std::vector<data::VertexSeed2D> vertexseed_v;

      LAROCV_DEBUG() << "No input seed given, determine track edges" << std::endl;
      auto thresh_img = Threshold(track_img,_threshold,255); // threshold
      auto ctor_v     = FindContours(thresh_img); // and find contours
      LAROCV_DEBUG() << "Found " << ctor_v.size() << " contours in track img (thresh:"<<_threshold<<")"<<std::endl;
      // Loop over contours and find 2 edge points
      // they are 2D track edge points == shower/track vertex candidate (to be filled in vertexseed_v)
      for(const auto& ctor : ctor_v) {
	geo2d::Vector<float> edge1(kINVALID_FLOAT,kINVALID_FLOAT);
	auto edge2 = edge1;
	// use furthest-point algorithm (btw it only works on track)
	FindEdges(ctor,edge1,edge2);

	// register edge 1 to a 2D seed container if valid
	if (edge1.x!=kINVALID_FLOAT && edge1.y!=kINVALID_FLOAT) {
	  LAROCV_DEBUG() << "Edge identified @ " << edge1 << std::endl;
	  vertexseed_v.emplace_back(std::move(edge1));
	}

	// register edge 2 to a 2D seed container if valid	  
	if (edge2.x!=kINVALID_FLOAT && edge2.y!=kINVALID_FLOAT) {
	  LAROCV_DEBUG() << "Edge identified @ " << edge2 << std::endl;
	  vertexseed_v.emplace_back(std::move(edge2));
	}
	  
      }
      LAROCV_DEBUG() << "Saved " << vertexseed_v.size() << " 2D seeds from track edge points" << std::endl;
      LAROCV_DEBUG() << "Track input seeds given, filtering on shower image" << std::endl;
      
      uint ix=0;

      // Loop over found track-edge 2D seed point, see if shower's coming out of it (if yes, then vtx candidate)
      for(size_t seed_idx=0; seed_idx<vertexseed_v.size(); ++seed_idx) {

	// Get seed: writeable ref to modify, then possibly std::move later into output data rep
	auto& seed = vertexseed_v[seed_idx];
	data::CircleVertex cvtx;
	cvtx.center.x = seed.x;
	cvtx.center.y = seed.y;
	cvtx.radius   = _OneTrackOneShower.circle_default_size();
	seed.radius   = cvtx.radius;

	// Inspect the possibility of being shower/track vtx point
	_OneTrackOneShower.ValidateCircleVertex(shower_img,cvtx);
	LAROCV_DEBUG() << ix << ") @ (" << seed.x << "," << seed.y << ") xs ..." << cvtx.xs_v.size() << std::endl;
	ix++;

	// To be track/shower vtx require only 1 xs point, else ignore
	if (cvtx.xs_v.size()!=1) continue;
	
	LAROCV_DEBUG() << "... saved" << std::endl;

	// Reaching here means condition satisfied: move to output data rep
	vertexseed2darray.emplace_back(std::move(seed));
      }// end seed filter
    LAROCV_DEBUG() << "Determined " << vertexseed2darray.as_vector().size() << " vertex seeds" << std::endl;
    }// end plane


  }
}
#endif
