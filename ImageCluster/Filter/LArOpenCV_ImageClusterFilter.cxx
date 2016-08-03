// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME LArOpenCV_ImageClusterFilter

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "RConfig.h"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// Since CINT ignores the std namespace, we need to do so in this file.
namespace std {} using namespace std;

// Header files passed as explicit arguments
#include "AttachedClusters.h"
#include "CheckStartDistance.h"
#include "PiZeroFilter.h"
#include "PiZeroFilterV.h"
#include "ROIParameters.h"
#include "RecoParameters.h"
#include "RecoROIFilter.h"
#include "SimpleCuts.h"
#include "TrackShower.h"

// Header files passed via #pragma extra_include

namespace ROOT {
   static TClass *larocvcLcLPiZeroFilter_Dictionary();
   static void larocvcLcLPiZeroFilter_TClassManip(TClass*);
   static void *new_larocvcLcLPiZeroFilter(void *p = 0);
   static void *newArray_larocvcLcLPiZeroFilter(Long_t size, void *p);
   static void delete_larocvcLcLPiZeroFilter(void *p);
   static void deleteArray_larocvcLcLPiZeroFilter(void *p);
   static void destruct_larocvcLcLPiZeroFilter(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::larocv::PiZeroFilter*)
   {
      ::larocv::PiZeroFilter *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::larocv::PiZeroFilter));
      static ::ROOT::TGenericClassInfo 
         instance("larocv::PiZeroFilter", "PiZeroFilter.h", 25,
                  typeid(::larocv::PiZeroFilter), DefineBehavior(ptr, ptr),
                  &larocvcLcLPiZeroFilter_Dictionary, isa_proxy, 4,
                  sizeof(::larocv::PiZeroFilter) );
      instance.SetNew(&new_larocvcLcLPiZeroFilter);
      instance.SetNewArray(&newArray_larocvcLcLPiZeroFilter);
      instance.SetDelete(&delete_larocvcLcLPiZeroFilter);
      instance.SetDeleteArray(&deleteArray_larocvcLcLPiZeroFilter);
      instance.SetDestructor(&destruct_larocvcLcLPiZeroFilter);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::larocv::PiZeroFilter*)
   {
      return GenerateInitInstanceLocal((::larocv::PiZeroFilter*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::larocv::PiZeroFilter*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *larocvcLcLPiZeroFilter_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::larocv::PiZeroFilter*)0x0)->GetClass();
      larocvcLcLPiZeroFilter_TClassManip(theClass);
   return theClass;
   }

   static void larocvcLcLPiZeroFilter_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *larocvcLcLPiZeroFilterV_Dictionary();
   static void larocvcLcLPiZeroFilterV_TClassManip(TClass*);
   static void *new_larocvcLcLPiZeroFilterV(void *p = 0);
   static void *newArray_larocvcLcLPiZeroFilterV(Long_t size, void *p);
   static void delete_larocvcLcLPiZeroFilterV(void *p);
   static void deleteArray_larocvcLcLPiZeroFilterV(void *p);
   static void destruct_larocvcLcLPiZeroFilterV(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::larocv::PiZeroFilterV*)
   {
      ::larocv::PiZeroFilterV *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::larocv::PiZeroFilterV));
      static ::ROOT::TGenericClassInfo 
         instance("larocv::PiZeroFilterV", "PiZeroFilterV.h", 14,
                  typeid(::larocv::PiZeroFilterV), DefineBehavior(ptr, ptr),
                  &larocvcLcLPiZeroFilterV_Dictionary, isa_proxy, 4,
                  sizeof(::larocv::PiZeroFilterV) );
      instance.SetNew(&new_larocvcLcLPiZeroFilterV);
      instance.SetNewArray(&newArray_larocvcLcLPiZeroFilterV);
      instance.SetDelete(&delete_larocvcLcLPiZeroFilterV);
      instance.SetDeleteArray(&deleteArray_larocvcLcLPiZeroFilterV);
      instance.SetDestructor(&destruct_larocvcLcLPiZeroFilterV);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::larocv::PiZeroFilterV*)
   {
      return GenerateInitInstanceLocal((::larocv::PiZeroFilterV*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::larocv::PiZeroFilterV*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *larocvcLcLPiZeroFilterV_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::larocv::PiZeroFilterV*)0x0)->GetClass();
      larocvcLcLPiZeroFilterV_TClassManip(theClass);
   return theClass;
   }

   static void larocvcLcLPiZeroFilterV_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *larocvcLcLCheckStartDistance_Dictionary();
   static void larocvcLcLCheckStartDistance_TClassManip(TClass*);
   static void *new_larocvcLcLCheckStartDistance(void *p = 0);
   static void *newArray_larocvcLcLCheckStartDistance(Long_t size, void *p);
   static void delete_larocvcLcLCheckStartDistance(void *p);
   static void deleteArray_larocvcLcLCheckStartDistance(void *p);
   static void destruct_larocvcLcLCheckStartDistance(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::larocv::CheckStartDistance*)
   {
      ::larocv::CheckStartDistance *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::larocv::CheckStartDistance));
      static ::ROOT::TGenericClassInfo 
         instance("larocv::CheckStartDistance", "CheckStartDistance.h", 25,
                  typeid(::larocv::CheckStartDistance), DefineBehavior(ptr, ptr),
                  &larocvcLcLCheckStartDistance_Dictionary, isa_proxy, 4,
                  sizeof(::larocv::CheckStartDistance) );
      instance.SetNew(&new_larocvcLcLCheckStartDistance);
      instance.SetNewArray(&newArray_larocvcLcLCheckStartDistance);
      instance.SetDelete(&delete_larocvcLcLCheckStartDistance);
      instance.SetDeleteArray(&deleteArray_larocvcLcLCheckStartDistance);
      instance.SetDestructor(&destruct_larocvcLcLCheckStartDistance);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::larocv::CheckStartDistance*)
   {
      return GenerateInitInstanceLocal((::larocv::CheckStartDistance*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::larocv::CheckStartDistance*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *larocvcLcLCheckStartDistance_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::larocv::CheckStartDistance*)0x0)->GetClass();
      larocvcLcLCheckStartDistance_TClassManip(theClass);
   return theClass;
   }

   static void larocvcLcLCheckStartDistance_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *larocvcLcLSimpleCuts_Dictionary();
   static void larocvcLcLSimpleCuts_TClassManip(TClass*);
   static void *new_larocvcLcLSimpleCuts(void *p = 0);
   static void *newArray_larocvcLcLSimpleCuts(Long_t size, void *p);
   static void delete_larocvcLcLSimpleCuts(void *p);
   static void deleteArray_larocvcLcLSimpleCuts(void *p);
   static void destruct_larocvcLcLSimpleCuts(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::larocv::SimpleCuts*)
   {
      ::larocv::SimpleCuts *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::larocv::SimpleCuts));
      static ::ROOT::TGenericClassInfo 
         instance("larocv::SimpleCuts", "SimpleCuts.h", 25,
                  typeid(::larocv::SimpleCuts), DefineBehavior(ptr, ptr),
                  &larocvcLcLSimpleCuts_Dictionary, isa_proxy, 4,
                  sizeof(::larocv::SimpleCuts) );
      instance.SetNew(&new_larocvcLcLSimpleCuts);
      instance.SetNewArray(&newArray_larocvcLcLSimpleCuts);
      instance.SetDelete(&delete_larocvcLcLSimpleCuts);
      instance.SetDeleteArray(&deleteArray_larocvcLcLSimpleCuts);
      instance.SetDestructor(&destruct_larocvcLcLSimpleCuts);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::larocv::SimpleCuts*)
   {
      return GenerateInitInstanceLocal((::larocv::SimpleCuts*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::larocv::SimpleCuts*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *larocvcLcLSimpleCuts_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::larocv::SimpleCuts*)0x0)->GetClass();
      larocvcLcLSimpleCuts_TClassManip(theClass);
   return theClass;
   }

   static void larocvcLcLSimpleCuts_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *larocvcLcLAttachedClusters_Dictionary();
   static void larocvcLcLAttachedClusters_TClassManip(TClass*);
   static void *new_larocvcLcLAttachedClusters(void *p = 0);
   static void *newArray_larocvcLcLAttachedClusters(Long_t size, void *p);
   static void delete_larocvcLcLAttachedClusters(void *p);
   static void deleteArray_larocvcLcLAttachedClusters(void *p);
   static void destruct_larocvcLcLAttachedClusters(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::larocv::AttachedClusters*)
   {
      ::larocv::AttachedClusters *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::larocv::AttachedClusters));
      static ::ROOT::TGenericClassInfo 
         instance("larocv::AttachedClusters", "AttachedClusters.h", 25,
                  typeid(::larocv::AttachedClusters), DefineBehavior(ptr, ptr),
                  &larocvcLcLAttachedClusters_Dictionary, isa_proxy, 4,
                  sizeof(::larocv::AttachedClusters) );
      instance.SetNew(&new_larocvcLcLAttachedClusters);
      instance.SetNewArray(&newArray_larocvcLcLAttachedClusters);
      instance.SetDelete(&delete_larocvcLcLAttachedClusters);
      instance.SetDeleteArray(&deleteArray_larocvcLcLAttachedClusters);
      instance.SetDestructor(&destruct_larocvcLcLAttachedClusters);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::larocv::AttachedClusters*)
   {
      return GenerateInitInstanceLocal((::larocv::AttachedClusters*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::larocv::AttachedClusters*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *larocvcLcLAttachedClusters_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::larocv::AttachedClusters*)0x0)->GetClass();
      larocvcLcLAttachedClusters_TClassManip(theClass);
   return theClass;
   }

   static void larocvcLcLAttachedClusters_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *larocvcLcLRecoROIFilter_Dictionary();
   static void larocvcLcLRecoROIFilter_TClassManip(TClass*);
   static void *new_larocvcLcLRecoROIFilter(void *p = 0);
   static void *newArray_larocvcLcLRecoROIFilter(Long_t size, void *p);
   static void delete_larocvcLcLRecoROIFilter(void *p);
   static void deleteArray_larocvcLcLRecoROIFilter(void *p);
   static void destruct_larocvcLcLRecoROIFilter(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::larocv::RecoROIFilter*)
   {
      ::larocv::RecoROIFilter *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::larocv::RecoROIFilter));
      static ::ROOT::TGenericClassInfo 
         instance("larocv::RecoROIFilter", "RecoROIFilter.h", 25,
                  typeid(::larocv::RecoROIFilter), DefineBehavior(ptr, ptr),
                  &larocvcLcLRecoROIFilter_Dictionary, isa_proxy, 4,
                  sizeof(::larocv::RecoROIFilter) );
      instance.SetNew(&new_larocvcLcLRecoROIFilter);
      instance.SetNewArray(&newArray_larocvcLcLRecoROIFilter);
      instance.SetDelete(&delete_larocvcLcLRecoROIFilter);
      instance.SetDeleteArray(&deleteArray_larocvcLcLRecoROIFilter);
      instance.SetDestructor(&destruct_larocvcLcLRecoROIFilter);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::larocv::RecoROIFilter*)
   {
      return GenerateInitInstanceLocal((::larocv::RecoROIFilter*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::larocv::RecoROIFilter*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *larocvcLcLRecoROIFilter_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::larocv::RecoROIFilter*)0x0)->GetClass();
      larocvcLcLRecoROIFilter_TClassManip(theClass);
   return theClass;
   }

   static void larocvcLcLRecoROIFilter_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *larocvcLcLROIParameters_Dictionary();
   static void larocvcLcLROIParameters_TClassManip(TClass*);
   static void *new_larocvcLcLROIParameters(void *p = 0);
   static void *newArray_larocvcLcLROIParameters(Long_t size, void *p);
   static void delete_larocvcLcLROIParameters(void *p);
   static void deleteArray_larocvcLcLROIParameters(void *p);
   static void destruct_larocvcLcLROIParameters(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::larocv::ROIParameters*)
   {
      ::larocv::ROIParameters *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::larocv::ROIParameters));
      static ::ROOT::TGenericClassInfo 
         instance("larocv::ROIParameters", "ROIParameters.h", 25,
                  typeid(::larocv::ROIParameters), DefineBehavior(ptr, ptr),
                  &larocvcLcLROIParameters_Dictionary, isa_proxy, 4,
                  sizeof(::larocv::ROIParameters) );
      instance.SetNew(&new_larocvcLcLROIParameters);
      instance.SetNewArray(&newArray_larocvcLcLROIParameters);
      instance.SetDelete(&delete_larocvcLcLROIParameters);
      instance.SetDeleteArray(&deleteArray_larocvcLcLROIParameters);
      instance.SetDestructor(&destruct_larocvcLcLROIParameters);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::larocv::ROIParameters*)
   {
      return GenerateInitInstanceLocal((::larocv::ROIParameters*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::larocv::ROIParameters*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *larocvcLcLROIParameters_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::larocv::ROIParameters*)0x0)->GetClass();
      larocvcLcLROIParameters_TClassManip(theClass);
   return theClass;
   }

   static void larocvcLcLROIParameters_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *larocvcLcLRecoParameters_Dictionary();
   static void larocvcLcLRecoParameters_TClassManip(TClass*);
   static void *new_larocvcLcLRecoParameters(void *p = 0);
   static void *newArray_larocvcLcLRecoParameters(Long_t size, void *p);
   static void delete_larocvcLcLRecoParameters(void *p);
   static void deleteArray_larocvcLcLRecoParameters(void *p);
   static void destruct_larocvcLcLRecoParameters(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::larocv::RecoParameters*)
   {
      ::larocv::RecoParameters *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::larocv::RecoParameters));
      static ::ROOT::TGenericClassInfo 
         instance("larocv::RecoParameters", "RecoParameters.h", 25,
                  typeid(::larocv::RecoParameters), DefineBehavior(ptr, ptr),
                  &larocvcLcLRecoParameters_Dictionary, isa_proxy, 4,
                  sizeof(::larocv::RecoParameters) );
      instance.SetNew(&new_larocvcLcLRecoParameters);
      instance.SetNewArray(&newArray_larocvcLcLRecoParameters);
      instance.SetDelete(&delete_larocvcLcLRecoParameters);
      instance.SetDeleteArray(&deleteArray_larocvcLcLRecoParameters);
      instance.SetDestructor(&destruct_larocvcLcLRecoParameters);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::larocv::RecoParameters*)
   {
      return GenerateInitInstanceLocal((::larocv::RecoParameters*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::larocv::RecoParameters*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *larocvcLcLRecoParameters_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::larocv::RecoParameters*)0x0)->GetClass();
      larocvcLcLRecoParameters_TClassManip(theClass);
   return theClass;
   }

   static void larocvcLcLRecoParameters_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_larocvcLcLPiZeroFilter(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::PiZeroFilter : new ::larocv::PiZeroFilter;
   }
   static void *newArray_larocvcLcLPiZeroFilter(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::PiZeroFilter[nElements] : new ::larocv::PiZeroFilter[nElements];
   }
   // Wrapper around operator delete
   static void delete_larocvcLcLPiZeroFilter(void *p) {
      delete ((::larocv::PiZeroFilter*)p);
   }
   static void deleteArray_larocvcLcLPiZeroFilter(void *p) {
      delete [] ((::larocv::PiZeroFilter*)p);
   }
   static void destruct_larocvcLcLPiZeroFilter(void *p) {
      typedef ::larocv::PiZeroFilter current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::larocv::PiZeroFilter

namespace ROOT {
   // Wrappers around operator new
   static void *new_larocvcLcLPiZeroFilterV(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::PiZeroFilterV : new ::larocv::PiZeroFilterV;
   }
   static void *newArray_larocvcLcLPiZeroFilterV(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::PiZeroFilterV[nElements] : new ::larocv::PiZeroFilterV[nElements];
   }
   // Wrapper around operator delete
   static void delete_larocvcLcLPiZeroFilterV(void *p) {
      delete ((::larocv::PiZeroFilterV*)p);
   }
   static void deleteArray_larocvcLcLPiZeroFilterV(void *p) {
      delete [] ((::larocv::PiZeroFilterV*)p);
   }
   static void destruct_larocvcLcLPiZeroFilterV(void *p) {
      typedef ::larocv::PiZeroFilterV current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::larocv::PiZeroFilterV

namespace ROOT {
   // Wrappers around operator new
   static void *new_larocvcLcLCheckStartDistance(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::CheckStartDistance : new ::larocv::CheckStartDistance;
   }
   static void *newArray_larocvcLcLCheckStartDistance(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::CheckStartDistance[nElements] : new ::larocv::CheckStartDistance[nElements];
   }
   // Wrapper around operator delete
   static void delete_larocvcLcLCheckStartDistance(void *p) {
      delete ((::larocv::CheckStartDistance*)p);
   }
   static void deleteArray_larocvcLcLCheckStartDistance(void *p) {
      delete [] ((::larocv::CheckStartDistance*)p);
   }
   static void destruct_larocvcLcLCheckStartDistance(void *p) {
      typedef ::larocv::CheckStartDistance current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::larocv::CheckStartDistance

namespace ROOT {
   // Wrappers around operator new
   static void *new_larocvcLcLSimpleCuts(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::SimpleCuts : new ::larocv::SimpleCuts;
   }
   static void *newArray_larocvcLcLSimpleCuts(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::SimpleCuts[nElements] : new ::larocv::SimpleCuts[nElements];
   }
   // Wrapper around operator delete
   static void delete_larocvcLcLSimpleCuts(void *p) {
      delete ((::larocv::SimpleCuts*)p);
   }
   static void deleteArray_larocvcLcLSimpleCuts(void *p) {
      delete [] ((::larocv::SimpleCuts*)p);
   }
   static void destruct_larocvcLcLSimpleCuts(void *p) {
      typedef ::larocv::SimpleCuts current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::larocv::SimpleCuts

namespace ROOT {
   // Wrappers around operator new
   static void *new_larocvcLcLAttachedClusters(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::AttachedClusters : new ::larocv::AttachedClusters;
   }
   static void *newArray_larocvcLcLAttachedClusters(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::AttachedClusters[nElements] : new ::larocv::AttachedClusters[nElements];
   }
   // Wrapper around operator delete
   static void delete_larocvcLcLAttachedClusters(void *p) {
      delete ((::larocv::AttachedClusters*)p);
   }
   static void deleteArray_larocvcLcLAttachedClusters(void *p) {
      delete [] ((::larocv::AttachedClusters*)p);
   }
   static void destruct_larocvcLcLAttachedClusters(void *p) {
      typedef ::larocv::AttachedClusters current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::larocv::AttachedClusters

namespace ROOT {
   // Wrappers around operator new
   static void *new_larocvcLcLRecoROIFilter(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::RecoROIFilter : new ::larocv::RecoROIFilter;
   }
   static void *newArray_larocvcLcLRecoROIFilter(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::RecoROIFilter[nElements] : new ::larocv::RecoROIFilter[nElements];
   }
   // Wrapper around operator delete
   static void delete_larocvcLcLRecoROIFilter(void *p) {
      delete ((::larocv::RecoROIFilter*)p);
   }
   static void deleteArray_larocvcLcLRecoROIFilter(void *p) {
      delete [] ((::larocv::RecoROIFilter*)p);
   }
   static void destruct_larocvcLcLRecoROIFilter(void *p) {
      typedef ::larocv::RecoROIFilter current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::larocv::RecoROIFilter

namespace ROOT {
   // Wrappers around operator new
   static void *new_larocvcLcLROIParameters(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::ROIParameters : new ::larocv::ROIParameters;
   }
   static void *newArray_larocvcLcLROIParameters(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::ROIParameters[nElements] : new ::larocv::ROIParameters[nElements];
   }
   // Wrapper around operator delete
   static void delete_larocvcLcLROIParameters(void *p) {
      delete ((::larocv::ROIParameters*)p);
   }
   static void deleteArray_larocvcLcLROIParameters(void *p) {
      delete [] ((::larocv::ROIParameters*)p);
   }
   static void destruct_larocvcLcLROIParameters(void *p) {
      typedef ::larocv::ROIParameters current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::larocv::ROIParameters

namespace ROOT {
   // Wrappers around operator new
   static void *new_larocvcLcLRecoParameters(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::RecoParameters : new ::larocv::RecoParameters;
   }
   static void *newArray_larocvcLcLRecoParameters(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::RecoParameters[nElements] : new ::larocv::RecoParameters[nElements];
   }
   // Wrapper around operator delete
   static void delete_larocvcLcLRecoParameters(void *p) {
      delete ((::larocv::RecoParameters*)p);
   }
   static void deleteArray_larocvcLcLRecoParameters(void *p) {
      delete [] ((::larocv::RecoParameters*)p);
   }
   static void destruct_larocvcLcLRecoParameters(void *p) {
      typedef ::larocv::RecoParameters current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::larocv::RecoParameters

namespace ROOT {
   static TClass *vectorlEvectorlElarocvcLcLVertex2DgRsPgR_Dictionary();
   static void vectorlEvectorlElarocvcLcLVertex2DgRsPgR_TClassManip(TClass*);
   static void *new_vectorlEvectorlElarocvcLcLVertex2DgRsPgR(void *p = 0);
   static void *newArray_vectorlEvectorlElarocvcLcLVertex2DgRsPgR(Long_t size, void *p);
   static void delete_vectorlEvectorlElarocvcLcLVertex2DgRsPgR(void *p);
   static void deleteArray_vectorlEvectorlElarocvcLcLVertex2DgRsPgR(void *p);
   static void destruct_vectorlEvectorlElarocvcLcLVertex2DgRsPgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<vector<larocv::Vertex2D> >*)
   {
      vector<vector<larocv::Vertex2D> > *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<vector<larocv::Vertex2D> >));
      static ::ROOT::TGenericClassInfo 
         instance("vector<vector<larocv::Vertex2D> >", -2, "vector", 457,
                  typeid(vector<vector<larocv::Vertex2D> >), DefineBehavior(ptr, ptr),
                  &vectorlEvectorlElarocvcLcLVertex2DgRsPgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<vector<larocv::Vertex2D> >) );
      instance.SetNew(&new_vectorlEvectorlElarocvcLcLVertex2DgRsPgR);
      instance.SetNewArray(&newArray_vectorlEvectorlElarocvcLcLVertex2DgRsPgR);
      instance.SetDelete(&delete_vectorlEvectorlElarocvcLcLVertex2DgRsPgR);
      instance.SetDeleteArray(&deleteArray_vectorlEvectorlElarocvcLcLVertex2DgRsPgR);
      instance.SetDestructor(&destruct_vectorlEvectorlElarocvcLcLVertex2DgRsPgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<vector<larocv::Vertex2D> > >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const vector<vector<larocv::Vertex2D> >*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEvectorlElarocvcLcLVertex2DgRsPgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<vector<larocv::Vertex2D> >*)0x0)->GetClass();
      vectorlEvectorlElarocvcLcLVertex2DgRsPgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEvectorlElarocvcLcLVertex2DgRsPgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEvectorlElarocvcLcLVertex2DgRsPgR(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<vector<larocv::Vertex2D> > : new vector<vector<larocv::Vertex2D> >;
   }
   static void *newArray_vectorlEvectorlElarocvcLcLVertex2DgRsPgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<vector<larocv::Vertex2D> >[nElements] : new vector<vector<larocv::Vertex2D> >[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEvectorlElarocvcLcLVertex2DgRsPgR(void *p) {
      delete ((vector<vector<larocv::Vertex2D> >*)p);
   }
   static void deleteArray_vectorlEvectorlElarocvcLcLVertex2DgRsPgR(void *p) {
      delete [] ((vector<vector<larocv::Vertex2D> >*)p);
   }
   static void destruct_vectorlEvectorlElarocvcLcLVertex2DgRsPgR(void *p) {
      typedef vector<vector<larocv::Vertex2D> > current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<vector<larocv::Vertex2D> >

namespace ROOT {
   static TClass *vectorlElarocvcLcLVertex2DgR_Dictionary();
   static void vectorlElarocvcLcLVertex2DgR_TClassManip(TClass*);
   static void *new_vectorlElarocvcLcLVertex2DgR(void *p = 0);
   static void *newArray_vectorlElarocvcLcLVertex2DgR(Long_t size, void *p);
   static void delete_vectorlElarocvcLcLVertex2DgR(void *p);
   static void deleteArray_vectorlElarocvcLcLVertex2DgR(void *p);
   static void destruct_vectorlElarocvcLcLVertex2DgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<larocv::Vertex2D>*)
   {
      vector<larocv::Vertex2D> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<larocv::Vertex2D>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<larocv::Vertex2D>", -2, "vector", 457,
                  typeid(vector<larocv::Vertex2D>), DefineBehavior(ptr, ptr),
                  &vectorlElarocvcLcLVertex2DgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<larocv::Vertex2D>) );
      instance.SetNew(&new_vectorlElarocvcLcLVertex2DgR);
      instance.SetNewArray(&newArray_vectorlElarocvcLcLVertex2DgR);
      instance.SetDelete(&delete_vectorlElarocvcLcLVertex2DgR);
      instance.SetDeleteArray(&deleteArray_vectorlElarocvcLcLVertex2DgR);
      instance.SetDestructor(&destruct_vectorlElarocvcLcLVertex2DgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<larocv::Vertex2D> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const vector<larocv::Vertex2D>*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlElarocvcLcLVertex2DgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<larocv::Vertex2D>*)0x0)->GetClass();
      vectorlElarocvcLcLVertex2DgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlElarocvcLcLVertex2DgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlElarocvcLcLVertex2DgR(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<larocv::Vertex2D> : new vector<larocv::Vertex2D>;
   }
   static void *newArray_vectorlElarocvcLcLVertex2DgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<larocv::Vertex2D>[nElements] : new vector<larocv::Vertex2D>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlElarocvcLcLVertex2DgR(void *p) {
      delete ((vector<larocv::Vertex2D>*)p);
   }
   static void deleteArray_vectorlElarocvcLcLVertex2DgR(void *p) {
      delete [] ((vector<larocv::Vertex2D>*)p);
   }
   static void destruct_vectorlElarocvcLcLVertex2DgR(void *p) {
      typedef vector<larocv::Vertex2D> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<larocv::Vertex2D>

namespace {
  void TriggerDictionaryInitialization_libLArOpenCV_ImageClusterFilter_Impl() {
    static const char* headers[] = {
"AttachedClusters.h",
"CheckStartDistance.h",
"PiZeroFilter.h",
"PiZeroFilterV.h",
"ROIParameters.h",
"RecoParameters.h",
"RecoROIFilter.h",
"SimpleCuts.h",
"TrackShower.h",
0
    };
    static const char* includePaths[] = {
"/Users/vgenty/sw/larlite/UserDev/BasicTool",
"/Users/vgenty/sw/larlite/UserDev/RecoTool",
"/Users/vgenty/sw/larlite/UserDev/RecoTool/FANN/include",
"/Users/vgenty/sw/larlite/core",
"/System/Library/Frameworks/Python.framework/Versions/2.7/include/python2.7",
"/System/Library/Frameworks/Python.framework/Versions/2.7/include/python2.7",
"/Library/Python/2.7/site-packages/numpy/core/include",
"/usr/local/include",
"/Users/vgenty/sw/laropencv",
"/Users/vgenty/sw/laropencv/ImageCluster/Base",
"/Users/vgenty/sw/laropencv/ImageCluster/Util",
"/usr/local/ROOT//include/root",
"/Users/vgenty/sw/laropencv/ImageCluster/Filter/",
0
    };
    static const char* fwdDeclCode = 
R"DICTFWDDCLS(
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
namespace larocv{class __attribute__((annotate("$clingAutoload$PiZeroFilter.h")))  PiZeroFilter;}
namespace larocv{class __attribute__((annotate("$clingAutoload$PiZeroFilterV.h")))  PiZeroFilterV;}
namespace larocv{class __attribute__((annotate("$clingAutoload$CheckStartDistance.h")))  CheckStartDistance;}
namespace larocv{class __attribute__((annotate("$clingAutoload$SimpleCuts.h")))  SimpleCuts;}
namespace larocv{class __attribute__((annotate("$clingAutoload$AttachedClusters.h")))  AttachedClusters;}
namespace larocv{class __attribute__((annotate("$clingAutoload$RecoROIFilter.h")))  RecoROIFilter;}
namespace larocv{class __attribute__((annotate("$clingAutoload$ROIParameters.h")))  ROIParameters;}
namespace larocv{class __attribute__((annotate("$clingAutoload$RecoParameters.h")))  RecoParameters;}
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(

#ifndef G__VECTOR_HAS_CLASS_ITERATOR
  #define G__VECTOR_HAS_CLASS_ITERATOR 1
#endif

#define _BACKWARD_BACKWARD_WARNING_H
#include "AttachedClusters.h"
#include "CheckStartDistance.h"
#include "PiZeroFilter.h"
#include "PiZeroFilterV.h"
#include "ROIParameters.h"
#include "RecoParameters.h"
#include "RecoROIFilter.h"
#include "SimpleCuts.h"
#include "TrackShower.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[]={
"larocv::AttachedClusters", payloadCode, "@",
"larocv::CheckStartDistance", payloadCode, "@",
"larocv::PiZeroFilter", payloadCode, "@",
"larocv::PiZeroFilterV", payloadCode, "@",
"larocv::ROIParameters", payloadCode, "@",
"larocv::RecoParameters", payloadCode, "@",
"larocv::RecoROIFilter", payloadCode, "@",
"larocv::SimpleCuts", payloadCode, "@",
nullptr};

    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("libLArOpenCV_ImageClusterFilter",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_libLArOpenCV_ImageClusterFilter_Impl, {}, classesHeaders);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_libLArOpenCV_ImageClusterFilter_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_libLArOpenCV_ImageClusterFilter() {
  TriggerDictionaryInitialization_libLArOpenCV_ImageClusterFilter_Impl();
}
