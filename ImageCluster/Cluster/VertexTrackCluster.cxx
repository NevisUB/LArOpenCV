#ifndef __VERTEXTRACKCLUSTER_CXX__
#define __VERTEXTRACKCLUSTER_CXX__

#include "VertexTrackCluster.h"
#include "Refine2DVertex.h"
#include "Core/Geo2D.h"

namespace larocv {

  /// Global larocv::VertexTrackClusterFactory to register AlgoFactory
  static VertexTrackClusterFactory __global_VERTEXTRACKClusterFactory__;

  void VertexTrackCluster::_Configure_(const ::fcllite::PSet &pset)
  {

    _dilation_size = pset.get<int>("DilationSize",2);
    _dilation_iter = pset.get<int>("DilationIter",1);
    _blur_size     = pset.get<int>("BlurSize",2);
    
    _thresh        = pset.get<float>("Thresh");
    _thresh_maxval = pset.get<float>("ThreshMaxVal");
    
    _mask_radius = 5.;

    _theta_hi = 10;
    _theta_lo = 10;

    _pi_threshold = 10;
    
    auto const vtx_algo_name = pset.get<std::string>("Refine2DVertexAlgo");
    _refine2d_algo_id = this->ID(vtx_algo_name);
    
    _use_theta_half_angle = true;
  }

  std::vector<larocv::data::ParticleCluster >
  VertexTrackCluster::TrackHypothesis(const ::cv::Mat& img,
				      const data::CircleVertex& vtx)
  {

    auto const& ref_vtx  = vtx.center;
    auto const& ref_xs_v = vtx.xs_v;

    std::vector< larocv::data::ParticleCluster >result_v;

    bool use_half_angle = (ref_xs_v.size() > 2) && _use_theta_half_angle;
    
    for(int xs_pt_idx=0; xs_pt_idx<ref_xs_v.size(); ++xs_pt_idx) {

      auto ref_vtx_copy = ref_vtx;

      float angle,theta_lo,theta_hi;

      cv::Mat rot_img;
      cv::Mat img_copy = img;

      cv::Mat img_padded;
      int padding = sqrt(img.rows*img.rows+img.cols*img.cols)/2;
      LAROCV_DEBUG() << "Diagonal is: " << padding << "... from row: " << img.rows << "... and cols: " << img.cols << std::endl;

      img_padded.create(img_copy.rows + 2*padding, img_copy.cols + 2*padding, img_copy.type());
      img_padded.setTo(cv::Scalar::all(0));
      img_copy.copyTo(img_padded(cv::Rect(padding, padding, img.cols, img.rows)));

      ref_vtx_copy.x+=padding;
      ref_vtx_copy.y+=padding;
      
      LAROCV_DEBUG() << "Reference vertex @ " << ref_vtx_copy << std::endl;
      
      if (use_half_angle) {
	int idx0  = xs_pt_idx-1 >= 0 ? (xs_pt_idx-1)%ref_xs_v.size() : xs_pt_idx-1+ref_xs_v.size();
	int idx1  = xs_pt_idx;
	int idx2  = (xs_pt_idx+1)%ref_xs_v.size();
	
	LAROCV_DEBUG() << "idx0 : " << idx0 << "... idx1: " << idx1 << "... idx2: " << idx2 << std::endl;

	auto xs0 = ref_xs_v[idx0].pt; xs0.x+=padding; xs0.y+=padding;
	auto xs1 = ref_xs_v[idx1].pt; xs1.x+=padding; xs1.y+=padding;
	auto xs2 = ref_xs_v[idx2].pt; xs2.x+=padding; xs2.y+=padding;
	
	LAROCV_DEBUG() << "Inspecting XS0 @ " << xs0 << " XS1 @ " << xs1 << " XS2 @ " << xs2 << std::endl;

	float angle0  = geo2d::angle(ref_vtx_copy,xs0);
	float angle1  = geo2d::angle(ref_vtx_copy,xs1);
	float angle2  = geo2d::angle(ref_vtx_copy,xs2);

	LAROCV_DEBUG() << "Angles are 0: " << angle0 << "... 1: " << angle1 << "... 2: " << angle2 << std::endl;

	if ( angle0 < 0 ) angle0+=360;
	if ( angle1 < 0 ) angle1+=360;
	if ( angle2 < 0 ) angle2+=360;
      
	float dangle10 = std::abs(angle1-angle0)/2.0;
	float dangle21 = std::abs(angle2-angle1)/2.0;
      
	angle = angle1 < 0 ? angle1+180-360. : angle1+180;

	LAROCV_DEBUG() << "Computed angle: " << angle << "... dangle10: " << dangle10 << "... dangle21: " << dangle21 << std::endl;

	theta_lo = dangle10;
	theta_hi = dangle21;
	
      } else { 
      
	auto xs = ref_xs_v[xs_pt_idx].pt; xs.x+=padding; xs.y+=padding;
	
	LAROCV_DEBUG() << "Inspecting XS @ " << xs << std::endl;
	
	angle = geo2d::angle(ref_vtx_copy,xs);
	angle += 180.;

	theta_lo = _theta_lo;
	theta_hi = _theta_hi;
      
      }
      
      auto rot = ::cv::getRotationMatrix2D(ref_vtx_copy,angle,1.);
      cv::Rect bbox = cv::RotatedRect(ref_vtx_copy,img_padded.size(),angle).boundingRect(); 
      LAROCV_DEBUG() << "bbox : " << bbox << "... size " << bbox.size() << std::endl;
      
      cv::warpAffine(img_padded, rot_img, rot, bbox.size());

      LAROCV_DEBUG() << "rot_img rows: " << rot_img.rows << "... cols: " << rot_img.cols << std::endl;
      
      /*
	std::stringstream ss1,ss2;
	ss1 << "norm_plane" << "_xs" << xs_pt_idx << ".png";
	ss2 << "rot_plane" << "_xs" << xs_pt_idx << ".png";
	cv::imwrite(std::string(ss1.str()).c_str(), img_padded);
	cv::imwrite(std::string(ss2.str()).c_str(), rot_img);
      */

      cv::Mat rot_polarimg, sb_img;
      
      // Cluster per xs-point found in Refine2DVertex

      int max_radius=1000;

      ::cv::threshold(rot_img,rot_img,_pi_threshold,255,CV_THRESH_BINARY);
      
      cv::linearPolar(rot_img,        //input
		      rot_polarimg,   //output
		      ref_vtx_copy,
		      max_radius,
		      cv::WARP_FILL_OUTLIERS); //seems like it has to set

      auto kernel = ::cv::getStructuringElement(cv::MORPH_RECT,
      						cv::Size(20,2));
      ::cv::dilate(rot_polarimg,rot_polarimg,kernel,::cv::Point(-1,-1),1);     
      /*
      std::stringstream ss3;
      ss3 << "polar_plane" << "_xs" << xs_pt_idx << ".png";
      cv::imwrite(std::string(ss3.str()).c_str(), rot_polarimg);
      */
      /*
      // mask-out very-near vtx pixels
      size_t mask_col_max = _mask_radius/1000. * rot_polarimg.cols + 1;
      for(size_t row=0; row<rot_polarimg.rows; row++) {
	for(size_t col=0; col<mask_col_max; ++col) {
	  rot_polarimg.at<unsigned char>(row, col) = (unsigned char)0;
	}
      }
      */
      
      // mask-out a bit further pixels for angles outside the range
      size_t row_min, row_max;

      LAROCV_DEBUG() << "rot_polarimg... rows: " << rot_polarimg.rows << "... cols: " << rot_polarimg.cols << std::endl;
      LAROCV_DEBUG() << "theta_lo: " << theta_lo << "... theta_hi: " << theta_hi << std::endl;

      row_min = (size_t)((float)(rot_polarimg.rows) * (0.5 - theta_lo/360.)); // was 10/360.
      row_max = (size_t)((float)(rot_polarimg.rows) * (0.5 + theta_hi/360.));

      LAROCV_DEBUG() << "row min: " << row_min << "... row max: " << row_max << std::endl;      

      for(size_t row=0; row<=row_min; ++row) {
	for(size_t col=0; col<rot_polarimg.cols; col++) {
	  rot_polarimg.at<unsigned char>(row,col) = (unsigned char)0;
	}
      }
      
      for(size_t row=row_max; row<rot_polarimg.rows; ++row) {
	for(size_t col=0; col<rot_polarimg.cols; col++) {
	  rot_polarimg.at<unsigned char>(row,col) = (unsigned char)0;
	}
      }
      /*
      std::stringstream ss4;
      ss4 << "mask_plane" << "_xs" << xs_pt_idx << ".png";
      cv::imwrite(std::string(ss4.str()).c_str(), rot_polarimg);
      */     

      //Contour finding
      ContourArray_t polar_ctor_v;    
      std::vector<::cv::Vec4i> cv_hierarchy_v;
      polar_ctor_v.clear(); cv_hierarchy_v.clear();

      /*
      ::cv::findContours(sb_img,polar_ctor_v,cv_hierarchy_v,
			 CV_RETR_EXTERNAL,
			 CV_CHAIN_APPROX_SIMPLE);
      */

      ::cv::findContours(rot_polarimg,
			 polar_ctor_v,
			 cv_hierarchy_v,
			 CV_RETR_EXTERNAL,
			 CV_CHAIN_APPROX_SIMPLE);

      LAROCV_DEBUG() << "Found " << polar_ctor_v.size() << " contours\n";
      // Find one contour that is closest to the vtx
      float min_radius=1e4;
      int   target_idx=-1;
      for(size_t polar_ctor_idx = 0; polar_ctor_idx < polar_ctor_v.size(); ++polar_ctor_idx) {
	auto const& polar_ctor = polar_ctor_v[polar_ctor_idx];
	LAROCV_DEBUG() << "Polar contour idx : " << polar_ctor_idx << " of size " << polar_ctor.size() << std::endl;
	for(auto const& pt : polar_ctor) {
	  float angle = pt.y / (float)(rot_polarimg.rows) * 360. - 180;
	  if(angle < -5 || angle > 5) continue;
	  if(pt.x > min_radius) continue;
	  min_radius = pt.x;
	  target_idx = polar_ctor_idx;
	}
	LAROCV_DEBUG() << "min_radius : " << min_radius << std::endl;
      }
      if(target_idx < 0) {
	LAROCV_DEBUG() << "No relevant contour find for this xs point" << std::endl;
	continue;
      }
      
      auto const& polar_contour = polar_ctor_v[target_idx];
      LAROCV_DEBUG() << "Chose polar contour at index : " << target_idx << " of size " << polar_contour.size() << std::endl;
      
      float rows = rot_polarimg.rows;
      float cols = rot_polarimg.cols;
      
      //cv::Mat polar_ctor_mat = cv::Mat(img_padded.rows,img_padded.cols,CV_8UC1,cvScalar(0.));
            
      //Cluster2D res_contour;
      //res_contour._contour.resize(polar_contour.size());
      larocv::data::ParticleCluster part;
      auto& contour = part._ctor;
      contour.reserve(polar_contour.size());

      //std::stringstream ss5;
      //ss3 << "Inserting into contour : ";
      ::geo2d::Vector<int> this_pt,last_pt;
      for (size_t pt_idx=0; pt_idx<polar_contour.size(); ++pt_idx) {
      	auto const& polar_pt = polar_contour[pt_idx];
	
	//polar_ctor_mat.at<unsigned char>(polar_pt.y,polar_pt.x) = (unsigned char) 255;
	
      	//up case to floating point
      	float r = polar_pt.x;
      	float t = polar_pt.y;
	
      	r = (r / cols) * max_radius;
      	t = ((t / rows) * 360.0 + angle) * M_PI / 180.0;
	
      	this_pt.x = (size_t)((float) r * std::cos(t) + ref_vtx_copy.x + 0.5);
      	this_pt.y = (size_t)((float) r * std::sin(t) + ref_vtx_copy.y + 0.5);
	
      	//res_contour._contour[pt_idx].x = (int)(pt.x + 0.5) - padding;
      	//res_contour._contour[pt_idx].y = (int)(pt.y + 0.5) - padding;

	this_pt.x -= padding;
	this_pt.y -= padding;
	if(pt_idx == 0 || this_pt.x != last_pt.x || this_pt.y != last_pt.y)
	  contour.push_back(this_pt);
	last_pt = this_pt;
	
	//ss5<<"[" << res_contour._contour[pt_idx].x << "," << res_contour._contour[pt_idx].y << "],";
      }
      //ss5<<std::endl;
      //LAROCV_DEBUG() << ss5.str();
      // cv::Mat cart_ctor_mat  = cv::Mat(img_padded.rows,img_padded.cols,CV_8UC1,cvScalar(0.));
      // for(auto& cpt : res_contour._contour)
      // 	cart_ctor_mat.at<unsigned char>(cpt.y,cpt.x) = (unsigned char) 255;

      // std::stringstream pp0;
      // pp0 << "cart_ctor_mat_"<<meta.plane()<<".png";
      // //cv::imwrite(pp0.str().c_str(),cart_ctor_mat);

      // std::stringstream pp1;
      // pp1 << "polar_ctor_mat_"<<meta.plane()<<".png";
      // //cv::imwrite(pp1.str().c_str(),polar_ctor_mat);

      //geo2d::UntanglePolygon(contour);
      
      result_v.emplace_back(std::move(part));
    }

    return result_v;
  }

  larocv::Cluster2DArray_t VertexTrackCluster::_Process_(const larocv::Cluster2DArray_t& clusters,
							 const ::cv::Mat& img,
							 larocv::ImageMeta& meta,
							 larocv::ROI& roi)
  {
    // Algorithm data
    auto& data = AlgoData<data::VertexClusterArray>();
    
    // if given from Refine2DVertex, fill
    if(_refine2d_algo_id!=kINVALID_ID && data._vtx_cluster_v.empty()) {

      auto const& ref_data = AlgoData<data::Refine2DVertexData>(_refine2d_algo_id);

      data._vtx_cluster_v.resize(ref_data.get_vertex().size());
      
      for(size_t vtx_id = 0; vtx_id < ref_data.get_vertex().size(); ++vtx_id) {

	auto const& vtx3d = ref_data.get_vertex()[vtx_id];
	auto const& circle_vtx_v = ref_data.get_circle_vertex(vtx3d.id());

	auto& vtx_cluster = data._vtx_cluster_v[vtx_id];
	vtx_cluster.set_vertex(vtx3d, circle_vtx_v);
      }
    }

    // Prepare image for analysis per vertex
    ::cv::Mat thresh_img;
    /* 
       // Using dilate/blur/threshold
       auto kernel = ::cv::getStructuringElement(cv::MORPH_ELLIPSE,::cv::Size(_dilation_size,_dilation_size));
       ::cv::dilate(img,thresh_img,kernel,::cv::Point(-1,-1),_dilation_iter);
       ::cv::blur(thresh_img,thresh_img,::cv::Size(_blur_size,_blur_size));
       ::cv::threshold(thresh_img, thresh_img, _pi_threshold, 1, CV_THRESH_BINARY);
    */
    // OR using just threshold
    ::cv::threshold(img, thresh_img, _pi_threshold, 1, CV_THRESH_BINARY);
    
    // Run clustering for this plane & store
    auto const plane = meta.plane();
    for(size_t vtx_id = 0; vtx_id < data._vtx_cluster_v.size(); ++vtx_id) {

      auto& vtx_cluster = data._vtx_cluster_v[vtx_id];
      auto const& circle_vtx = vtx_cluster.get_circle_vertex(plane);
      LAROCV_INFO() << "Vertex ID " << vtx_id << " plane " << plane
		    << " CircleVertex @ " << circle_vtx.center << " w/ R = " << circle_vtx.radius << std::endl;
      
      // Create track cluster hypothesis from the vertex point
      auto cluster_v = TrackHypothesis(img,circle_vtx);

      //
      // Analyze fraction of pixels clustered
      // 0) find a contour that contains the subject 2D vertex, and find allcontained non-zero pixels inside
      // 1) per track cluster count # of pixels from 0) that is contained inside
      GEO2D_ContourArray_t parent_ctor_v;
      std::vector<::cv::Vec4i> cv_hierarchy_v;
      ::cv::findContours(thresh_img, parent_ctor_v, cv_hierarchy_v, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
      size_t parent_ctor_id   = kINVALID_SIZE;
      size_t parent_ctor_size = 0;
      double dist2vtx = -1e9;
      for(size_t ctor_id=0; ctor_id < parent_ctor_v.size(); ++ctor_id){
	auto const& ctor = parent_ctor_v[ctor_id];
	auto dist = ::cv::pointPolygonTest(ctor, circle_vtx.center, false);
	if(dist < dist2vtx) continue;
	if(dist2vtx >=0 && parent_ctor_size > ctor.size()) continue;
	parent_ctor_id = ctor_id;
	parent_ctor_size = ctor.size();
	dist2vtx = dist;
      }
      geo2d::VectorArray<int> parent_points;
      if(parent_ctor_id != kINVALID_SIZE) {
	auto const& parent_ctor = parent_ctor_v[parent_ctor_id];
	geo2d::VectorArray<int> points;
	findNonZero(thresh_img, points);
	parent_points.reserve(points.size());
	for(auto const& pt : points) {
	  auto dist = ::cv::pointPolygonTest(parent_ctor,pt,false);
	  if(dist <0) continue;
	  parent_points.push_back(pt);
	}
      }
      vtx_cluster.set_num_pixel(plane,parent_points.size());

      LAROCV_INFO() << "# non-zero pixels " << parent_points.size()
		    << " ... # track clusters " << cluster_v.size()
		    << std::endl;
      
      for(size_t cidx=0; cidx<cluster_v.size(); ++cidx) {
	auto& cluster = cluster_v[cidx];
	// Find # pixels in this cluster from the parent
	size_t num_pixel = 0;
	for(auto const& pt : parent_points) {
	  auto dist = ::cv::pointPolygonTest(cluster._ctor,pt,false);
	  if(dist<0) continue;
	  num_pixel +=1;
	}
	cluster._num_pixel = num_pixel;
	vtx_cluster.emplace_back(plane,std::move(cluster));
      }
    }


    

    // If only 1 cluster, return that cluster
    // NOT IMPLEMENTED YET

    return clusters;
  }

}

#endif
