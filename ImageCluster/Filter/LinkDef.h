//
// cint script to generate libraries
// Declaire namespace & classes you defined
// #pragma statement: order matters! Google it ;)
//

#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

//#pragma link C++ class larocv::PiZeroFilter+;
#pragma link C++ class larocv::PiZeroFilterV+;
#pragma link C++ class larocv::CheckStartDistance+;
#pragma link C++ class larocv::SimpleCuts+;
#pragma link C++ class larocv::SelectTwoPhotons+;

#pragma link C++ class larocv::AttachedClusters+;		
#pragma link C++ class larocv::RecoROIFilter+;		
#pragma link C++ class larocv::ROIParameters+;		
#pragma link C++ class larocv::RecoParameters+;

#pragma link C++ class larocv::HoughPTrackShower+;
#pragma link C++ class larocv::FilterROI+;
#pragma link C++ class larocv::VertexInHull+;
#pragma link C++ class larocv::CheckWires+;
#pragma link C++ class larocv::ClusDeadWireOverlap+;

#pragma link C++ class larocv::InertiaTrackShower+;

#endif

