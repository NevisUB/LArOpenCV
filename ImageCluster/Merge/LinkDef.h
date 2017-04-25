//
// cint script to generate libraries
// Declaire namespace & classes you defined
// #pragma statement: order matters! Google it ;)
//

#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class larocv::StartEndMerge+;
#pragma link C++ class larocv::InConeCluster+;
#pragma link C++ class larocv::DeadWireCombine+;		
#pragma link C++ class larocv::NearestConeCluster+;
#pragma link C++ class larocv::SatelliteMerge+;
#pragma link C++ class larocv::FlashlightMerge+;
#pragma link C++ class larocv::PolarMerge+;
#pragma link C++ class larocv::VtxAlignedMerge+;

//ADD_NEW_CLASS ... do not change this line
#endif



