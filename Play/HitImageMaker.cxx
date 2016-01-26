#ifndef LARLITE_HITIMAGEMAKER_CXX
#define LARLITE_HITIMAGEMAKER_CXX

#include "HitImageMaker.h"
#include "Utils/NDArrayConverter.h"
#include "DataFormat/hit.h"
namespace larlite {

  HitImageMaker::HitImageMaker()
  { _name="HitImageMaker"; _fout=0;  init(); }

  void HitImageMaker::init()
  { import_array(); }

  bool HitImageMaker::initialize() {
    return true;
  }
  
  bool HitImageMaker::analyze(storage_manager* storage) {

    auto const ev_hit = storage->get_data<event_hit>("gaushit");

    _mat_v.clear();
    _canny_v.clear();
    _contour_v.clear();
    
    std::vector<int> x_min_v;
    std::vector<int> x_max_v;
    std::vector<int> y_min_v;
    std::vector<int> y_max_v;
    std::vector<float> q_max_v;
    for(auto const& h : *ev_hit) {

      if(h.Integral()<5.) continue;
      size_t plane = h.WireID().Plane;
      if(plane >= x_min_v.size()) {

	x_min_v.resize(plane+1,4000);
	x_max_v.resize(plane+1,0);
	y_min_v.resize(plane+1,9600);
	y_max_v.resize(plane+1,0);
	q_max_v.resize(plane+1,0);
      }

      //std::cout<<plane<< " : " <<h.Integral()<<std::endl;

      int wire = h.WireID().Wire;
      int time = (int)(h.PeakTime());
      float  q = h.Integral();
      if( x_min_v[plane] > wire ) x_min_v[plane] = wire;
      if( x_max_v[plane] < wire ) x_max_v[plane] = wire;
      if( y_min_v[plane] > time ) y_min_v[plane] = time;
      if( y_max_v[plane] < time ) y_max_v[plane] = time;
      if( q_max_v[plane] < q    ) q_max_v[plane] = q;
    }

    for(size_t plane=0; plane<x_min_v.size(); ++plane) {
      std::cout<<"Plane "<<plane<<" : "
	       <<x_min_v[plane]<<" => "<<x_max_v[plane]<< " : "
	       <<y_min_v[plane]<<" => "<<y_max_v[plane]<< std::endl;

      ::cv::Mat mat(x_max_v[plane] - x_min_v[plane] + 1,
		    y_max_v[plane] - y_min_v[plane] + 1,
		    CV_8UC1, cvScalar(0.));

      ::cv::Mat canny;
      canny.create(mat.size(),mat.type());
      
      _mat_v.emplace_back(mat);
      _canny_v.emplace_back(canny);
    }
    _contour_v.resize(x_min_v.size());

    for(auto const& h : *ev_hit) {

      if(h.Integral()<5.) continue;
      int wire = h.WireID().Wire;
      int time = (int)(h.PeakTime());
      size_t plane = h.WireID().Plane;
      int charge = ((256. * h.Integral() / q_max_v[plane]));
      wire -= x_min_v[plane];
      time -= y_min_v[plane];
      auto& mat = _mat_v[plane];

      charge += mat.at<unsigned char>(wire,time);

      if(charge>256) charge = 256;
      mat.at<unsigned char>(wire,time) = (unsigned char)(charge);
	
    }

    for(size_t plane=0; plane<_mat_v.size(); ++plane) {

      ::cv::blur( _mat_v[plane],
		  _mat_v[plane],
		  ::cv::Size(3,3) );

      /// Canny detector
      //Canny( detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size );
      
      ::cv::Canny(_mat_v[plane],_canny_v[plane],10,100,3);

      std::vector<std::vector<cv::Point> > cv_contour_v;
      std::vector<cv::Vec4i> cv_hierarchy_v;
      //::cv::findContours(_canny_v[plane],cv_contour_v,cv_hierarchy_v,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);
      ::cv::findContours(_canny_v[plane],cv_contour_v,cv_hierarchy_v,
			 CV_RETR_EXTERNAL,
			 CV_CHAIN_APPROX_SIMPLE);

      std::cout<<"Found "<<cv_contour_v.size()<<" contours..."<<std::endl;

      auto& plane_contour_v = _contour_v[plane];
      plane_contour_v.resize( cv_contour_v.size() );

      for(size_t c_index=0; c_index<cv_contour_v.size(); ++c_index) {

	auto& cv_contour  = cv_contour_v[c_index];
	auto& out_contour = plane_contour_v[c_index];

	out_contour.resize(cv_contour.size());
	for(size_t p_index=0; p_index<cv_contour.size(); ++p_index)
	  out_contour.set(p_index, cv_contour[p_index].x, cv_contour[p_index].y);
	
      }
    }

    //::cv::namedWindow( "Display window", ::cv::WINDOW_AUTOSIZE );// Create a window for display.
    //::cv::imshow( "Display window", _mat_v.back() );                   // Show our image inside it.
    
    return true;
  }

  bool HitImageMaker::finalize() {

    return true;
  }

  PyObject* HitImageMaker::GetImage(const size_t plane)
  {
    if(plane >= _mat_v.size()) {
      print(msg::kERROR,__FUNCTION__,"Invalid plane ID requested...");
      throw std::exception();
    }

    ::larcv::convert::NDArrayConverter converter;
    return converter.toNDArray(_mat_v[plane]);
      
  }

  PyObject* HitImageMaker::GetCanny(const size_t plane)
  {
    if(plane >= _mat_v.size()) {
      print(msg::kERROR,__FUNCTION__,"Invalid plane ID requested...");
      throw std::exception();
    }

    ::larcv::convert::NDArrayConverter converter;
    return converter.toNDArray(_canny_v[plane]);
      
  }

  size_t HitImageMaker::NumContours(const size_t plane) const
  {
    if(plane >= _mat_v.size()) {
      print(msg::kERROR,__FUNCTION__,"Invalid plane ID requested...");
      throw std::exception();
    }
    return _contour_v[plane].size();
  }

  PyObject* HitImageMaker::GetContour(const size_t plane, const size_t contour_index)
  {
    if(plane >= _mat_v.size()) {
      print(msg::kERROR,__FUNCTION__,"Invalid plane ID requested...");
      throw std::exception();
    }

    if(contour_index > _contour_v[plane].size()){
      print(msg::kERROR,__FUNCTION__,"Invalid contour ID requested...");
      throw std::exception();
    }
    
    int* dim_data = new int[2];
    dim_data[0] = 2;
    dim_data[1] = (int)(_contour_v[plane][contour_index].size());

    return PyArray_FromDimsAndData( 2, dim_data, NPY_DOUBLE, (char*) &(_contour_v[plane][contour_index].raw_data()[0]) );
				    
  }

}
#endif
