// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME LArOpenCV_ImageClusterCluster

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
#include "SBCluster.h"
#include "SBManCluster.h"
#include "TCluster.h"

// Header files passed via #pragma extra_include

namespace ROOT {
   static TClass *larocvcLcLSBCluster_Dictionary();
   static void larocvcLcLSBCluster_TClassManip(TClass*);
   static void *new_larocvcLcLSBCluster(void *p = 0);
   static void *newArray_larocvcLcLSBCluster(Long_t size, void *p);
   static void delete_larocvcLcLSBCluster(void *p);
   static void deleteArray_larocvcLcLSBCluster(void *p);
   static void destruct_larocvcLcLSBCluster(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::larocv::SBCluster*)
   {
      ::larocv::SBCluster *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::larocv::SBCluster));
      static ::ROOT::TGenericClassInfo 
         instance("larocv::SBCluster", "SBCluster.h", 11,
                  typeid(::larocv::SBCluster), DefineBehavior(ptr, ptr),
                  &larocvcLcLSBCluster_Dictionary, isa_proxy, 4,
                  sizeof(::larocv::SBCluster) );
      instance.SetNew(&new_larocvcLcLSBCluster);
      instance.SetNewArray(&newArray_larocvcLcLSBCluster);
      instance.SetDelete(&delete_larocvcLcLSBCluster);
      instance.SetDeleteArray(&deleteArray_larocvcLcLSBCluster);
      instance.SetDestructor(&destruct_larocvcLcLSBCluster);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::larocv::SBCluster*)
   {
      return GenerateInitInstanceLocal((::larocv::SBCluster*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::larocv::SBCluster*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *larocvcLcLSBCluster_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::larocv::SBCluster*)0x0)->GetClass();
      larocvcLcLSBCluster_TClassManip(theClass);
   return theClass;
   }

   static void larocvcLcLSBCluster_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *larocvcLcLTCluster_Dictionary();
   static void larocvcLcLTCluster_TClassManip(TClass*);
   static void *new_larocvcLcLTCluster(void *p = 0);
   static void *newArray_larocvcLcLTCluster(Long_t size, void *p);
   static void delete_larocvcLcLTCluster(void *p);
   static void deleteArray_larocvcLcLTCluster(void *p);
   static void destruct_larocvcLcLTCluster(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::larocv::TCluster*)
   {
      ::larocv::TCluster *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::larocv::TCluster));
      static ::ROOT::TGenericClassInfo 
         instance("larocv::TCluster", "TCluster.h", 25,
                  typeid(::larocv::TCluster), DefineBehavior(ptr, ptr),
                  &larocvcLcLTCluster_Dictionary, isa_proxy, 4,
                  sizeof(::larocv::TCluster) );
      instance.SetNew(&new_larocvcLcLTCluster);
      instance.SetNewArray(&newArray_larocvcLcLTCluster);
      instance.SetDelete(&delete_larocvcLcLTCluster);
      instance.SetDeleteArray(&deleteArray_larocvcLcLTCluster);
      instance.SetDestructor(&destruct_larocvcLcLTCluster);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::larocv::TCluster*)
   {
      return GenerateInitInstanceLocal((::larocv::TCluster*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::larocv::TCluster*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *larocvcLcLTCluster_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::larocv::TCluster*)0x0)->GetClass();
      larocvcLcLTCluster_TClassManip(theClass);
   return theClass;
   }

   static void larocvcLcLTCluster_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *larocvcLcLSBManCluster_Dictionary();
   static void larocvcLcLSBManCluster_TClassManip(TClass*);
   static void *new_larocvcLcLSBManCluster(void *p = 0);
   static void *newArray_larocvcLcLSBManCluster(Long_t size, void *p);
   static void delete_larocvcLcLSBManCluster(void *p);
   static void deleteArray_larocvcLcLSBManCluster(void *p);
   static void destruct_larocvcLcLSBManCluster(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::larocv::SBManCluster*)
   {
      ::larocv::SBManCluster *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::larocv::SBManCluster));
      static ::ROOT::TGenericClassInfo 
         instance("larocv::SBManCluster", "SBManCluster.h", 11,
                  typeid(::larocv::SBManCluster), DefineBehavior(ptr, ptr),
                  &larocvcLcLSBManCluster_Dictionary, isa_proxy, 4,
                  sizeof(::larocv::SBManCluster) );
      instance.SetNew(&new_larocvcLcLSBManCluster);
      instance.SetNewArray(&newArray_larocvcLcLSBManCluster);
      instance.SetDelete(&delete_larocvcLcLSBManCluster);
      instance.SetDeleteArray(&deleteArray_larocvcLcLSBManCluster);
      instance.SetDestructor(&destruct_larocvcLcLSBManCluster);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::larocv::SBManCluster*)
   {
      return GenerateInitInstanceLocal((::larocv::SBManCluster*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::larocv::SBManCluster*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *larocvcLcLSBManCluster_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::larocv::SBManCluster*)0x0)->GetClass();
      larocvcLcLSBManCluster_TClassManip(theClass);
   return theClass;
   }

   static void larocvcLcLSBManCluster_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_larocvcLcLSBCluster(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::SBCluster : new ::larocv::SBCluster;
   }
   static void *newArray_larocvcLcLSBCluster(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::SBCluster[nElements] : new ::larocv::SBCluster[nElements];
   }
   // Wrapper around operator delete
   static void delete_larocvcLcLSBCluster(void *p) {
      delete ((::larocv::SBCluster*)p);
   }
   static void deleteArray_larocvcLcLSBCluster(void *p) {
      delete [] ((::larocv::SBCluster*)p);
   }
   static void destruct_larocvcLcLSBCluster(void *p) {
      typedef ::larocv::SBCluster current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::larocv::SBCluster

namespace ROOT {
   // Wrappers around operator new
   static void *new_larocvcLcLTCluster(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::TCluster : new ::larocv::TCluster;
   }
   static void *newArray_larocvcLcLTCluster(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::TCluster[nElements] : new ::larocv::TCluster[nElements];
   }
   // Wrapper around operator delete
   static void delete_larocvcLcLTCluster(void *p) {
      delete ((::larocv::TCluster*)p);
   }
   static void deleteArray_larocvcLcLTCluster(void *p) {
      delete [] ((::larocv::TCluster*)p);
   }
   static void destruct_larocvcLcLTCluster(void *p) {
      typedef ::larocv::TCluster current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::larocv::TCluster

namespace ROOT {
   // Wrappers around operator new
   static void *new_larocvcLcLSBManCluster(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::SBManCluster : new ::larocv::SBManCluster;
   }
   static void *newArray_larocvcLcLSBManCluster(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::SBManCluster[nElements] : new ::larocv::SBManCluster[nElements];
   }
   // Wrapper around operator delete
   static void delete_larocvcLcLSBManCluster(void *p) {
      delete ((::larocv::SBManCluster*)p);
   }
   static void deleteArray_larocvcLcLSBManCluster(void *p) {
      delete [] ((::larocv::SBManCluster*)p);
   }
   static void destruct_larocvcLcLSBManCluster(void *p) {
      typedef ::larocv::SBManCluster current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::larocv::SBManCluster

namespace {
  void TriggerDictionaryInitialization_libLArOpenCV_ImageClusterCluster_Impl() {
    static const char* headers[] = {
"SBCluster.h",
"SBManCluster.h",
"TCluster.h",
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
"/usr/local/ROOT//include/root",
"/Users/vgenty/sw/laropencv/ImageCluster/Cluster/",
0
    };
    static const char* fwdDeclCode = 
R"DICTFWDDCLS(
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
namespace larocv{class __attribute__((annotate("$clingAutoload$SBCluster.h")))  SBCluster;}
namespace larocv{class __attribute__((annotate("$clingAutoload$TCluster.h")))  TCluster;}
namespace larocv{class __attribute__((annotate("$clingAutoload$SBManCluster.h")))  SBManCluster;}
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(

#ifndef G__VECTOR_HAS_CLASS_ITERATOR
  #define G__VECTOR_HAS_CLASS_ITERATOR 1
#endif

#define _BACKWARD_BACKWARD_WARNING_H
#include "SBCluster.h"
#include "SBManCluster.h"
#include "TCluster.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[]={
"larocv::SBCluster", payloadCode, "@",
"larocv::SBManCluster", payloadCode, "@",
"larocv::TCluster", payloadCode, "@",
nullptr};

    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("libLArOpenCV_ImageClusterCluster",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_libLArOpenCV_ImageClusterCluster_Impl, {}, classesHeaders);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_libLArOpenCV_ImageClusterCluster_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_libLArOpenCV_ImageClusterCluster() {
  TriggerDictionaryInitialization_libLArOpenCV_ImageClusterCluster_Impl();
}
