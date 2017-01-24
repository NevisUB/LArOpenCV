#ifndef __DEFECTCLUSTER_CXX__
#define __DEFECTCLUSTER_CXX__

#include "DefectCluster.h"
#include "Core/Geo2D.h"
#include "VertexTrackCluster.h"
#include "LinearTrackCluster.h"

namespace larocv {

  /// Global larocv::DefectClusterFactory to register AlgoFactory
  static DefectClusterFactory __global_DefectClusterFactory__;

  void DefectCluster::_Configure_(const ::fcllite::PSet &pset)
  {
    _min_defect_size      = pset.get<int>("MinDefectSize",5);
    _hull_edge_pts_split  = pset.get<int>("NHullEdgePoints",50);

    auto const vertextrack_algo_name = pset.get<std::string>("VertexTrackClusterAlgo","");
    _vertextrack_algo_id = kINVALID_ALGO_ID;
    if(!vertextrack_algo_name.empty()) {
      LAROCV_INFO() << "Using vertex track cluster algo: " << vertextrack_algo_name << std::endl;
      _vertextrack_algo_id = this->ID(vertextrack_algo_name);
    }else{
      LAROCV_INFO() << "Not using vertex track cluster algo..." << std::endl;
    }

    auto const lineartrack_algo_name = pset.get<std::string>("LinearTrackClusterAlgo","");
    _lineartrack_algo_id = kINVALID_ALGO_ID;
    if(!lineartrack_algo_name.empty()) {
      LAROCV_INFO() << "Using linear track cluster algo: " << lineartrack_algo_name << std::endl;
      _lineartrack_algo_id = this->ID(lineartrack_algo_name);
    }else{
      LAROCV_INFO() << "Not using linear track cluster algo..." << std::endl;
    }

    Register(new data::DefectClusterData);
  }

  larocv::Cluster2DArray_t DefectCluster::_Process_(const larocv::Cluster2DArray_t& clusters,
						    const ::cv::Mat& img,
						    larocv::ImageMeta& meta,
						    larocv::ROI& roi)
  {
    if(this->ID()==0) throw larbys("DefectCluster should not be run 1st!");
    auto& data          = AlgoData<data::DefectClusterData>(0);
    auto& plane_data    = data._raw_cluster_vv[meta.plane()];

    // process vertex associated particle clusters if provided, and if not yet processed
    if( _vertextrack_algo_id!=kINVALID_ALGO_ID && data.get_vertex_clusters().empty()) {

      auto const& vtxtrack_data = AlgoData<data::VertexClusterArray>(_vertextrack_algo_id,0);

      // particle vertex cluster array
      const auto& vtx_cluster_v = vtxtrack_data._vtx_cluster_v;
      
      // loop over vtx
      for(auto const& vtx_cluster : vtx_cluster_v) {
	data::ParticleCompoundArray pcompound_set;
	// loop over plane
	for(size_t plane = 0 ; plane < vtx_cluster.num_planes(); ++plane) {
	  // loop over clusters on this plane
	  for(auto const& pcluster : vtx_cluster.get_clusters(plane)) {
	    LAROCV_INFO() << "Inspecting defects for Vertex " << vtx_cluster.id()
			  << " plane " << plane
			  << " particle " << pcluster.id()
			  << std::endl;
	    auto pcompound = BreakContour(pcluster._ctor);
	    pcompound_set.emplace_back(plane,std::move(pcompound));
	  }
	}
	// record
	data.move(vtx_cluster.id(),std::move(pcompound_set));
      }
      LAROCV_INFO() << "Finished processing all vertex (result size = "
		    << data.num_vertex_clusters() << " vertex clusters)" << std::endl;
    }
    else if( _lineartrack_algo_id!=kINVALID_ALGO_ID && data.get_vertex_clusters().empty()) {
      
      auto const& lintrack_data = AlgoData<data::LinearTrackArray>(_lineartrack_algo_id,0);

      // particle vertex cluster array
      const auto& lin_cluster_v = lintrack_data.get_clusters();
      
      // loop over vtx
      for(auto const& lin_cluster : lin_cluster_v) {
	data::ParticleCompoundArray pcompound_set;
	// loop over plane
	for(size_t plane = 0 ; plane < 3; ++plane) {
	  // loop over clusters on this plane
	  auto& lcluster = lin_cluster.get_cluster(plane);
	  if ( lcluster.ctor.empty() ) continue;		   
	  LAROCV_INFO() << "Inspecting defects for LinearTrack " << lin_cluster.id()
			<< " plane " << plane
			<< std::endl;
	  auto pcompound = BreakContour(lcluster.ctor);
	  pcompound_set.emplace_back(plane,std::move(pcompound));
	}
	// record
	data.move(lin_cluster.id(),std::move(pcompound_set));
      }
      LAROCV_INFO() << "Finished processing all vertex (result size = "
		    << data.num_vertex_clusters() << " vertex clusters)" << std::endl;
      
    
    }else{

      // Process input clusters on this plane
      for(size_t cindex=0; cindex<clusters.size(); ++cindex) {
	auto const& cluster = clusters[cindex];
	
	LAROCV_INFO() << "Inspecting defects plane " << meta.plane()
		      << " cluster " << cindex
		      << std::endl;
	
	auto cluscomp = BreakContour(cluster._contour);
	
	plane_data.emplace_back(std::move(cluscomp));
	
      }
    }
    
    // Construct output
    Cluster2DArray_t oclusters_v;
    for(auto& cluscomp : plane_data.get_cluster()) {
      for(auto& atomic : cluscomp.get_atoms()) {
	Cluster2D ocluster;
	ocluster._contour = atomic._ctor;
	oclusters_v.emplace_back(std::move(ocluster));
      }
    }
    return oclusters_v;
  }

  bool DefectCluster::on_line(const geo2d::Line<float>& line,::cv::Point pt) { 

    float eps = 0.9;

    float xpos = pt.x;
    float ypos = pt.y;
    
    if ((ypos < line.y(xpos) + eps) and
	(ypos > line.y(xpos) - eps))
      return true;
    
    if ((ypos < line.y(xpos + eps)) and
    	(ypos > line.y(xpos - eps)))
    return true;
    
    return false;
    
  }


  void DefectCluster::fill_hull_and_defects(const GEO2D_Contour_t& ctor,
					    std::vector<int>& hullpts,
					    std::vector<cv::Vec4i>& defects,
					    std::vector<float>& defects_d) {
    
    //Make this hull
    ::cv::convexHull(ctor, hullpts);
       
    //Close this hull up
    hullpts.push_back(hullpts.at(0));

    //Compute the defects
    ::cv::convexityDefects(ctor,hullpts,defects);
    
    //Put the defects distances specially into vector of doubles
    defects_d.resize( defects.size() );
       
    for( int j = 0; j < defects.size(); ++j )
	defects_d[j]  = ( ( (float) defects[j][3] ) / 256.0 );

  }
  void DefectCluster::filter_defects(std::vector<cv::Vec4i>& defects,
			    std::vector<float>& defects_d,
			    float min_defect_size){

    std::vector<cv::Vec4i> defects_tmp;
    defects_tmp.reserve(defects.size());

    std::vector<float> defects_d_tmp;
    defects_d_tmp.reserve(defects.size());

    for(unsigned i=0;i<defects.size();++i) {
      if (defects_d[i] < min_defect_size) continue;
      defects_tmp.emplace_back(defects[i]);
      defects_d_tmp.emplace_back(defects_d[i]);
    }

    std::swap(defects_tmp  ,defects);
    std::swap(defects_d_tmp,defects_d);
  }

  geo2d::Line<float> DefectCluster::find_line_hull_defect(const GEO2D_Contour_t& ctor, cv::Vec4i defect_info) {

    //number of points in contour
    int pts_c = ctor.size();

    // contour segment
    geo2d::LineSegment<float> ctor_segment(-1,-1,-1,-1);

    // hull defect segment
    geo2d::LineSegment<float> hull_defect_segment(-1,-1,-1,-1);

    // Line that represent the side of hull
    geo2d::Vector<float> start, end, defect, pt1, pt2;
    defect.x = ctor[defect_info[2]].x;  defect.y = ctor[defect_info[2]].y;
    start.x  = ctor[defect_info[0]].x;  start.y  = ctor[defect_info[0]].y;
    end.x    = ctor[defect_info[1]].x;  end.y    = ctor[defect_info[1]].y;

    auto hull_line = geo2d::Line<float>(start,end-start);
    
    float dist_defect_to_hull = geo2d::ClosestPoint(hull_line, defect, pt1, pt2);

    const geo2d::Vector<float> hull_to_defect = geo2d::dir(pt2,pt1);

    LAROCV_DEBUG() << "Defect: " << defect
		   << " ... hull line: " << start << " => " << end
		   << " (dist=" << dist_defect_to_hull << ")" << std::endl;
    LAROCV_DEBUG() << "Hull to defect dir : " << hull_to_defect <<  std::endl;

    /*
    auto minidx = std::min(defect_info[0],defect_info[1]);
    auto maxidx = std::max(defect_info[0],defect_info[1]);
    */
    std::vector<size_t> ctor_idx_v;
    ctor_idx_v.reserve(ctor.size());
    if(defect_info[1] > defect_info[0]) {
      auto idx = defect_info[0];
      while(1) {
	ctor_idx_v.push_back(idx);
	if(idx == defect_info[1]) break;
	++idx;
      }
    }else{
      auto idx = defect_info[0];
      while(1) {
	ctor_idx_v.push_back(idx%ctor.size());
	if((idx%ctor.size()) == defect_info[1]) break;
	++idx;
      }
    }
    
    int npts=_hull_edge_pts_split;

    // These are parameters to be computed (for return)
    float yinter=0.;
    float dir=0.;

    // Loop sequence control variable
    bool first = true;
    while(1) {

      if(!first) {

	LAROCV_DEBUG() << "Could not find a breaker line ... trying dist: " << dist_defect_to_hull
		       << " => " << dist_defect_to_hull/2. << std::endl;
	dist_defect_to_hull /= 2.;
	start += hull_to_defect * dist_defect_to_hull;
	end   += hull_to_defect * dist_defect_to_hull;
	
	if(dist_defect_to_hull < 1) throw larbys("Could not locate a breaking line");
      }
      first=false;
      
      // start
      float x1 = start.x;
      float y1 = start.y;
      // end
      float x2 = end.x;
      float y2 = end.y;
      // defect
      float x3 = defect.x;
      float y3 = defect.y;
      
      //LAROCV_DEBUG() << "Inspecting hull defect\n";
      //LAROCV_DEBUG() << "start and end of edge\n";
      //LAROCV_DEBUG() << "plt.plot(["<<start.x<<","<<end.x<<"],["<<start.y<<","<<end.y<<"],'-',lw=5)\n";
     
      
      std::vector< geo2d::Vector<float> > l_;
      l_.resize(npts);

      geo2d::Vector<float> dir_(x2-x1,y2-y1);
    
      // make the points on the hull edge
      for(int j=0;j<npts;++j) {
	float k = ( (float) j ) / ( (float) npts );
	l_[j] = dir_*k+geo2d::Vector<float>(x1,y1);
      }
    
      geo2d::Vector<float> p3(x3,y3);
      geo2d::Vector<float> p4(-1,-1);

      float mdist = 9.e9;
      
      for ( const auto& l : l_ ) {
	
	// number intersections counter
	int inters = 0;
	
	// min and max index from contour start to end, so we know how to order the loop
	hull_defect_segment.pt1.x = p3.x;
	hull_defect_segment.pt1.y = p3.y;
	hull_defect_segment.pt2.x = l.x;
	hull_defect_segment.pt2.y = l.y;
	
	// loop over portion of contour facing edge
	//LAROCV_DEBUG() << "\n\n\n\n";
	for(auto const& ix : ctor_idx_v) {
	  
	  ctor_segment.pt1 = ctor[ ix         ];
	  ctor_segment.pt2 = ctor[(ix+1)%pts_c];
	  
	  inters += geo2d::Intersect(ctor_segment,hull_defect_segment);

	  //LAROCV_DEBUG() << ix << " " << (ix+1)%pts_c << " " << ctor[ix] << " " << ctor[(ix+1)%pts_c] << " " << inters << "\n";
	  
	  //There is more than one intersection for this line
	  //between the contour and the hull
	  
	  if (inters > 2) break;
	  
	}

	//the line cut through the contour
	if(inters > 2) continue;

	float dd = geo2d::length(hull_defect_segment);
	
	if ( dd > mdist ) continue;
	
	mdist = dd;
	
	p4 = l;
      }
      
      if (p4.x == -1 || p4.y == -1) continue;

      // Found the breaking point
      dir    = (p4.y-p3.y) / (p4.x-p3.x);
      yinter = -1.0*dir*p4.x+p4.y;
      break;
    }
    return geo2d::Line<float>(geo2d::Vector<float>(0,yinter),geo2d::Vector<float>(1,dir));
  }

  geo2d::Line<float> DefectCluster::scan_breaker(const GEO2D_Contour_t& ctor, cv::Vec4i defect_info) {

    //number of points in contour
    int pts_c = ctor.size();

    // contour segment
    geo2d::LineSegment<float> ctor_segment(-1,-1,-1,-1);

    // hull defect segment
    geo2d::LineSegment<float> hull_defect_segment(-1,-1,-1,-1);

    // Line that represent the side of hull
    geo2d::Vector<float> start, end, defect;
    defect.x = ctor[defect_info[2]].x;  defect.y = ctor[defect_info[2]].y;
    start.x  = ctor[defect_info[0]].x;  start.y  = ctor[defect_info[0]].y;
    end.x    = ctor[defect_info[1]].x;  end.y    = ctor[defect_info[1]].y;

    //auto const minidx = std::min(defect_info[0],defect_info[1]);
    //auto const maxidx = std::max(defect_info[0],defect_info[1]);

    std::vector<size_t> ctor_idx_v;
    ctor_idx_v.reserve(ctor.size());
    if(defect_info[1] > defect_info[0]) {
      auto idx = defect_info[0];
      while(1) {
	ctor_idx_v.push_back(idx);
	if(idx == defect_info[1]) break;
	++idx;
      }
    }else{
      auto idx = defect_info[0];
      while(1) {
	ctor_idx_v.push_back(idx%ctor.size());
	if((idx%ctor.size()) == defect_info[1]) break;
	++idx;
      }
    }
    
    int npts=_hull_edge_pts_split;

    bool scan_mode = true;
    size_t scan_edge1=0;
    size_t scan_edge2=ctor_idx_v.size()-1;

    // These are parameters to be computed (for return)
    float yinter=0.;
    float dir=0.;

    // Loop sequence control variable
    bool first = true;
    while(1) {

      if(!first) {
	if( !scan_mode ) {
	  if ( scan_edge2 < 1 ) throw larbys("LOGIC ERROR");
	  if ( ctor_idx_v[(scan_edge2-1)%(ctor.size())] == defect_info[2] )  
	    throw larbys("Scan reached the defect point. Could not locate the breaking line...");
	  --scan_edge2;
	}else{
	  if ( ctor_idx_v[(scan_edge1+1)%(ctor.size())] == defect_info[2] )  {
	    // Flipping the scan mode
	    scan_edge1 = 0;
	    scan_edge2 = ctor_idx_v.size()-1;
	    scan_mode = false;
	  }else
	    ++scan_edge1;
	}
      }
      first=false;

      start = ctor[ctor_idx_v[scan_edge1%(ctor.size())]];
      end   = ctor[ctor_idx_v[scan_edge2%(ctor.size())]];
      
      // start
      float x1 = start.x;
      float y1 = start.y;
      // end
      float x2 = end.x;
      float y2 = end.y;
      // defect
      float x3 = defect.x;
      float y3 = defect.y;
      
      //LAROCV_DEBUG() << "Inspecting hull defect\n";
      //LAROCV_DEBUG() << "start and end of edge\n";
      //LAROCV_DEBUG() << "plt.plot(["<<start.x<<","<<end.x<<"],["<<start.y<<","<<end.y<<"],'-',lw=5)\n";
     
      
      std::vector< geo2d::Vector<float> > l_;
      l_.resize(npts);

      geo2d::Vector<float> dir_(x2-x1,y2-y1);
    
      // make the points on the hull edge
      for(int j=0;j<npts;++j) {
	float k = ( (float) j ) / ( (float) npts );
	l_[j] = dir_*k+geo2d::Vector<float>(x1,y1);
      }
    
      geo2d::Vector<float> p3(x3,y3);
      geo2d::Vector<float> p4(-1,-1);

      float mdist = 9.e9;
      
      for ( const auto& l : l_ ) {
	
	// number intersections counter
	int inters = 0;
	
	// min and max index from contour start to end, so we know how to order the loop
	hull_defect_segment.pt1.x = p3.x;
	hull_defect_segment.pt1.y = p3.y;
	hull_defect_segment.pt2.x = l.x;
	hull_defect_segment.pt2.y = l.y;
	
	// loop over portion of contour facing edge
	//LAROCV_DEBUG() << "\n\n\n\n";
	for(auto const& ix : ctor_idx_v) {
	  
	  ctor_segment.pt1 = ctor[ ix         ];
	  ctor_segment.pt2 = ctor[(ix+1)%pts_c];
	  
	  inters += geo2d::Intersect(ctor_segment,hull_defect_segment);

	  //LAROCV_DEBUG() << ix << " " << (ix+1)%pts_c << " " << ctor[ix] << " " << ctor[(ix+1)%pts_c] << " " << inters << "\n";
	  
	  //There is more than one intersection for this line
	  //between the contour and the hull
	  
	  if (inters > 2) break;
	  
	}

	//the line cut through the contour
	if(inters > 2) continue;

	float dd = geo2d::length(hull_defect_segment);
	
	if ( dd > mdist ) continue;
	
	mdist = dd;
	
	p4 = l;
      }
      
      if (p4.x == -1 || p4.y == -1) continue;

      // Found the breaking point
      dir    = (p4.y-p3.y) / (p4.x-p3.x);
      yinter = -1.0*dir*p4.x+p4.y;
      break;
    }
    return geo2d::Line<float>(geo2d::Vector<float>(0,yinter),geo2d::Vector<float>(1,dir));
  }
  
  void DefectCluster::split_contour(const GEO2D_Contour_t& ctor,
				    GEO2D_Contour_t& ctor1,
				    GEO2D_Contour_t& ctor2,
				    const geo2d::Line<float>& line) {
    
    //get the two intersection points of this contour and this line
    //one of these points is presumably on the contour
    //the other needs to be extended through the contour, and calculated
    
    auto pts_c = ctor.size();

    GEO2D_Contour_t ctor_copy;
    ctor_copy.reserve(ctor.size());

    geo2d::LineSegment<float> ctor_segment(-1,-1,-1,-1);
    geo2d::LineSegment<float> span_segment(-1,-1,-1,-1);

    uint n_inters=0;
    uint n_new_inters=0;
    
    for(unsigned i=0; i<ctor.size(); ++i) {
      
      auto p1 = cv::Point2f(ctor[ i   %pts_c]);
      auto p2 = cv::Point2f(ctor[(i+1)%pts_c]);
      
      ctor_segment.pt1 = p1;
      ctor_segment.pt2 = p2;
      
      // put the first point into the copy contour
      ctor_copy.emplace_back(p1);

      //make a line segment which spans the full range of
      //X from this contour segment
      
      auto min_x = std::min(p1.x,p2.x);
      auto max_x = std::max(p1.x,p2.x);

      if (min_x == max_x) { min_x-=1; max_x+=1; }
      
      geo2d::Vector<float> p3(min_x,line.y(min_x));
      geo2d::Vector<float> p4(max_x,line.y(max_x));

      span_segment.pt1 = p3;
      span_segment.pt2 = p4;
      
      // intersection points for the two segments, one I made
      // and the other is the contour itself
      geo2d::Vector<float> ip(0,0);

      // do they intersect
      if ( ! geo2d::IntersectionPoint(ctor_segment,span_segment,ip) ) continue;

      //they intersect, cast to int
      cv::Point inter_pt(ip.x,ip.y);
      
      //this point is already a part of the contour, don't put a duplicate inside
      if ( inter_pt == cv::Point(p1) or inter_pt == cv::Point(p2) ){
	LAROCV_DEBUG() << "inter_pt " << inter_pt << " is either p1 " << p1 << " or " << p2 << std::endl;
	n_inters++;
	continue;
      }

      LAROCV_DEBUG() << "Inserting a new point: " << inter_pt << std::endl;
      ctor_copy.emplace_back(std::move(inter_pt));// this cv::Point2f has been floored with typecast
      n_new_inters++;
      n_inters++;
    }

    LAROCV_DEBUG() << "New intersections inserted " << n_new_inters << " total intersections " << n_inters << std::endl;
      
    // for each point in the copy contour
    for(auto& pt : ctor_copy) {

      // // point on the line (with tolerance) --  we should put it into both output contours
      // if ( on_line(line,pt) )
      // 	{ ctor1.emplace_back(pt); ctor2.emplace_back(pt); continue; }
      
      // point is above the line -- put in contour 1
      if ( pt.y > line.y(pt.x) )
	{ ctor1.emplace_back(pt); continue; }

      // it's below the line -- put in contour 2
      ctor2.emplace_back(pt);
    }
    /*
    int xmin,xmax,ymin,ymax;

    xmin=xmax=ymin=ymax=-1;
    for(auto const& pt : ctor1) {
      if(xmin<0) {
	xmin = xmax = pt.x;
	ymin = ymax = pt.y;
      }
      if(pt.x < xmin) xmin = pt.x;
      if(pt.x > xmax) xmax = pt.x;
      if(pt.y < ymin) ymin = pt.y;
      if(pt.y > ymax) ymax = pt.y;
    }

    cv::Mat img1(ymax-ymin+1,xmax-xmin+1,CV_8UC1,cvScalar(0.));
    for(auto const& pt : ctor1) 
      img1.at<unsigned char>((size_t)(pt.y-ymin),(size_t)(pt.x-xmin)) = (unsigned char)255;
    auto kernel = ::cv::getStructuringElement(cv::MORPH_ELLIPSE,::cv::Size(5,5));
    ::cv::dilate(img1,img1,kernel,::cv::Point(-1,-1),1);
    ::cv::blur(img1,img1,::cv::Size(5,5));

    cv::imwrite("poop1.png",img1);

    // find contours again
    ContourArray_t actor_v;
    std::vector<::cv::Vec4i> cv_hierarchy_v;
    actor_v.clear(); cv_hierarchy_v.clear();
    ::cv::findContours(img1,actor_v,cv_hierarchy_v,
		       CV_RETR_EXTERNAL,
		       CV_CHAIN_APPROX_SIMPLE);
    ctor1 = actor_v[0];
    if(actor_v.size()>1) {
      std::cout<<actor_v.size()<<std::endl;
      throw larbys("FUCK");
    }
    for(auto& pt : ctor1) { pt.x += xmin; pt.y += ymin; }

    xmin=xmax=ymin=ymax=-1;
    for(auto const& pt : ctor2) {
      if(xmin<0) {
	xmin = xmax = pt.x;
	ymin = ymax = pt.y;
      }
      if(pt.x < xmin) xmin = pt.x;
      if(pt.x > xmax) xmax = pt.x;
      if(pt.y < ymin) ymin = pt.y;
      if(pt.y > ymax) ymax = pt.y;
    }

    cv::Mat img2(xmax-xmin+1,ymax-ymin+1,CV_8UC1,cvScalar(0.));
    for(auto const& pt : ctor2) 
      img2.at<unsigned char>((size_t)(pt.y - ymin),(size_t)(pt.x - xmin)) = (unsigned char)255;
    //auto kernel = ::cv::getStructuringElement(cv::MORPH_ELLIPSE,::cv::Size(5,5));
    ::cv::dilate(img2,img2,kernel,::cv::Point(-1,-1),1);
    ::cv::blur(img2,img2,::cv::Size(5,5));
    cv::imwrite("poop2.png",img2);

    // find contours again
    actor_v.clear(); cv_hierarchy_v.clear();
    ::cv::findContours(img2,actor_v,cv_hierarchy_v,
		       CV_RETR_EXTERNAL,
		       CV_CHAIN_APPROX_SIMPLE);
    ctor2 = actor_v[0];
    if(actor_v.size()>1) {
      std::cout<<actor_v.size()<<std::endl;
      throw larbys("FUCK");
    }
    for(auto& pt : ctor2) { pt.x += xmin; pt.y += ymin; }

    */
    
    // Do contour finding on these points
    
    /*
    if(ctor1.size()>2 && ctor1.back() != ctor1.front())
      ctor1.push_back(ctor1[0]);
    
    if(ctor2.size()>2 && ctor2.back() != ctor2.front())
      ctor2.push_back(ctor2[0]);
    */

    /*
    for(size_t i=0; i<ctor1.size(); ++i) {
      for(size_t j=i; j<ctor1.size(); ++j) {
	if(ctor1[i] == ctor1[j]) throw larbys("RUI");
      }
    }

    for(size_t i=0; i<ctor2.size(); ++i) {
      for(size_t j=i; j<ctor2.size(); ++j) {
	if(ctor2[i] == ctor2[j]) throw larbys("RUI");
      }
    }
    
    if(ctor1.size()>3)
      geo2d::UntanglePolygon(ctor1);
    if(ctor2.size()>3)
      geo2d::UntanglePolygon(ctor2);
    */
  }

  cv::Vec4i DefectCluster::max_hull_edge(const GEO2D_Contour_t& ctor, std::vector<cv::Vec4i> defects) {

    float max_dist = -1;
    int max_idx    = -1;
    
    for(unsigned i=0;i<defects.size();++i) {

      const auto& d1=defects[i];
      
      float ll=std::sqrt(std::pow(ctor[d1[0]].x-ctor[d1[1]].x,2)
			 +
			 std::pow(ctor[d1[0]].y-ctor[d1[1]].y,2));

      if (ll > max_dist) {
	max_dist = ll;
	max_idx  = i;
      }
      
    }

    if (max_idx == -1) throw larbys("Maximum hull edge could not be calculated\n");
    
    return defects.at(max_idx);
      
  }

  void DefectCluster::AssociateDefects(const data::ClusterCompound& cluscomp,
				       const data::AtomicContour& parent,
				       const data::ContourDefect& defect,
				       data::AtomicContour& child1,
				       data::AtomicContour& child2)
  {

    if(this->logger().level() <= ::larocv::msg::kDEBUG) {
      std::stringstream ss;
      ss << "Parent defect points:";
      for(auto const& defect_id : parent.associated_defects())
	ss << " ID " << defect_id << " @ " << cluscomp.get_defect(defect_id)._pt_defect;
      ss << " ... new defect ID " << defect.id() << " @ " << defect._pt_defect;
      LAROCV_DEBUG() << std::string(ss.str()) << std::endl;
    }
    // Three cases to handle
    // case 0) parent is the "original" (i.e. no associated defect)
    // case 1) parent is an "edge" (i.e. 1 associated defect)
    // case 2) parent is "non-edge" (i.e. 2 associated defects)
    
    // case 0) simply associate both children to the defect
    if(parent.associated_defects().empty()) {
      LAROCV_DEBUG() << "Case 0: defect " << defect.id() << " is associated with both children" << std::endl;
      child1.associate(defect.id());
      child2.associate(defect.id());
      return;
    }

    // case 1 and 2 both require to compute the "closest defects from history" to each child
    
    // Look for the closest defect in the parent (parent) for child 1
    double closest_defect1_dist1 = 1e20;
    double closest_defect1_dist2 = 1e20;
    size_t closest_defect1_id1 = kINVALID_SIZE;
    size_t closest_defect1_id2 = kINVALID_SIZE;
    for(auto const& defect_id : parent.associated_defects()) {
      auto const& defect_pt = cluscomp.get_defect(defect_id)._pt_defect;
      double dist = pointPolygonTest(child1._ctor,defect_pt,true) * -1.;
      if(dist <  closest_defect1_dist1) {
	// update 2nd closest defect
	closest_defect1_dist2 = closest_defect1_dist1;
	closest_defect1_id2   = closest_defect1_id1;
	closest_defect1_dist1 = dist;
	closest_defect1_id1 = defect_id;
      }else if(dist < closest_defect1_dist2) {
	closest_defect1_dist2 = dist;
	closest_defect1_id2   = defect_id;
      }
    }

    LAROCV_DEBUG() << "Child1 atom size " << child1._ctor.size()
		   << " closest defect " << closest_defect1_id1
		   << " @ " << cluscomp.get_defect(closest_defect1_id1)._pt_defect
		   << " dist = " << closest_defect1_dist1 << std::endl;

    // Look for the closest defect in the parent (parent) for child 2
    double closest_defect2_dist1 = 1e20;
    double closest_defect2_dist2 = 1e20;
    size_t closest_defect2_id1 = kINVALID_SIZE;
    size_t closest_defect2_id2 = kINVALID_SIZE;
    for(auto const& defect_id : parent.associated_defects()) {
      auto const& defect_pt = cluscomp.get_defect(defect_id)._pt_defect;
      double dist = pointPolygonTest(child2._ctor,defect_pt,true) * -1.;
      if(dist < closest_defect2_dist1) {
	// update 2nd closest defect
	closest_defect2_dist2 = closest_defect2_dist1;
	closest_defect2_id2   = closest_defect2_id1;
	closest_defect2_dist1 = dist;
	closest_defect2_id1   = defect_id;
      }else if(dist < closest_defect2_dist2) {
	closest_defect2_dist2 = dist;
	closest_defect2_id2   = defect_id;
      }
    }
    LAROCV_DEBUG() << "Child2 atom size " << child2._ctor.size()
		   << " closest defect " << closest_defect2_id1
		   << " @ " << cluscomp.get_defect(closest_defect2_id1)._pt_defect
		   << " dist = " << closest_defect2_dist1 << std::endl;

    // case 1) parent is an "edge"
    // In this case one of children must inherit "edge" feature.
    // The edge child should have a larger distance to the "closest" defect in the history (history = 1 defect anyway...)
    if(parent.associated_defects().size() == 1) {
      if(closest_defect1_dist1 > closest_defect2_dist1) {
	child1.associate(defect.id());
	child2.associate(defect.id());
	child2.associate(closest_defect2_id1);
	LAROCV_DEBUG() << "Case 1: new edge @ defect " << defect.id()
		       << " and non-edge defect " << closest_defect2_id1
		       << std::endl;
      }else{
	child1.associate(defect.id());
	child1.associate(closest_defect1_id1);
	child2.associate(defect.id());
	LAROCV_DEBUG() << "Case 1: new edge @ defect " << defect.id()
		       << " and non-edge defect " << closest_defect1_id1
		       << std::endl;
      }
      return;
    }

    // case 2) parent is "non-edge"
    child1.associate(defect.id());
    child1.associate(closest_defect1_id1);

    child2.associate(defect.id());
    if(closest_defect1_id1 == closest_defect2_id1) {
      child2.associate(closest_defect2_id2);
      LAROCV_DEBUG() << "Case 2: defect pairs (" << defect.id() << "," << closest_defect1_id1 << ")"
		     << " and (" << defect.id() << "," << closest_defect2_id2 << ")" << std::endl;
    }
    else {
      child2.associate(closest_defect2_id1);
      LAROCV_DEBUG() << "Case 2: defect pairs (" << defect.id() << "," << closest_defect1_id1 << ")"
		     << " and (" << defect.id() << "," << closest_defect2_id1 << ")" << std::endl;
    }    
    return;
  }

  
  data::ClusterCompound DefectCluster::BreakContour(const larocv::GEO2D_Contour_t& in_ctor) {
    
    ////////////////////////////////////////////
    // Take a single contour, find the defect on the side with 
    // longest hull edge. Break the contour into two. Re insert into queue
    // repeat operation on queue

    data::ClusterCompound cluscomp;
    
    //contours which may be broken up are put in this queue
    std::vector<larocv::data::AtomicContour> candidate_ctor_v;
    std::vector<bool> deprecate_ctor_v;
    
    int nbreaks=0;

    std::vector<int> hullpts;
    std::vector<::cv::Vec4i> defects;
    std::vector<float> defects_d;
    
    LAROCV_DEBUG() << "Original contour size: " << in_ctor.size() << std::endl;
    
    data::AtomicContour first_a_ctor;
    first_a_ctor._ctor = in_ctor;
    candidate_ctor_v.push_back(first_a_ctor);
    deprecate_ctor_v.push_back(false);

    while(nbreaks<=10) {

      // get a contour out off the list
      size_t target_ctor_idx=kINVALID_SIZE;
      for(size_t candidate_idx=0; candidate_idx < candidate_ctor_v.size(); ++candidate_idx) {
	if(deprecate_ctor_v[candidate_idx]) continue;
	target_ctor_idx = candidate_idx;
	break;
      }

      if(target_ctor_idx == kINVALID_SIZE) {
	LAROCV_DEBUG() << "No more cluster to break..." << std::endl;
	break;
      }
      
      auto& a_ctor = candidate_ctor_v[target_ctor_idx];
      auto& ctor   = a_ctor._ctor;

      LAROCV_INFO() << "Next target cluster for breaking: candidate index = " << target_ctor_idx
		    << " out of " << candidate_ctor_v.size() << " candidates ... size = " << ctor.size() << std::endl;
      
      //this contour contains only two points. it's a line. should not exist.
      if (ctor.size() <= 2) {
	LAROCV_CRITICAL() << "Contour too small (size<=2)... should not appear!" << std::endl;
	throw larbys();
	//deprecate_ctor_v[target_ctor_idx] = true;
	//ctor.clear();
	//continue;
      }

      //this contour cannot have a defect by definition
      if (ctor.size() == 3) {
	LAROCV_DEBUG() << "Contour too small to look for a defect (size=3)" << std::endl;
	auto& atomic = cluscomp.make_atom();
	for(auto const& defect_id : a_ctor.associated_defects())
	  atomic.associate(defect_id);
	atomic._ctor = ctor;
	LAROCV_INFO() << "Found atomic. Currently # atoms = "<< cluscomp.get_atoms().size()
		      << " ... # defects = " << cluscomp.get_defects().size()
		      << std::endl;
	deprecate_ctor_v[target_ctor_idx] = true;
	//ctor.clear();
	continue;
      }
		 
      // clear the hull and defects for this contour
      hullpts.clear();
      defects.clear();
      defects_d.clear();
	 
      // fill the hull and defects
      try{
	fill_hull_and_defects(ctor,hullpts,defects,defects_d);
      }catch(...) {
	LAROCV_NORMAL() << "Failed to compute defects point (cv::convexityDefects)" << std::endl
			<< "Size of hullpts: " << hullpts.size() << std::endl
			<< "Size of defects: " << defects.size() << std::endl
			<< "Size of contour: " << ctor.size() << std::endl;
	deprecate_ctor_v[target_ctor_idx] = true;
	//ctor.clear();
	continue;
      }
	 
      // filter the hull and defects based on input minimum size
      filter_defects(defects,defects_d,_min_defect_size);
	 
      // no defects of minimum size found! the contour is atomic
      if ( ! defects_d.size() ) {
	auto& atomic = cluscomp.make_atom();
	for(auto const& defect_id : a_ctor.associated_defects())
	  atomic.associate(defect_id);
	atomic._ctor = ctor;
	LAROCV_INFO() << "Found atomic. Currently # atoms = "<< cluscomp.get_atoms().size()
		      << " ... # defects = " << cluscomp.get_defects().size()
		      << std::endl;
	deprecate_ctor_v[target_ctor_idx] = true;
	//ctor.clear();
	continue;
      }

      //get the chosen edge, currently take the defect facing the longest hull edge
      LAROCV_DEBUG() <<"Choosing edge for ctor..."<<std::endl;
      auto chosen_edge = max_hull_edge(ctor,defects);
      LAROCV_DEBUG() << "This chosen hull edge: " << chosen_edge << std::endl;

      LAROCV_DEBUG() <<ctor.size()<<" : "<<chosen_edge[0]<<" and "<<chosen_edge[1]<<std::endl;

      auto start = ctor[chosen_edge[0]];
      auto end   = ctor[chosen_edge[1]];
      auto diff  = end - start;

      if (std::abs(diff.x)<=3 and std::abs(diff.y)<=3) {
	LAROCV_DEBUG() << "This hull suspect... start : " <<  start << " end : " << end << " diff : " << diff << std::endl;
	auto& atomic = cluscomp.make_atom();
	for(auto const& defect_id : a_ctor.associated_defects())
	  atomic.associate(defect_id);
	atomic._ctor = ctor;
	LAROCV_INFO() << "Found atomic. Currently # atoms = "<< cluscomp.get_atoms().size()
		      << " ... # defects = " << cluscomp.get_defects().size()
		      << std::endl;
	deprecate_ctor_v[target_ctor_idx] = true;
	//ctor.clear();
	continue;
      }

      nbreaks+=1;
      LAROCV_DEBUG() << "Breaking! Number of total breaks : " << nbreaks << std::endl;

      // segment the hull line and find the line between the hull edge and
      // the defect point, which does not intersect the contour itself, other than at the defect point
      geo2d::Line<float> min_line;
      bool broken=false;
      try{
	min_line = find_line_hull_defect(ctor,chosen_edge);
	broken=true;
      }catch(const larbys& err) {
	LAROCV_INFO() << "Could not break the contour by moving hull line ..."<<std::endl;
      }
      if(!broken) {
	try{
	  min_line = scan_breaker(ctor, chosen_edge);
	  broken = true;
	}catch(const larbys& err) {
	  LAROCV_INFO() << "Could not break the contour by scanning the perimeter..." << std::endl;
	}
      }

      if(!broken) {
	LAROCV_NORMAL() << "Registering un-breakable contour into an atomic contour list..." << std::endl;
	auto& atomic = cluscomp.make_atom();
	for(auto const& defect_id : a_ctor.associated_defects())
	  atomic.associate(defect_id);
	atomic._ctor = ctor;
	LAROCV_INFO() << "Found atomic. Currently # atoms = "<< cluscomp.get_atoms().size()
		      << " ... # defects = " << cluscomp.get_defects().size()
		      << std::endl;
	deprecate_ctor_v[target_ctor_idx] = true;
	//ctor.clear();
	continue;	   
      }

      GEO2D_Contour_t ctor1, ctor2;
      // split the contour into two by this line
      split_contour(ctor,ctor1,ctor2,min_line);

      if(ctor1.size()<=2  && ctor2.size()<=2) {
	LAROCV_DEBUG() << "Contour got split into 2 too small contours (size<=2) ... storing original as an atom" << std::endl;
	auto& atomic = cluscomp.make_atom();
	for(auto const& defect_id : a_ctor.associated_defects())
	  atomic.associate(defect_id);
	atomic._ctor = ctor;
	LAROCV_INFO() << "Found atomic. Currently # atoms = "<< cluscomp.get_atoms().size()
		       << " ... # defects = " << cluscomp.get_defects().size()
		       << std::endl;
	deprecate_ctor_v[target_ctor_idx] = true;
	//ctor.clear();
	continue;
      }
	 	 
      LAROCV_DEBUG() << "Split contour of size: " << ctor.size()
		     << " into " << ctor1.size() << " + " << ctor2.size()
		     << " = " << ctor1.size()+ctor2.size() << std::endl;

      if(ctor1.size() >2 && ctor2.size() >2) {
	auto& defect = cluscomp.make_defect();
	defect._pt_start  = ctor[chosen_edge[0]];
	defect._pt_end    = ctor[chosen_edge[1]];
	defect._pt_defect = ctor[chosen_edge[2]];
	defect._dist      = (float)chosen_edge[3] / 256.;
	defect._split_line = min_line;

	data::AtomicContour a_ctor1;
	a_ctor1._ctor = ctor1;
	data::AtomicContour a_ctor2;
	a_ctor2._ctor = ctor2;
	AssociateDefects(cluscomp,a_ctor,defect,a_ctor1,a_ctor2);

	//candidate_ctor_v.emplace_back(std::move(a_ctor1));
	//candidate_ctor_v.emplace_back(std::move(a_ctor2));
	candidate_ctor_v.push_back(a_ctor1);
	candidate_ctor_v.push_back(a_ctor2);
	deprecate_ctor_v.push_back(false);
	deprecate_ctor_v.push_back(false);

      }else {

	if(ctor1.size()<=2) { LAROCV_DEBUG() << "Broken contour too small (size<=2)... ignoring..." << std::endl; }
	else {
	  data::AtomicContour a_ctor1;
	  a_ctor1._ctor = ctor1;
	  for(auto const& defect_id : a_ctor.associated_defects())
	    a_ctor1.associate(defect_id);
	  //candidate_ctor_v.emplace_back(std::move(a_ctor1));
	  candidate_ctor_v.push_back(a_ctor1);
	  deprecate_ctor_v.push_back(false);
	}

	if(ctor2.size()<=2) { LAROCV_DEBUG() << "Broken contour too small (size<=2)... ignoring..." << std::endl; }
	else {
	  data::AtomicContour a_ctor2;
	  a_ctor2._ctor = ctor2;
	  for(auto const& defect_id : a_ctor.associated_defects())
	    a_ctor2.associate(defect_id);
	  //candidate_ctor_v.emplace_back(std::move(a_ctor2));
	  candidate_ctor_v.push_back(a_ctor2);
	  deprecate_ctor_v.push_back(false);
	}
      }
      
      deprecate_ctor_v[target_ctor_idx] = true;
      //ctor.clear();
      
      size_t num_remaining_candidates = 0;
      for(auto const& deprecate : deprecate_ctor_v) {
	if(!deprecate) ++num_remaining_candidates;
      }
      LAROCV_DEBUG() << "Valid candidate size " << num_remaining_candidates << " @ end of loop..." << std::endl;

    } //end of breaking

    //atomic_contour_v is filled, candidate_ctor_v should be all deprecated, if its not, put what's inside into atomics
    for (size_t target_ctor_idx=0; target_ctor_idx<candidate_ctor_v.size(); ++target_ctor_idx) {

      auto& candidate_ctor = candidate_ctor_v[target_ctor_idx];
      
      if(deprecate_ctor_v[target_ctor_idx]) continue;
      if(candidate_ctor._ctor.size() <= 2) continue;
      
      LAROCV_NORMAL() << "Putting defect ctor of size : " << candidate_ctor._ctor.size() << " into atomic_atomic_ctor_v "<< std::endl;
      auto& atomic = cluscomp.make_atom();
      for(auto const& defect_id : candidate_ctor.associated_defects())
	atomic.associate(defect_id);
      atomic._ctor = candidate_ctor._ctor;
      LAROCV_DEBUG() << "Currently # atoms = "<< cluscomp.get_atoms().size()
		     << " ... # defects = " << cluscomp.get_defects().size()
		     << std::endl;
    }

    // now perform correlation between defects and atomic contours.
    // this is not done in the loop above because it appears hard in the 1st pass.
    // we implement using a geometrical approximation.
    auto const& ctor_defect_v = cluscomp.get_defects();
    auto const& atomic_ctor_v = cluscomp.get_atoms();
    // make sure logic stands: # defects should be 1 less than # contours
    if(ctor_defect_v.size()+1 != atomic_ctor_v.size()) {
      LAROCV_CRITICAL() << "Logic error ... # of defects = " << ctor_defect_v.size()
			<< " ... "
			<< "# of atoms = " << atomic_ctor_v.size()
			<< std::endl;
      throw larbys();
    }
    /*
    // Following block is for geometric approximation for defect <=> atom association
    // If you want to use it, make sure you do not execute the algorithmic construction in the loop above.
    LAROCV_DEBUG() << "Constructing association:"
		   << " # defects = " << ctor_defect_v.size()
		   << " # atoms = " << atomic_ctor_v.size() << std::endl;

    // loop over defects, look for 2 closest atoms
    std::vector<int> used_atom_v(cluscomp.get_atoms().size(),0);
    for(auto const& defect : ctor_defect_v) {
      double mindist1=1e10;
      double mindist2=1e10;
      size_t atomid1=kINVALID_SIZE;
      size_t atomid2=kINVALID_SIZE;
      auto const& defect_pt = defect._pt_defect;
      LAROCV_DEBUG() << "Defect @ " << defect_pt << " (ID=" << defect.id() << ")" << std::endl;
      for(auto const& atom : atomic_ctor_v) {
	if(used_atom_v[atom.id()] >=2) continue;
	double mindist =1e10;
	double dist=0;
	for(auto const& pt : atom._ctor) {
	  dist = pow(pt.x - defect_pt.x,2) + pow(pt.y - defect_pt.y,2);
	  if(dist < mindist) mindist = dist;
	}
	LAROCV_DEBUG() << "Defect ID " << defect.id()
		       << " to Atom ID " << atom.id()
		       << " distance = " << mindist << std::endl;
	if(mindist < mindist1) {
	  // update 2nd closest one as well
	  mindist2 = mindist1;
	  atomid2  = atomid1;
	  
	  mindist1 = mindist;
	  atomid1  = atom.id();
	}else if(mindist < mindist2) {
	  mindist2 = mindist;
	  atomid2  = atom.id();
	}
      }
      if(atomid1 == kINVALID_SIZE || atomid2 == kINVALID_SIZE) {
	LAROCV_CRITICAL() << "Could not find 2 associated atoms for defect " << defect.id() << std::endl;
	throw larbys();
      }
      cluscomp.associate(atomid1,defect.id());
      cluscomp.associate(atomid2,defect.id());
      used_atom_v[atomid1] += 1;
      used_atom_v[atomid2] += 1;
    }
    */

    // Loop over atoms to complete association for defects=>atom
    for(auto const& atom : cluscomp.get_atoms()) {

      for(auto const& defect_id : atom.associated_defects())

	cluscomp.associate(atom.id(),defect_id);

    }

    // INFO report
    if(this->logger().level() <= ::larocv::msg::kINFO) {
      std::stringstream ss;
      ss << "Reporting the final set of atoms/defects" << std::endl;
      for(auto const& atom : cluscomp.get_atoms()) {

	ss << "    Atom ID " << atom.id() << " size = " << atom._ctor.size() << " ... associated defects: ";
	for(auto const& defect_id : atom.associated_defects())

	  ss << defect_id << " @ " << cluscomp.get_defect(defect_id)._pt_defect << " ... ";

	ss << std::endl;
      }
      LAROCV_INFO() << std::string(ss.str()) << std::endl;
    }
    
    return cluscomp;
  }

}
#endif
