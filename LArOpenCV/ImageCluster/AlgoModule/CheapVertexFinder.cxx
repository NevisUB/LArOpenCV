#ifndef __CHEAPVERTEXFINDER_CXX__
#define __CHEAPVERTEXFINDER_CXX__

#include "CheapVertexFinder.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"

namespace larocv {

  /// Global larocv::CheapVertexFinderFactory to register AlgoFactory
  static CheapVertexFinderFactory __global_CheapVertexFinderFactory__;
  
  void CheapVertexFinder::_Configure_(const Config_t &pset)
  {
  }

  bool CheapVertexFinder::_PostProcess_() const
  { return true; }

  void CheapVertexFinder::_Process_() {

    //
    // Get the track and shower image
    //
    std::vector<cv::Mat> shower_img_v = ImageArray(ImageSetID_t::kImageSetShower);
    std::vector<cv::Mat> track_img_v  = ImageArray(ImageSetID_t::kImageSetTrack);

    //
    // resize _closest_pt vector to size=3 (1 point per plane)
    // and fill it with (1,1)
    //
    _closest_pt_v.resize(3);
    for(cv::Point& v : _closest_pt_v) v = cv::Point(1,1);
    
    // __Goal__ : Find the 2D vertex point (the interface between the track and shower image)
    // __Input__: Track (a line) and shower (a triangle) images
    // __Task__ : Can you find the 2D point there the fake proton and electron are produced in this image?
    //            Once you _find_ the 2D point, fill it in _closest_pt_v cv::Point vector so we can check it in python!
    //            I prefilled the _closest_pt_v vector with the point (1,1) on each plane, head over
    //            to LArCV/app/LArOpenCVHandle/example/example_vertex.py to test the solution!
    //            Your star should be put in the "obvious" vertex point.


    //
    // Begin your code
    //


    //
    // End your code
    //
    return;
    
  }
}
#endif


/*

  Vic's solution below don't cheat!!!!!!
  Vic's solution below don't cheat!!!!!!
  Vic's solution below don't cheat!!!!!!
  Vic's solution below don't cheat!!!!!!
  Vic's solution below don't cheat!!!!!!
  Vic's solution below don't cheat!!!!!!
  Vic's solution below don't cheat!!!!!!
  Vic's solution below don't cheat!!!!!!
  Vic's solution below don't cheat!!!!!!
  Vic's solution below don't cheat!!!!!!
  Vic's solution below don't cheat!!!!!!
  Vic's solution below don't cheat!!!!!!
  Vic's solution below don't cheat!!!!!!
  Vic's solution below don't cheat!!!!!!
  Vic's solution below don't cheat!!!!!!

 */

/*

    
    for(size_t plane = 0; plane<3; ++plane) {

      auto& track_img  = track_img_v.at(plane);
      auto& shower_img = shower_img_v.at(plane);
      
      auto trk_ctor_v = FindContours(track_img);
      auto shr_ctor_v = FindContours(shower_img);

      float dmin = kINVALID_FLOAT;
      cv::Point ptmin;
      
      for(const auto& trk_ctor : trk_ctor_v) {
	for(const auto& shr_ctor : shr_ctor_v) {
	  for(auto trk_pt : trk_ctor) {
	    for(auto shr_pt : shr_ctor) {

	      float d;
	      d  = pow((float)trk_pt.x - (float)shr_pt.x,2);
	      d += pow((float)trk_pt.y - (float)shr_pt.y,2);
	      d  = sqrt(d);

	      if (d < dmin) {
		dmin = d;
		ptmin = trk_pt;
	      }
	      
	    } // end shower point contour
	  } // end track point on contour
	  
	} // end shower contour
      } // end track contour

      _closest_pt_v[plane] = ptmin;
      
    } // end plane
    

*/
