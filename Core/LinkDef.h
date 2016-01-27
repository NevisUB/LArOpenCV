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
#pragma link C++ namespace larcv+;
#pragma link C++ namespace larcv::msg;
#pragma link C++ class larcv::larbys+;
#pragma link C++ class larcv::logger+;
#pragma link C++ class larcv::laropencv_base+;
#pragma link C++ class larcv::Point2D+;
#pragma link C++ class larcv::Point2DArray+;
#pragma link C++ class larcv::ImageMeta+;
#pragma link C++ class larcv::Image2D+;
#pragma link C++ class larcv::ImageManager+;
//ADD_NEW_CLASS ... do not change this line
#endif



