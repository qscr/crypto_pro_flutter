/*
 * Copyright(C) 2004-2019
 *
 * Этот файл содержит информацию, являющуюся
 * собственностью компании Крипто Про.
 *
 * Любая часть этого файла не может быть скопирована,
 * исправлена, переведена на другие языки,
 * локализована или модифицирована любым способом,
 * откомпилирована, передана по сети с или на
 * любую компьютерную систему без предварительного
 * заключения соглашения с компанией Крипто Про.
 */

/*!
 * \file $RCSfile$
 * \version $Revision: 198833 $
 * \date $Date:: 2019-09-06 17:02:08 +0300#$
 * \author $Author: afevma $
 *
 * \brief API для работы с XAdES (XML Advanced Electronic Signatures)
 */

#ifndef _XADES_H_INCLUDED
#define _XADES_H_INCLUDED

#include "cades.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1400) && !defined(XADES_NOFORCE_MANIFEST)

#define XADES_ASSEMBLY_NAME "CryptoPro.PKI.XAdES"
#define XADES_ASSEMBLY_VERSION "2.0.0.0"

#ifndef CP_ASSEMBLY_PUBLICKEYTOKEN
#define CP_ASSEMBLY_PUBLICKEYTOKEN "a6d31b994cfcddc4"
#endif // CP_ASSEMBLY_PUBLICKEYTOKEN

#ifdef _M_IX86
#define XADES_ASSEMBLY_PROCARCH "x86"
#endif // _M_IX86

#ifdef _M_AMD64
#define XADES_ASSEMBLY_PROCARCH "amd64"
#endif // _M_AMD64

#pragma comment(linker,"/manifestdependency:\"type='win32' " \
        "name='" XADES_ASSEMBLY_NAME "' " \
        "version='" XADES_ASSEMBLY_VERSION "' " \
        "processorArchitecture='" XADES_ASSEMBLY_PROCARCH "' " \
        "language='*' " \
        "publicKeyToken='" CP_ASSEMBLY_PUBLICKEYTOKEN "'\"")

#endif // defined(_MSC_VER) && (_MSC_VER >= 1400) && !defined(XADES_NOFORCE_MANIFEST)

#if !defined XADES_DLL_DEFINES
#   define XADES_DLL_DEFINES
#   if defined _WIN32 && !defined XADES_STATIC
#	ifdef XADES_DLL
#	    define XADES_CLASS __declspec(dllexport)
#	    define XADES_API __declspec(dllexport)
#	    define XADES_DATA __declspec(dllexport)
#	    define XADES_EXTERN_TEMPLATE
#	else // defined XADES_DLL
#	    define XADES_CLASS __declspec(dllimport)
#	    define XADES_API __declspec(dllimport)
#	    define XADES_DATA __declspec(dllimport)
#	    define XADES_EXTERN_TEMPLATE extern
#	endif // !defined XADES_DLL
#   else // defined _WIN32 && !defined XADES_STATIC
#	define XADES_CLASS
#	define XADES_API
#	define XADES_DATA
#	define XADES_EXTERN_TEMPLATE
#       define NO_EXPIMP_CDLLLIST_ITERATORS
#   endif // !defined _WIN32 ||  defined XADES_STATIC
#endif // !defined XADES_DLL_DEFINES

// LOWWORD
typedef enum
{
    XML_XADES_SIGNATURE_TYPE_ENVELOPED  = 0x00,
    XML_XADES_SIGNATURE_TYPE_ENVELOPING = 0x01,
    XML_XADES_SIGNATURE_TYPE_TEMPLATE   = 0x02
} XML_XADES_SIGNATURE_TYPE;

// Тип подписи LOWWORD
#define XADES_DEFAULT       0x00000010
#define XADES_BES           0x00000020
#define XADES_T             0x00000050
#define XADES_X_LONG_TYPE_1 0x000005d0
#define XADES_A             0x000007d0
#define XADES_XMLDSIG       0x00000000
#define XADES_NONE          0xf0000000

// Дополнительные параметры к типу подписи.
// HIWORD
#define XADES_DISABLE_REDUNDANCY          ADES_DISABLE_REDUNDANCY
#define XADES_USE_OCSP_AUTHORIZED_POLICY  ADES_USE_OCSP_AUTHORIZED_POLICY

// Типы аутентификации (конфликт с XML_XADES_SIGNATURE_TYPE).

// #define XADES_AUTH_ANONYMOUS	CADES_AUTH_ANONYMOUS
// #define XADES_AUTH_BASIC		CADES_AUTH_BASIC
// #define XADES_AUTH_NTLM		CADES_AUTH_NTLM
// #define XADES_AUTH_DIGEST	CADES_AUTH_DIGEST
// #define XADES_AUTH_NEGOTIATE	CADES_AUTH_NEGOTIATE

// Коды ошибок.
#define XADES_VERIFY_SUCCESS                            ADES_VERIFY_SUCCESS
#define XADES_VERIFY_INVALID_REFS_AND_VALUES            ADES_VERIFY_INVALID_REFS_AND_VALUES
#define XADES_VERIFY_SIGNER_NOT_FOUND                   ADES_VERIFY_SIGNER_NOT_FOUND
#define XADES_VERIFY_NO_VALID_SIGNATURE_TIMESTAMP       ADES_VERIFY_NO_VALID_SIGNATURE_TIMESTAMP
#define XADES_VERIFY_REFS_AND_VALUES_NO_MATCH           ADES_VERIFY_REFS_AND_VALUES_NO_MATCH
#define XADES_VERIFY_NO_CHAIN                           ADES_VERIFY_NO_CHAIN
#define XADES_VERIFY_END_CERT_REVOCATION                ADES_VERIFY_END_CERT_REVOCATION
#define XADES_VERIFY_CHAIN_CERT_REVOCATION              ADES_VERIFY_CHAIN_CERT_REVOCATION
#define XADES_VERIFY_BAD_SIGNATURE                      ADES_VERIFY_BAD_SIGNATURE
#define XADES_VERIFY_NO_VALID_CADES_C_TIMESTAMP         ADES_VERIFY_NO_VALID_CADES_C_TIMESTAMP
#define XADES_VERIFY_BAD_POLICY                         ADES_VERIFY_BAD_POLICY
#define XADES_VERIFY_UNSUPPORTED_ATTRIBUTE              ADES_VERIFY_UNSUPPORTED_ATTRIBUTE
#define XADES_VERIFY_FAILED_POLICY                      ADES_VERIFY_FAILED_POLICY
#define XADES_VERIFY_ECONTENTTYPE_NO_MATCH              ADES_VERIFY_ECONTENTTYPE_NO_MATCH
#define XADES_VERIFY_NO_VALID_SIG_AND_REFS_TIMESTAMP    ADES_VERIFY_NO_VALID_CADES_C_TIMESTAMP
#define XADES_TIMESTAMP_NO_CERT_REQ                     ADES_TIMESTAMP_NO_CERT_REQ
#define XADES_CHECK_CERT_REQ                            ADES_CHECK_CERT_REQ
#define XADES_SKIP_IE_PROXY_CONFIGURATION               ADES_SKIP_IE_PROXY_CONFIGURATION

// В структурах есть отличия от cades.
typedef struct _XADES_SIGN_PARA
{
    DWORD dwSize;
    DWORD dwSignatureType;
    PCCERT_CONTEXT pSignerCert;
    LPCSTR szDigestMethod;
    LPCSTR szSignatureMethod;
    HCERTSTORE hAdditionalStore;
    PCADES_SERVICE_CONNECTION_PARA pTspConnectionPara;
    PCADES_PROXY_PARA pProxyPara;
    LPVOID pXadesExtraPara;
    DWORD cAdditionalOCSPServices;
    LPCWSTR *rgAdditionalOCSPServices;
DEFINE_ADES_STRUCT_MEMBERS(_XADES_SIGN_PARA);
} XADES_SIGN_PARA, *PXADES_SIGN_PARA;

typedef struct _XADES_EXTRA_PARA
{
    DWORD dwSize;
    DWORD dwFlags;
DEFINE_ADES_STRUCT_MEMBERS(_XADES_EXTRA_PARA);
} XADES_EXTRA_PARA, *PXADES_EXTRA_PARA;

typedef struct _XADES_SIGN_MESSAGE_PARA
{
    DWORD dwSize;
    PXADES_SIGN_PARA pXadesSignPara;
DEFINE_ADES_STRUCT_MEMBERS(_XADES_SIGN_MESSAGE_PARA);
} XADES_SIGN_MESSAGE_PARA, *PXADES_SIGN_MESSAGE_PARA;

typedef struct _XADES_VERIFICATION_PARA
{
    DWORD dwSize;
    PCADES_PROXY_PARA pProxyPara;
    HCERTSTORE hStore;
    CSP_BOOL bIsDetached;
    LPVOID pReserved3; //TODO: Поле зарезервервировано для передачи PCADES_POLICY_DATA, может сразу его назвать так?
    DWORD dwSignatureType;
    DWORD dwFlags;
DEFINE_ADES_STRUCT_MEMBERS(_XADES_VERIFICATION_PARA);
} XADES_VERIFICATION_PARA, *PXADES_VERIFICATION_PARA;

typedef struct _XADES_VERIFICATION_INFO
{
    DWORD dwSize;
    DWORD dwStatus;
    DWORD dwVerifiedXadesType;
    PCCERT_CONTEXT pSignerCert;
    LPFILETIME pSigningTime;
    LPFILETIME pReserved; // TODO: не очень понятно зачем его оставлять? Если будем добавлять поля для Validation Report то тут это точно лишнее.
    LPFILETIME pSignatureTimeStampTime;
    LPFILETIME pSigAndRefsTimeStampTime;
DEFINE_ADES_STRUCT_MEMBERS(_XADES_VERIFICATION_INFO);
} XADES_VERIFICATION_INFO, *PXADES_VERIFICATION_INFO;

typedef struct _XADES_VERIFICATION_INFO_ARRAY
{
    DWORD dwSize;
    DWORD cbCount;
    PXADES_VERIFICATION_INFO pXadesVerificationInfo;
    DEFINE_ADES_STRUCT_MEMBERS(_XADES_VERIFICATION_INFO_ARRAY);
} XADES_VERIFICATION_INFO_ARRAY, *PXADES_VERIFICATION_INFO_ARRAY;

typedef struct _XADES_VERIFY_MESSAGE_PARA
{
    DWORD dwSize;
    PXADES_VERIFICATION_PARA pXadesVerifyPara;
DEFINE_ADES_STRUCT_MEMBERS(_XADES_VERIFY_MESSAGE_PARA);
} XADES_VERIFY_MESSAGE_PARA, *PXADES_VERIFY_MESSAGE_PARA;
/*
typedef struct _XADES_ENHANCE_MESSAGE_PARA
{
    DWORD dwSize;
    DWORD dwXadesType;
    PXADES_SIGN_PARA pXadesSignPara;
DEFINE_ADES_STRUCT_MEMBERS(_XADES_ENHANCE_MESSAGE_PARA);
} XADES_ENHANCE_MESSAGE_PARA, *PXADES_ENHANCE_MESSAGE_PARA;
*/
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// Simplified API

XADES_API CSP_BOOL WINAPI XadesIsTypeEncoded(
    __in LPCSTR pXPathString,
    __in const BYTE *pbSignedBlob,
    __in DWORD cbSignedBlob,
    __in DWORD dwXadesType,
    __out CSP_BOOL *pbResult);

XADES_API LONG WINAPI XadesGetSignerCount(
    __in LPCSTR pXPathString,
    __in const BYTE *pbSignedBlob,
    __in DWORD cbSignedBlob);

XADES_API CSP_BOOL WINAPI XadesSign(
    __in PXADES_SIGN_MESSAGE_PARA pSignPara,
    __in LPCSTR pXPathString,
    __in CSP_BOOL fDetachedSignature,
    __in const BYTE *pbToBeSigned,
    __in DWORD cbToBeSigned,
    __out PCRYPT_DATA_BLOB *ppSignedBlob);

XADES_API CSP_BOOL WINAPI XadesVerify(
    __in PXADES_VERIFY_MESSAGE_PARA pVerifyPara,
    __in LPCSTR pXPathString,
    __in const BYTE *pbSignedBlob,
    __in DWORD cbSignedBlob,
    __out_opt PXADES_VERIFICATION_INFO_ARRAY *ppVerificationInfoArray);

XADES_API CSP_BOOL WINAPI XadesGetSigningCertId(
    __in LPCSTR pXPathString,
    __in const BYTE *pbSignedBlob,
    __in DWORD cbSignedBlob,
    __out_opt PCRYPT_DATA_BLOB *ppCertId);

XADES_API ALG_ID WINAPI XadesGetSigningCertIdHashAlg(
    __in LPCSTR pXPathString,
    __in const BYTE *pbSignedBlob,
    __in DWORD cbSignedBlob
    );

/*

XADES_API CSP_BOOL WINAPI XadesGetSignatureTimestamps(
    __in LPCSTR pXPathString,
    __in const BYTE* pbSignedBlob,
    __in DWORD cbSignedBlob,
    __out PCADES_BLOB_ARRAY *ppTimestamps);

XADES_API CSP_BOOL WINAPI XadesGetSigAndRefsTimestamps(
    __in LPCSTR pXPathString,
    __in const BYTE* pbSignedBlob,
    __in DWORD cbSignedBlob,
    __out PCADES_BLOB_ARRAY *ppTimestamps);

*/

XADES_API CSP_BOOL WINAPI XadesGetCertificateValues(
    __in LPCSTR pXPathString,
    __in const BYTE *pbSignedBlob,
    __in DWORD cbSignedBlob,
    __out PCADES_BLOB_ARRAY *ppCertificates);

/*

XADES_API CSP_BOOL WINAPI XadesGetRevocationValues(
    __in LPCSTR pXPathString,
    __in const BYTE* pbSignedBlob,
    __in DWORD cbSignedBlob,
    __out PCADES_BLOB_ARRAY *ppCRLs,
    __out PCADES_BLOB_ARRAY *ppBasicOCSPResponses);

*/

/*
XADES_API CSP_BOOL WINAPI XadesFreeVerificationInfo(
    __in PXADES_VERIFICATION_INFO pVerificationInfo);
*/

XADES_API CSP_BOOL WINAPI XadesFreeVerificationInfoArray(
    __in PXADES_VERIFICATION_INFO_ARRAY pVerificationInfoArray);

XADES_API CSP_BOOL WINAPI XadesFreeBlob(
    __in PCRYPT_DATA_BLOB pBlob);

XADES_API CSP_BOOL WINAPI XadesFreeBlobArray(
    __in PCADES_BLOB_ARRAY pBlobArray);

#ifdef __cplusplus
} /* extern "C" */
#endif // __cplusplus

#endif // _XADES_H_INCLUDED
