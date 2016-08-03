// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME LArOpenCV_ImageClusterMatch

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
#include "EmptyMatcher.h"
#include "MatchTimeOverlap.h"

// Header files passed via #pragma extra_include

namespace ROOT {
   static TClass *larocvcLcLMatchTimeOverlap_Dictionary();
   static void larocvcLcLMatchTimeOverlap_TClassManip(TClass*);
   static void *new_larocvcLcLMatchTimeOverlap(void *p = 0);
   static void *newArray_larocvcLcLMatchTimeOverlap(Long_t size, void *p);
   static void delete_larocvcLcLMatchTimeOverlap(void *p);
   static void deleteArray_larocvcLcLMatchTimeOverlap(void *p);
   static void destruct_larocvcLcLMatchTimeOverlap(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::larocv::MatchTimeOverlap*)
   {
      ::larocv::MatchTimeOverlap *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::larocv::MatchTimeOverlap));
      static ::ROOT::TGenericClassInfo 
         instance("larocv::MatchTimeOverlap", "MatchTimeOverlap.h", 25,
                  typeid(::larocv::MatchTimeOverlap), DefineBehavior(ptr, ptr),
                  &larocvcLcLMatchTimeOverlap_Dictionary, isa_proxy, 4,
                  sizeof(::larocv::MatchTimeOverlap) );
      instance.SetNew(&new_larocvcLcLMatchTimeOverlap);
      instance.SetNewArray(&newArray_larocvcLcLMatchTimeOverlap);
      instance.SetDelete(&delete_larocvcLcLMatchTimeOverlap);
      instance.SetDeleteArray(&deleteArray_larocvcLcLMatchTimeOverlap);
      instance.SetDestructor(&destruct_larocvcLcLMatchTimeOverlap);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::larocv::MatchTimeOverlap*)
   {
      return GenerateInitInstanceLocal((::larocv::MatchTimeOverlap*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::larocv::MatchTimeOverlap*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *larocvcLcLMatchTimeOverlap_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::larocv::MatchTimeOverlap*)0x0)->GetClass();
      larocvcLcLMatchTimeOverlap_TClassManip(theClass);
   return theClass;
   }

   static void larocvcLcLMatchTimeOverlap_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *larocvcLcLEmptyMatcher_Dictionary();
   static void larocvcLcLEmptyMatcher_TClassManip(TClass*);
   static void *new_larocvcLcLEmptyMatcher(void *p = 0);
   static void *newArray_larocvcLcLEmptyMatcher(Long_t size, void *p);
   static void delete_larocvcLcLEmptyMatcher(void *p);
   static void deleteArray_larocvcLcLEmptyMatcher(void *p);
   static void destruct_larocvcLcLEmptyMatcher(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::larocv::EmptyMatcher*)
   {
      ::larocv::EmptyMatcher *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::larocv::EmptyMatcher));
      static ::ROOT::TGenericClassInfo 
         instance("larocv::EmptyMatcher", "EmptyMatcher.h", 25,
                  typeid(::larocv::EmptyMatcher), DefineBehavior(ptr, ptr),
                  &larocvcLcLEmptyMatcher_Dictionary, isa_proxy, 4,
                  sizeof(::larocv::EmptyMatcher) );
      instance.SetNew(&new_larocvcLcLEmptyMatcher);
      instance.SetNewArray(&newArray_larocvcLcLEmptyMatcher);
      instance.SetDelete(&delete_larocvcLcLEmptyMatcher);
      instance.SetDeleteArray(&deleteArray_larocvcLcLEmptyMatcher);
      instance.SetDestructor(&destruct_larocvcLcLEmptyMatcher);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::larocv::EmptyMatcher*)
   {
      return GenerateInitInstanceLocal((::larocv::EmptyMatcher*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::larocv::EmptyMatcher*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *larocvcLcLEmptyMatcher_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::larocv::EmptyMatcher*)0x0)->GetClass();
      larocvcLcLEmptyMatcher_TClassManip(theClass);
   return theClass;
   }

   static void larocvcLcLEmptyMatcher_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_larocvcLcLMatchTimeOverlap(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::MatchTimeOverlap : new ::larocv::MatchTimeOverlap;
   }
   static void *newArray_larocvcLcLMatchTimeOverlap(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::MatchTimeOverlap[nElements] : new ::larocv::MatchTimeOverlap[nElements];
   }
   // Wrapper around operator delete
   static void delete_larocvcLcLMatchTimeOverlap(void *p) {
      delete ((::larocv::MatchTimeOverlap*)p);
   }
   static void deleteArray_larocvcLcLMatchTimeOverlap(void *p) {
      delete [] ((::larocv::MatchTimeOverlap*)p);
   }
   static void destruct_larocvcLcLMatchTimeOverlap(void *p) {
      typedef ::larocv::MatchTimeOverlap current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::larocv::MatchTimeOverlap

namespace ROOT {
   // Wrappers around operator new
   static void *new_larocvcLcLEmptyMatcher(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::EmptyMatcher : new ::larocv::EmptyMatcher;
   }
   static void *newArray_larocvcLcLEmptyMatcher(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::EmptyMatcher[nElements] : new ::larocv::EmptyMatcher[nElements];
   }
   // Wrapper around operator delete
   static void delete_larocvcLcLEmptyMatcher(void *p) {
      delete ((::larocv::EmptyMatcher*)p);
   }
   static void deleteArray_larocvcLcLEmptyMatcher(void *p) {
      delete [] ((::larocv::EmptyMatcher*)p);
   }
   static void destruct_larocvcLcLEmptyMatcher(void *p) {
      typedef ::larocv::EmptyMatcher current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::larocv::EmptyMatcher

namespace {
  void TriggerDictionaryInitialization_libLArOpenCV_ImageClusterMatch_Impl() {
    static const char* headers[] = {
"EmptyMatcher.h",
"MatchTimeOverlap.h",
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
"/Users/vgenty/sw/laropencv/ImageCluster/Match/",
0
    };
    static const char* fwdDeclCode = 
R"DICTFWDDCLS(
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
namespace larocv{class __attribute__((annotate("$clingAutoload$MatchTimeOverlap.h")))  MatchTimeOverlap;}
namespace larocv{class __attribute__((annotate("$clingAutoload$EmptyMatcher.h")))  EmptyMatcher;}
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(

#ifndef G__VECTOR_HAS_CLASS_ITERATOR
  #define G__VECTOR_HAS_CLASS_ITERATOR 1
#endif

#define _BACKWARD_BACKWARD_WARNING_H
#include "EmptyMatcher.h"
#include "MatchTimeOverlap.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[]={
"larocv::EmptyMatcher", payloadCode, "@",
"larocv::MatchTimeOverlap", payloadCode, "@",
nullptr};

    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("libLArOpenCV_ImageClusterMatch",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_libLArOpenCV_ImageClusterMatch_Impl, {}, classesHeaders);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_libLArOpenCV_ImageClusterMatch_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_libLArOpenCV_ImageClusterMatch() {
  TriggerDictionaryInitialization_libLArOpenCV_ImageClusterMatch_Impl();
}
