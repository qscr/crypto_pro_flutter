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
#ifndef ADES_CORE_H_INCLUDED
#define ADES_CORE_H_INCLUDED

#if defined _WIN32
#include <windows.h>
#include <wincrypt.h>
#include <prsht.h>

#else
#ifndef __in
#   define __in
#endif
#ifndef __in_opt
#   define __in_opt
#endif
#ifndef __out
#   define __out
#endif
#ifndef __out_opt
#   define __out_opt
#endif
#ifndef __reserved
#   define __reserved
#endif
#include "CSP_WinCrypt.h"
#endif    /* _WIN32 */

#include "WinCryptEx.h"

// Дополнительные параметры к типу подписи.
// HIWORD
#define ADES_DISABLE_REDUNDANCY          0x00010000
#define ADES_USE_OCSP_AUTHORIZED_POLICY  0x00020000

#define ADES_VERIFY_SUCCESS                      0x00
#define ADES_VERIFY_INVALID_REFS_AND_VALUES      0x01
#define ADES_VERIFY_SIGNER_NOT_FOUND             0x02
#define ADES_VERIFY_NO_VALID_SIGNATURE_TIMESTAMP 0x03
#define ADES_VERIFY_REFS_AND_VALUES_NO_MATCH     0x04
#define ADES_VERIFY_NO_CHAIN                     0x05
#define ADES_VERIFY_END_CERT_REVOCATION          0x06
#define ADES_VERIFY_CHAIN_CERT_REVOCATION        0x07
#define ADES_VERIFY_BAD_SIGNATURE                0x08
#define ADES_VERIFY_NO_VALID_CADES_C_TIMESTAMP   0x09
#define ADES_VERIFY_BAD_POLICY                   0x0A
#define ADES_VERIFY_UNSUPPORTED_ATTRIBUTE        0x0B
#define ADES_VERIFY_FAILED_POLICY                0x0C
#define ADES_VERIFY_ECONTENTTYPE_NO_MATCH        0x0D

#define ADES_TIMESTAMP_NO_CERT_REQ        0x00000001
#define ADES_CHECK_CERT_REQ               0x00000002
#define ADES_SKIP_IE_PROXY_CONFIGURATION  0x00000004
#define ADES_LEGACY_CRL_RETRIEVE	  0x00000008

#ifndef DEFINE_ADES_STRUCT_MEMBERS
#define DEFINE_ADES_STRUCT_MEMBERS(className)
#endif // DEFINE_ADES_STRUCT_MEMBERS

// A macro to disallow the copy constructor and operator= functions
// This should be used in the private: declarations for a class
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&);               \
    void operator=(const TypeName&)
#endif //ADES_CORE_H_INCLUDED

typedef CRYPT_SEQUENCE_OF_ANY CADES_BLOB_ARRAY, *PCADES_BLOB_ARRAY;

