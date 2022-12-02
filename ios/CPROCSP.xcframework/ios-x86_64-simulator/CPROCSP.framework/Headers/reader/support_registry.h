#ifndef SUPPORT_REGISTRY_H_INCLUDED
#define SUPPORT_REGISTRY_H_INCLUDED

#include<CPROCSP/reader/support_base_defs.h>

/*+ Путь к локальному registry. +*/
#define SUPPORT_REG_LOCAL _TEXT( "\\LOCAL" )
/*+ Путь к глобальному registry. +*/
#define SUPPORT_REG_GLOBAL _TEXT( "\\GLOBAL" )
/*+ Путь к локальному защищённому registry. +*/
#define SUPPORT_FKC_PROTECTED_LOCAL _TEXT( "\\PROTECTED_FKC_LOCAL" )
/*+ Путь к глобальному защищённому registry. +*/
#define SUPPORT_FKC_PROTECTED_GLOBAL _TEXT( "\\PROTECTED_FKC_GLOBAL" )
/*+ Путь к локальному защищённому registry. +*/
#define SUPPORT_REG_PROTECTED_LOCAL _TEXT( "\\PROTECTED_LOCAL" )
/*+ Путь к глобальному защищённому registry. +*/
#define SUPPORT_REG_PROTECTED_GLOBAL _TEXT( "\\PROTECTED_GLOBAL" )
/*+ Путь к конфигурационному registry. +*/
#define SUPPORT_REG_CONFIG _TEXT( "\\CONFIG" )
/*+ Путь к конфигурационному registry. +*/
#define SUPPORT_REG_LICENSE _TEXT( "\\LICENSE\\InprocServer32" )
#define SUPPORT_REG_APPPATH _TEXT( "\\APPPATH" )
#define SUPPORT_REG_CRYPTOGRAPHY _TEXT( "\\CRYPTOGRAPHY" )

#define SUPPORT_REG_POLICIES _TEXT( "\\POLICIES" )
#define SUPPORT_REG_SYSCONTROL _TEXT( "\\SYSCONTROL" )
#ifdef _WIN64
#define SUPPORT_REG_APPPATH32 _TEXT( "\\APPPATH32" )
#define SUPPORT_REG_CONFIG64 _TEXT( "\\CONFIG64" )
#endif
#define SUPPORT_REG_CRYPTOGRAPHY64 _TEXT( "\\CRYPTOGRAPHY64" )
#endif //SUPPORT_REGISTRY_H_INCLUDED


