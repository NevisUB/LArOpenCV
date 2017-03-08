//
// cint script to generate libraries
// Declaire namespace & classes you defined
// #pragma statement: order matters! Google it ;)
//

#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ namespace larcaffe+;
#pragma link C++ namespace larcaffe::msg+;
#pragma link C++ class larcaffe::larbys+;
#pragma link C++ namespace larocv+;
#pragma link C++ namespace larocv::msg;
#pragma link C++ class larocv::larbys+;
#pragma link C++ class larocv::logger+;
#pragma link C++ class std::map<std::string,larocv::logger>+;
#pragma link C++ class larocv::laropencv_base+;
#pragma link C++ class larocv::Point2D+;
#pragma link C++ class larocv::Point2DArray+;
#pragma link C++ class larocv::ImageMeta+;
#pragma link C++ class larocv::Image2D+;
#pragma link C++ class larocv::ImageManager+;
#pragma link C++ class larocv::ROI+;
//ADD_NEW_CLASS ... do not change this line
#endif



