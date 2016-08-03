// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME LArOpenCV_ImageClusterStartPoint

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
#include "BoundRectStart.h"
#include "GetStartPoint.h"
#include "ROIAssistedStart.h"
#include "ROIStart.h"

// Header files passed via #pragma extra_include

namespace ROOT {
   static TClass *larocvcLcLBoundRectStart_Dictionary();
   static void larocvcLcLBoundRectStart_TClassManip(TClass*);
   static void *new_larocvcLcLBoundRectStart(void *p = 0);
   static void *newArray_larocvcLcLBoundRectStart(Long_t size, void *p);
   static void delete_larocvcLcLBoundRectStart(void *p);
   static void deleteArray_larocvcLcLBoundRectStart(void *p);
   static void destruct_larocvcLcLBoundRectStart(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::larocv::BoundRectStart*)
   {
      ::larocv::BoundRectStart *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::larocv::BoundRectStart));
      static ::ROOT::TGenericClassInfo 
         instance("larocv::BoundRectStart", "BoundRectStart.h", 11,
                  typeid(::larocv::BoundRectStart), DefineBehavior(ptr, ptr),
                  &larocvcLcLBoundRectStart_Dictionary, isa_proxy, 4,
                  sizeof(::larocv::BoundRectStart) );
      instance.SetNew(&new_larocvcLcLBoundRectStart);
      instance.SetNewArray(&newArray_larocvcLcLBoundRectStart);
      instance.SetDelete(&delete_larocvcLcLBoundRectStart);
      instance.SetDeleteArray(&deleteArray_larocvcLcLBoundRectStart);
      instance.SetDestructor(&destruct_larocvcLcLBoundRectStart);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::larocv::BoundRectStart*)
   {
      return GenerateInitInstanceLocal((::larocv::BoundRectStart*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::larocv::BoundRectStart*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *larocvcLcLBoundRectStart_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::larocv::BoundRectStart*)0x0)->GetClass();
      larocvcLcLBoundRectStart_TClassManip(theClass);
   return theClass;
   }

   static void larocvcLcLBoundRectStart_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *larocvcLcLROIStart_Dictionary();
   static void larocvcLcLROIStart_TClassManip(TClass*);
   static void *new_larocvcLcLROIStart(void *p = 0);
   static void *newArray_larocvcLcLROIStart(Long_t size, void *p);
   static void delete_larocvcLcLROIStart(void *p);
   static void deleteArray_larocvcLcLROIStart(void *p);
   static void destruct_larocvcLcLROIStart(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::larocv::ROIStart*)
   {
      ::larocv::ROIStart *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::larocv::ROIStart));
      static ::ROOT::TGenericClassInfo 
         instance("larocv::ROIStart", "ROIStart.h", 25,
                  typeid(::larocv::ROIStart), DefineBehavior(ptr, ptr),
                  &larocvcLcLROIStart_Dictionary, isa_proxy, 4,
                  sizeof(::larocv::ROIStart) );
      instance.SetNew(&new_larocvcLcLROIStart);
      instance.SetNewArray(&newArray_larocvcLcLROIStart);
      instance.SetDelete(&delete_larocvcLcLROIStart);
      instance.SetDeleteArray(&deleteArray_larocvcLcLROIStart);
      instance.SetDestructor(&destruct_larocvcLcLROIStart);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::larocv::ROIStart*)
   {
      return GenerateInitInstanceLocal((::larocv::ROIStart*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::larocv::ROIStart*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *larocvcLcLROIStart_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::larocv::ROIStart*)0x0)->GetClass();
      larocvcLcLROIStart_TClassManip(theClass);
   return theClass;
   }

   static void larocvcLcLROIStart_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *larocvcLcLROIAssistedStart_Dictionary();
   static void larocvcLcLROIAssistedStart_TClassManip(TClass*);
   static void *new_larocvcLcLROIAssistedStart(void *p = 0);
   static void *newArray_larocvcLcLROIAssistedStart(Long_t size, void *p);
   static void delete_larocvcLcLROIAssistedStart(void *p);
   static void deleteArray_larocvcLcLROIAssistedStart(void *p);
   static void destruct_larocvcLcLROIAssistedStart(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::larocv::ROIAssistedStart*)
   {
      ::larocv::ROIAssistedStart *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::larocv::ROIAssistedStart));
      static ::ROOT::TGenericClassInfo 
         instance("larocv::ROIAssistedStart", "ROIAssistedStart.h", 25,
                  typeid(::larocv::ROIAssistedStart), DefineBehavior(ptr, ptr),
                  &larocvcLcLROIAssistedStart_Dictionary, isa_proxy, 4,
                  sizeof(::larocv::ROIAssistedStart) );
      instance.SetNew(&new_larocvcLcLROIAssistedStart);
      instance.SetNewArray(&newArray_larocvcLcLROIAssistedStart);
      instance.SetDelete(&delete_larocvcLcLROIAssistedStart);
      instance.SetDeleteArray(&deleteArray_larocvcLcLROIAssistedStart);
      instance.SetDestructor(&destruct_larocvcLcLROIAssistedStart);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::larocv::ROIAssistedStart*)
   {
      return GenerateInitInstanceLocal((::larocv::ROIAssistedStart*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::larocv::ROIAssistedStart*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *larocvcLcLROIAssistedStart_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::larocv::ROIAssistedStart*)0x0)->GetClass();
      larocvcLcLROIAssistedStart_TClassManip(theClass);
   return theClass;
   }

   static void larocvcLcLROIAssistedStart_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_larocvcLcLBoundRectStart(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::BoundRectStart : new ::larocv::BoundRectStart;
   }
   static void *newArray_larocvcLcLBoundRectStart(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::BoundRectStart[nElements] : new ::larocv::BoundRectStart[nElements];
   }
   // Wrapper around operator delete
   static void delete_larocvcLcLBoundRectStart(void *p) {
      delete ((::larocv::BoundRectStart*)p);
   }
   static void deleteArray_larocvcLcLBoundRectStart(void *p) {
      delete [] ((::larocv::BoundRectStart*)p);
   }
   static void destruct_larocvcLcLBoundRectStart(void *p) {
      typedef ::larocv::BoundRectStart current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::larocv::BoundRectStart

namespace ROOT {
   // Wrappers around operator new
   static void *new_larocvcLcLROIStart(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::ROIStart : new ::larocv::ROIStart;
   }
   static void *newArray_larocvcLcLROIStart(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::ROIStart[nElements] : new ::larocv::ROIStart[nElements];
   }
   // Wrapper around operator delete
   static void delete_larocvcLcLROIStart(void *p) {
      delete ((::larocv::ROIStart*)p);
   }
   static void deleteArray_larocvcLcLROIStart(void *p) {
      delete [] ((::larocv::ROIStart*)p);
   }
   static void destruct_larocvcLcLROIStart(void *p) {
      typedef ::larocv::ROIStart current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::larocv::ROIStart

namespace ROOT {
   // Wrappers around operator new
   static void *new_larocvcLcLROIAssistedStart(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::ROIAssistedStart : new ::larocv::ROIAssistedStart;
   }
   static void *newArray_larocvcLcLROIAssistedStart(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::ROIAssistedStart[nElements] : new ::larocv::ROIAssistedStart[nElements];
   }
   // Wrapper around operator delete
   static void delete_larocvcLcLROIAssistedStart(void *p) {
      delete ((::larocv::ROIAssistedStart*)p);
   }
   static void deleteArray_larocvcLcLROIAssistedStart(void *p) {
      delete [] ((::larocv::ROIAssistedStart*)p);
   }
   static void destruct_larocvcLcLROIAssistedStart(void *p) {
      typedef ::larocv::ROIAssistedStart current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::larocv::ROIAssistedStart

namespace {
  void TriggerDictionaryInitialization_libLArOpenCV_ImageClusterStartPoint_Impl() {
    static const char* headers[] = {
"BoundRectStart.h",
"GetStartPoint.h",
"ROIAssistedStart.h",
"ROIStart.h",
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
"/Users/vgenty/sw/laropencv/ImageCluster/StartPoint/",
0
    };
    static const char* fwdDeclCode = 
R"DICTFWDDCLS(
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
namespace larocv{class __attribute__((annotate("$clingAutoload$BoundRectStart.h")))  BoundRectStart;}
namespace larocv{class __attribute__((annotate("$clingAutoload$ROIStart.h")))  ROIStart;}
namespace larocv{class __attribute__((annotate("$clingAutoload$ROIAssistedStart.h")))  ROIAssistedStart;}
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(

#ifndef G__VECTOR_HAS_CLASS_ITERATOR
  #define G__VECTOR_HAS_CLASS_ITERATOR 1
#endif

#define _BACKWARD_BACKWARD_WARNING_H
#include "BoundRectStart.h"
#include "GetStartPoint.h"
#include "ROIAssistedStart.h"
#include "ROIStart.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[]={
"larocv::BoundRectStart", payloadCode, "@",
"larocv::ROIAssistedStart", payloadCode, "@",
"larocv::ROIStart", payloadCode, "@",
nullptr};

    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("libLArOpenCV_ImageClusterStartPoint",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_libLArOpenCV_ImageClusterStartPoint_Impl, {}, classesHeaders);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_libLArOpenCV_ImageClusterStartPoint_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_libLArOpenCV_ImageClusterStartPoint() {
  TriggerDictionaryInitialization_libLArOpenCV_ImageClusterStartPoint_Impl();
}
