// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME LArOpenCV_ImageClusterReCluster

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
#include "joshua.h"

// Header files passed via #pragma extra_include

namespace ROOT {
   static TClass *larocvcLcLjoshua_Dictionary();
   static void larocvcLcLjoshua_TClassManip(TClass*);
   static void *new_larocvcLcLjoshua(void *p = 0);
   static void *newArray_larocvcLcLjoshua(Long_t size, void *p);
   static void delete_larocvcLcLjoshua(void *p);
   static void deleteArray_larocvcLcLjoshua(void *p);
   static void destruct_larocvcLcLjoshua(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::larocv::joshua*)
   {
      ::larocv::joshua *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::larocv::joshua));
      static ::ROOT::TGenericClassInfo 
         instance("larocv::joshua", "joshua.h", 13,
                  typeid(::larocv::joshua), DefineBehavior(ptr, ptr),
                  &larocvcLcLjoshua_Dictionary, isa_proxy, 4,
                  sizeof(::larocv::joshua) );
      instance.SetNew(&new_larocvcLcLjoshua);
      instance.SetNewArray(&newArray_larocvcLcLjoshua);
      instance.SetDelete(&delete_larocvcLcLjoshua);
      instance.SetDeleteArray(&deleteArray_larocvcLcLjoshua);
      instance.SetDestructor(&destruct_larocvcLcLjoshua);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::larocv::joshua*)
   {
      return GenerateInitInstanceLocal((::larocv::joshua*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::larocv::joshua*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *larocvcLcLjoshua_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::larocv::joshua*)0x0)->GetClass();
      larocvcLcLjoshua_TClassManip(theClass);
   return theClass;
   }

   static void larocvcLcLjoshua_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_larocvcLcLjoshua(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::joshua : new ::larocv::joshua;
   }
   static void *newArray_larocvcLcLjoshua(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::joshua[nElements] : new ::larocv::joshua[nElements];
   }
   // Wrapper around operator delete
   static void delete_larocvcLcLjoshua(void *p) {
      delete ((::larocv::joshua*)p);
   }
   static void deleteArray_larocvcLcLjoshua(void *p) {
      delete [] ((::larocv::joshua*)p);
   }
   static void destruct_larocvcLcLjoshua(void *p) {
      typedef ::larocv::joshua current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::larocv::joshua

namespace {
  void TriggerDictionaryInitialization_libLArOpenCV_ImageClusterReCluster_Impl() {
    static const char* headers[] = {
"joshua.h",
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
"/Users/vgenty/sw/laropencv/ImageCluster/ReCluster/",
0
    };
    static const char* fwdDeclCode = 
R"DICTFWDDCLS(
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
namespace larocv{class __attribute__((annotate("$clingAutoload$joshua.h")))  joshua;}
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(

#ifndef G__VECTOR_HAS_CLASS_ITERATOR
  #define G__VECTOR_HAS_CLASS_ITERATOR 1
#endif

#define _BACKWARD_BACKWARD_WARNING_H
#include "joshua.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[]={
"larocv::joshua", payloadCode, "@",
nullptr};

    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("libLArOpenCV_ImageClusterReCluster",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_libLArOpenCV_ImageClusterReCluster_Impl, {}, classesHeaders);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_libLArOpenCV_ImageClusterReCluster_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_libLArOpenCV_ImageClusterReCluster() {
  TriggerDictionaryInitialization_libLArOpenCV_ImageClusterReCluster_Impl();
}
