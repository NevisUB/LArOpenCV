#ifndef __ROIASSISTEDSTART_CXX__
#define __ROIASSISTEDSTART_CXX__

#include "ROIAssistedStart.h"
#include "PCAUtilities.h"
#include "StatUtilities.h"

namespace larocv{

  void ROIAssistedStart::_Configure_(const ::fcllite::PSet &pset)
  {
    _padx = pset.get<float>("Padx");
    _pady = pset.get<float>("Pady");
  }

  Cluster2DArray_t ROIAssistedStart::_Process_(const larocv::Cluster2DArray_t& clusters,
					       const ::cv::Mat& img,
					       larocv::ImageMeta& meta)
  {


    // Just like Ariana's ROIStart but this time we let the ROI choose the side of the
    // cluster the start point is probably on rather than the aboslute coordinate of the start point.
    // The start point and end point will be chosen as the points that are farthest away from the center
    // point. We could do PCA I bet and and walk inward and find the point that is closest to the PCA
    // axis in an future larbys extension to this module

    //copy and paste from ROIStart

    //the ROI vertex
    auto pi0_st = meta.roivtx();
    if ( pi0_st.x == ::larocv::kINVALID_DOUBLE ) { std::cout << "BAD VTX\n"; throw std::exception(); }

    auto pi0st = Point2D( (pi0_st.y - meta.origin().y) / meta.pixel_height(), (pi0_st.x - meta.origin().x) / meta.pixel_width() );

    Cluster2DArray_t oclusters;
    oclusters.reserve( clusters.size() );
    
    for (size_t k = 0; k < clusters.size(); k++) {

      Cluster2D ocluster = clusters[k]; // make copy 
      auto& contour      = ocluster._contour;

      //copy from BoundRecStart.cxx
      // use bounding box considerations to define start point, fill the vertices vector
      auto bbox = ::cv::minAreaRect(contour);
      ocluster._boundingBox = bbox.boundingRect();

      ::cv::Point2f verticies[4];
      bbox.points(verticies);

      // turn verticies vector into std::vector
      std::vector<::cv::Point2f> v; v.resize(4);
      for (int r = 0; r < 4; ++r) v[r] = verticies[r];

      //give ocluster the minimum area rectangle
      std::swap(v, ocluster._minAreaRect);

      //set the center points as the center of this box
      ocluster._centerPt  = Point2D(bbox.center.x, bbox.center.y);

      // handles to few variables
      auto& center  = ocluster._centerPt;
      auto& angle   = bbox.angle;

      double N = 2;
      
      std::vector<::cv::RotatedRect> divisions;
      divisions.resize(N);

      double angle_deg = bbox.angle;
      double height ( bbox.size.height ), width( bbox.size.width );

      bool swapped = false;

      if (bbox.size.width < bbox.size.height) {
        angle_deg += 90.;
        width  = bbox.size.height;
        height = bbox.size.width;
        swapped = true;
      }


      // divide the box up
      double w_div = width  / (double) N;
      //double h_div = height / (double) N;

      // what is the step?
      auto dx = w_div * std::cos(angle_deg * _deg2rad);
      auto dy = w_div * std::sin(angle_deg * _deg2rad);

      // where do i start?
      auto cx = center.x - (N / 2 - 0.5) * dx;
      auto cy = center.y - (N / 2 - 0.5) * dy;

      auto& vvv = ocluster._verts;
      vvv.clear(); vvv.resize(N);
      
      for (unsigned i = 0; i < N; ++i) {
        auto& vv = vvv[i];

        ::cv::Size2f s = swapped ? ::cv::Size2f(height+_padx, w_div+_pady) : ::cv::Size2f(w_div+_padx, height+_pady);

        // step and make rotated rect
        divisions[i] = ::cv::RotatedRect(::cv::Point2f(cx + i * dx, cy + i * dy), s, angle);

        ::cv::Point2f ver[4];
        divisions[i].points(ver);
        v.clear(); v.resize(4);
        for (int r = 0; r < 4; ++r) v[r] = ver[r];
        std::swap(vv, v);
	
      }

      // length N vector that will hold the hits on either side
      std::vector<Contour_t> insides;
      insides.resize(N);
      
      // from ROIStart
      float min_dist = std::pow(10,8);
      int min_hit_index = -1 ;
     
      // hits in the cluster...
      auto const & hits = ocluster._insideHits ;
      
      for (int i = 0; i < hits.size(); i++) {

	auto & hit = hits[i];
	
	auto dist = std::sqrt( std::pow(pi0st.x - hit.x, 2) + std::pow(pi0st.y - hit.y, 2) );

        if ( dist < min_dist ) {
          min_dist = dist;
	  min_hit_index = i;
        }
	
        //which ones are in this segment
	int count = 0;
        for ( unsigned j = 0; j < divisions.size(); ++j ) {

          auto& div = ocluster._verts[j];

          if ( ::cv::pointPolygonTest(div, hit, false) >= 0 ) 
	    
            { insides[j].emplace_back(hit); ++count; }
	  
	}

      }
            
      
      //which side of the bounding rectangle is it on? f_half
      unsigned j = 0;
      bool found = false;
      for ( ; j < insides.size(); ++j )  {

	for( const auto& hd : insides[j] ) {

	  if ( hd == hits[ min_hit_index ] )

	    { found = true; break; }
	}

	if ( found ) break;
      }
      
      if ( N != 2 ) throw std::exception();
      if ( j == N )
	{
	  std::cout << " If you are seeing this message then there are hits that lay on the outside of divisions\n";
	  std::cout << " which could mean that there is a hit that failed to get associated with a segment. Take a look";
	  std::cout << " min_hit_index: " << min_hit_index << " \n";
	  std::cout << " hits[min_hit_index]" << hits[min_hit_index] << "\n";
	  std::cout << " min dist : " << min_dist << "\n";
	  std::cout << " vertex : " << pi0st.x << "," << pi0st.y << "\n";
	  std::cout << " insides size " << insides[0].size() << " and " << insides[1].size() << "\n";
	  std::cout << " hits.size() : " << hits.size() << "\n";
	  throw std::exception();
	}
      
      int cstart = j;
      int cend   = cstart > 0 ? 0 : 1;
      
      //get the farthest point from the center as start point
      ::cv::Point* f_start;
      double far = 0;
      for ( auto& h : insides[cstart] ) {
        auto d = dist(h.x, center.x, h.y, center.y);
        if ( d > far ) { far = d;  f_start = &h; }
      }
      
      
      //end point is on the other side
      ::cv::Point* f_end; //farthest end point
      far = 0;
      for ( auto& h : insides[cend] ) {
        auto d = dist(h.x, center.x, h.y, center.y);
        if ( d > far ) { far = d;  f_end = &h; }
      }

      auto& roi   = ocluster.roi;
      roi.startpt = Point2D(f_start->x, f_start->y);
      roi.endpt   = Point2D(f_end->x  , f_end->y  );

      oclusters.emplace_back(std::move(ocluster));
      
    } //end loop over clusters

    return oclusters;

  } // end function

}
#endif
