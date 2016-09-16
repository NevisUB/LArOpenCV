//by vic
#ifndef __BOUNDRECTSTART_CXX__
#define __BOUNDRECTSTART_CXX__

#include "BoundRectStart.h"
#include "PCAUtilities.h"
#include "StatUtilities.h"

namespace larocv {

  void BoundRectStart::_Configure_(const ::fcllite::PSet &pset)
  {
    _nDivWidth = pset.get<int>("NDivWidth");
    _cutbadreco = pset.get<bool>("CutBadReco");
    _use_roi_vertex= pset.get<bool>("UseROIVertex");
    
    if ( _nDivWidth % 2 != 0 ) { throw larbys("NDivWidth must be an even number"); }
    
  }

  Cluster2DArray_t BoundRectStart::_Process_(const larocv::Cluster2DArray_t& clusters,
					     const ::cv::Mat& img,
					     larocv::ImageMeta& meta, larocv::ROI& roi)
  {

    /*
      Extend on Ariana's track shower module to include segmentation. Segment the box long ways
      calculate the linearity, and total charge in each box. Maximize the product of the two
      then the point farthest from the center is the start point
    */

    Cluster2DArray_t oclusters; oclusters.reserve( clusters.size() );

    for (size_t k = 0; k < clusters.size(); k++) {

      Cluster2D ocluster = clusters[k];

      if ( ocluster._numHits() == 0 ) throw larbys("Somehow this cluster has no hits, why?");

      // use bounding box considerations to define start point, fill the vertices vector
      auto& bbox = ocluster._minAreaBox;

      //set the center points as the center of this box
      ocluster._centerPt  = Point2D(bbox.center.x, bbox.center.y);

      // handles to few variables
      auto& center  = ocluster._centerPt;
      auto& angle   = ocluster._angle2D;

      auto v = ocluster._minAreaRect;
      
      std::vector<::cv::RotatedRect> divisions;
      divisions.resize(_nDivWidth);

      double angle_deg = bbox.angle;
      double height ( bbox.size.height ), width( bbox.size.width );

      bool swapped = false;

      if (bbox.size.width < bbox.size.height) {
        angle_deg += 90.;
        width  = bbox.size.height;
        height = bbox.size.width;
        swapped = true;
      }

      double N = _nDivWidth;

      // divide the box up
      double w_div = width  / (double) N;
      //double h_div = height / (double) N;

      // what is the step
      auto dx = w_div * std::cos(angle_deg * _deg2rad);
      auto dy = w_div * std::sin(angle_deg * _deg2rad);

      // where do i start
      auto cx = center.x - (N / 2 - 0.5) * dx;
      auto cy = center.y - (N / 2 - 0.5) * dy;

      std::vector<std::vector<::cv::Point2f> > vvv;
      vvv.resize(N);

      for (unsigned i = 0; i < N; ++i) {
        auto& vv = vvv[i];

        ::cv::Size2f s = swapped ? ::cv::Size2f(height, w_div) : ::cv::Size2f(w_div, height);

        // step and make rotated rect
        divisions[i] = ::cv::RotatedRect(::cv::Point2f(cx + i * dx, cy + i * dy), s, angle);

        ::cv::Point2f ver[4];
        divisions[i].points(ver);
        v.clear();
	v.resize(4);

        for (int r = 0; r < 4; ++r)
	  v[r] = ver[r];
	
        std::swap(vv, v);
      }


      // do something with the segments...
      std::vector<Contour_t> insides;
      insides.resize(N);

      std::vector<double> tot_charge;
      tot_charge.resize(N);


      auto const & hits = ocluster._insideHits ;

      bool placed=false;
      // hits in the cluster...
      for (auto& h : hits) {

	placed=false;

        // which ones are in this segment
        for ( unsigned i = 0; i < divisions.size(); ++i ) {
          auto& div = vvv[i];

          if ( ::cv::pointPolygonTest(div, h, false) >= 0 ) {
            tot_charge[i] += (int) img.at<uchar>(h.y, h.x);
            insides[i].push_back(h);
	    placed=true;
          }

        }

	if ( placed ) continue;
	// wasn't in either segment? why? assigned hit to segment which is closest to center
	float min_dist = 1e8;
	size_t min = 0;
	
	for ( unsigned i = 0; i < divisions.size(); ++i ) {
	  auto center = divisions[i].center;
	  
	  auto dist = std::sqrt( std::pow(center.x - h.x, 2) + std::pow(center.y - h.y, 2) );

	  if ( dist < min_dist ) {
	    min=i;
	    min_dist=dist;
	  }
	}	  
	insides[min].push_back(h);
      }

      //which side of the bounding rectangle is it on? f_half
      int cstart,cend;
      
      if( _use_roi_vertex ) {

	auto pi0st = roi.roivtx_in_image(meta);
	
	int min_hit_index = -1;
	float min_dist=1e8;
	
	for (int i = 0; i < hits.size(); i++) {

	  auto const& hit = hits[i];
	  
	  auto dist = std::sqrt( std::pow(pi0st.x - hit.x, 2) + std::pow(pi0st.y - hit.y, 2) );
	  
	  if ( dist < min_dist ) {
	    min_dist = dist;
	    min_hit_index = i;
	  }

	}
	
	unsigned j = 0;
	bool found = false;
	
	for ( ; j < insides.size(); ++j )  {

	  for( const auto& hd : insides[j] ) {

	    if ( hd == hits[ min_hit_index ] )

	      { found = true; break; }
	  }

	  if ( found ) break;
	  
	}
      
	if ( N != 2 ) throw larbys("N != 2 in BoundRectStart\n");
	if ( j == N ) {
	  std::cout  << " If you are seeing this message then there are hits that lay on the outside of divisions\n";
	  std::cout  << " which could mean that there is a hit that failed to get associated with a segment. Take a look\n";
	  std::cout  << " min_hit_index: " << min_hit_index << " \n";
	  std::cout  << " hits[min_hit_index]: " << hits[min_hit_index] << "\n";
	  std::cout  << " min dist : " << min_dist << "\n";
	  std::cout  << " vertex : " << pi0st.x << "," << pi0st.y << "\n";
	  std::cout  << " insides size :" << insides[0].size() << " and " << insides[1].size() << "\n";
	  std::cout  << " hits.size() : " << hits.size() << "\n";
	  throw larbys("Find me in BoundRectStart");
	}
      
	cstart = j;
	cend   = cstart > 0 ? 0 : 1;
	
      }
      else { // NO ROI vertex, lets give it a shot
	
	double f_half(0), s_half(0);
	
	// total charge half
	auto tc_half = tot_charge.size() / 2;
	
	//first half total charge
	auto f_total_charge = std::accumulate(std::begin(tot_charge),std::begin(tot_charge) + tc_half ,0);
	
	//second hald total charge
	auto s_total_charge = std::accumulate(std::begin(tot_charge) + tc_half,std::end(tot_charge),0);
	
	for (int i = 0; i < N; ++i) {
	  auto roicov = std::abs(roi_cov(insides[i]));

	  if ( i < N / 2 ) f_half += roicov * tot_charge[i];
	  else             s_half += roicov * tot_charge[i];
	}
      
	// this is charge weighting for the pearsons r
	f_half /= f_total_charge;
	s_half /= s_total_charge;
	
	cstart = f_half > s_half ? 0     : N - 1;
	cend   = f_half > s_half ? N - 1 : 0;
      }	


      //get the farthest point from the center as start point
      ::cv::Point* f_start;
      double far = 0;
      for ( auto& h : insides[cstart] ) {
	auto d = dist(h.x, center.x, h.y, center.y);
	if ( d > far ) { far = d;  f_start = &h; }
      }
	
      //no start point found...
      if ( far == 0 && _cutbadreco) continue;
	
      //end point is on the other side
      ::cv::Point* f_end; //farthest end point
      far = 0;
      for ( auto& h : insides[cend] ) {
	auto d = dist(h.x, center.x, h.y, center.y);
	if ( d > far ) { far = d;  f_end = &h; }
      }
	
      //no end point found...
      if ( far == 0 && _cutbadreco) continue;

      if(_use_roi_vertex){

	auto& roi   = ocluster.roi;
	roi.startpt = Point2D(f_start->x, f_start->y);
	roi.endpt   = Point2D(f_end->x  , f_end->y  );

      } else {

	auto& reco   = ocluster.reco;
	reco.startpt = Point2D(f_start->x, f_start->y);
	reco.endpt   = Point2D(f_end->x  , f_end->y  );
	
      }
      
      oclusters.emplace_back(std::move(ocluster));
      
    }//end loop over clusters
    
    //all is said and done
    return oclusters;
  }
  
}
#endif
