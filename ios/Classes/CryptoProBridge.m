#import "CryptoProBridge.h"

#import <CPROCSP/CPROCSP.h>
#import <CPROCSP/license_api.h>
#import <CPROPKI/cades.h>

DECLARE_CHECKSUM_VARIABLE(framework_control_sum);

extern bool USE_CACHE_DIR;
bool USE_CACHE_DIR = false;

@implementation CryptoProBridge

// MARK: - Helper functions

/// Create error with helpful data
void CryptoCustomError(NSError **errorOut,
                       NSString * _Nullable message,
                       NSString * _Nullable customCode)
{
    if (!errorOut) return;

    NSMutableDictionary *errorData = [NSMutableDictionary dictionary];

    if (customCode != NULL) errorData[@"customCode"] = customCode;
    if (message != NULL) errorData[NSLocalizedDescriptionKey] = message;
    errorData[@"stackTrace"] = [NSThread callStackSymbols];
    
    NSInteger cspErrorCode = CSP_GetLastError();

    *errorOut = [NSError errorWithDomain:@"crypto_pro_flutter"
                                    code:cspErrorCode
                                userInfo:errorData];
}

/// Check: if certificate has private key
BOOL CertHasPrivateKey(PCCERT_CONTEXT cert) {
    DWORD dwSize = 0;
    CRYPT_KEY_PROV_INFO *pKeyInfo = NULL;
    if (!CertGetCertificateContextProperty(cert, CERT_KEY_PROV_INFO_PROP_ID, 0, &dwSize)) {
        return FALSE;
    }
    pKeyInfo = (CRYPT_KEY_PROV_INFO *)malloc(dwSize);
    if (!pKeyInfo) {
        return FALSE;
    }
    if (!(CertGetCertificateContextProperty(cert, CERT_KEY_PROV_INFO_PROP_ID, pKeyInfo, &dwSize))) {
        free(pKeyInfo);
        return FALSE;
    }
    free(pKeyInfo);
    return TRUE;
}

HCERTSTORE OpenSystemStore(NSString *storeName) {
    return CertOpenSystemStore(0, storeName.UTF8String);
}

HCERTSTORE OpenRootStore(void) {
    return CertOpenStore(CERT_STORE_PROV_SYSTEM, 0, 0, CERT_SYSTEM_STORE_LOCAL_MACHINE | CERT_STORE_OPEN_EXISTING_FLAG, L"ROOT");
}

/// Check: if subject == issuer (is ROOT certificate)
BOOL CertIsSelfSigned(PCCERT_CONTEXT cert) {
    if (!cert || !cert->pCertInfo) return NO;

    if (!CertCompareCertificateName(
            X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
            &cert->pCertInfo->Subject,
            &cert->pCertInfo->Issuer
        )) {
        return NO;
    }
    
    DWORD flags = 0;
    BOOL ok = CertVerifySubjectCertificateContext(cert, cert, &flags);
    return ok;
}

const char* GetHashOid(PCCERT_CONTEXT pCert) {
    const char *pKeyAlg = pCert->pCertInfo->SubjectPublicKeyInfo.Algorithm.pszObjId;
    if (strcmp(pKeyAlg, szOID_CP_GOST_R3410EL) == 0)
    {
        return szOID_CP_GOST_R3411;
    }
    else if (strcmp(pKeyAlg, szOID_CP_GOST_R3410_12_256) == 0)
    {
        return szOID_CP_GOST_R3411_12_256;
    }
    else if (strcmp(pKeyAlg, szOID_CP_GOST_R3410_12_512) == 0)
    {
        return szOID_CP_GOST_R3411_12_512;
    }
    return NULL;
}

NSString * HexFromBlob(const BYTE *bytes, DWORD len) {
    if (!bytes || len == 0) return @"";
    NSMutableString *s = [NSMutableString stringWithCapacity:len * 2];
    for (DWORD i = 0; i < len; i++) {
        [s appendFormat:@"%02X", bytes[i]];
    }
    return s;
}

NSString * SerialNumberString(PCCERT_CONTEXT cert) {
    CRYPT_INTEGER_BLOB sn = cert->pCertInfo->SerialNumber;
    if (!sn.pbData || sn.cbData == 0) return @"";
    NSMutableData *rev = [NSMutableData dataWithLength:sn.cbData];
    BYTE *dst = (BYTE *)rev.mutableBytes;
    for (DWORD i = 0; i < sn.cbData; i++) dst[i] = sn.pbData[sn.cbData - 1 - i];
    return HexFromBlob(dst, (DWORD)rev.length);
}

NSString * NameString(PCCERT_CONTEXT cert, DWORD nameType, DWORD flags) {
    DWORD cch = CertGetNameStringW(cert, nameType, flags, NULL, NULL, 0);
    if (cch <= 1) return @"";
    NSMutableData *buf = [NSMutableData dataWithLength:cch * sizeof(wchar_t)];
    CertGetNameStringW(cert, nameType, flags, NULL, (wchar_t *)buf.mutableBytes, cch);
    return [NSString stringWithCharacters:(const unichar *)buf.bytes length:(cch - 1)];
}

NSString * SignatureAlgNameFromOID(NSString *oid) {
    if (oid.length == 0) return @"";
    PCCRYPT_OID_INFO info = CryptFindOIDInfo(CRYPT_OID_INFO_OID_KEY,
                                            (void *)oid.UTF8String,
                                            0);
    if (!info || !info->pwszName) return oid; // fallback: вернём OID
    return [NSString stringWithCharacters:(const unichar *)info->pwszName
                                   length:wcslen(info->pwszName)];
}

static NSString *FileTimeToISO8601(const FILETIME *ft) {
    if (!ft) return @"";
    SYSTEMTIME stUTC;
    if (!CSP_FileTimeToSystemTime(ft, &stUTC)) return @"";

    // YYYY-MM-DDTHH:MM:SSZ
    return [NSString stringWithFormat:@"%04d-%02d-%02dT%02d:%02d:%02dZ",
            stUTC.wYear, stUTC.wMonth, stUTC.wDay,
            stUTC.wHour, stUTC.wMinute, stUTC.wSecond];
}

/// Returns NSDictionary with fields: alias/owner/issuer/serialNumber/algorithm/version/oid/validFrom/validTo
NSDictionary * CertificateDictFromCertContext(PCCERT_CONTEXT cert) {
    if (!cert || !cert->pCertInfo) return nil;

    NSString *serial = SerialNumberString(cert);
    NSString *subject = NameString(cert, CERT_NAME_RDN_TYPE, 0);
    NSString *issuer  = NameString(cert, CERT_NAME_RDN_TYPE, CERT_NAME_ISSUER_FLAG);
    
    const char *oidChar = cert->pCertInfo->SignatureAlgorithm.pszObjId;
    NSString *oid = oidChar ? [NSString stringWithUTF8String:oidChar] : @"";
    NSString *algName = SignatureAlgNameFromOID(oid);

    long version = (long)cert->pCertInfo->dwVersion + 1;

    NSString *notBefore = FileTimeToISO8601(&cert->pCertInfo->NotBefore);
    NSString *notAfter  = FileTimeToISO8601(&cert->pCertInfo->NotAfter);

    return @{
        @"alias": serial ?: NULL,
        @"owner": subject ?: NULL,
        @"issuer": issuer ?: NULL,
        @"serialNumber": serial ?: NULL,
        @"algorithm": algName ?: NULL,
        @"version": @(version),
        @"oid": oid ?: NULL,
        @"validFrom": notBefore ?: NULL,
        @"validTo": notAfter ?: NULL,
    };
}

/// Find certificate with specific serial number
PCCERT_CONTEXT GetRecipientCert(HCERTSTORE hCertStore, NSString *serialNumber) {
    PCCERT_CONTEXT pCertContext = NULL;
    
    for (;;) {
        pCertContext = CertFindCertificateInStore(hCertStore, X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 0,
                                                      CERT_FIND_ANY, 0, pCertContext);
    
        if (!pCertContext) return NULL;
        if (!pCertContext->pCertInfo) continue;
        
        NSString * currentSerial = SerialNumberString(pCertContext);
        if ([currentSerial isEqual:serialNumber]) {
            return pCertContext;
        } else {
            continue;
        }
    }
}

BOOL AddCertToCaOrRoot(PCCERT_CONTEXT cert, HCERTSTORE caStore, HCERTSTORE rootStore) {
    if (!cert) return FALSE;

    HCERTSTORE dst = CertIsSelfSigned(cert) ? rootStore : caStore;

    if (!CertAddCertificateContextToStore(dst, cert, CERT_STORE_ADD_REPLACE_EXISTING, NULL)) {
        return FALSE;
    }
    
    return TRUE;
}

WCHAR * _Nullable ConvertToWideChar(NSString *value) {
    if (value == NULL) {
        return NULL;
    }
    
    NSUInteger len = [value length];

    unichar *tmp = (unichar *)calloc(len + 1, sizeof(unichar));
    [value getCharacters:tmp range:NSMakeRange(0, len)];
    tmp[len] = 0;

    WCHAR *wideValue = (WCHAR *)calloc(len + 1, sizeof(WCHAR));
    for (NSUInteger i = 0; i < len + 1; i++) {
        wideValue[i] = (WCHAR)tmp[i];
    }
    free(tmp);
    
    return wideValue;
}

// MARK: - Public methods

/// Get info about current license
- (NSDictionary * _Nullable)getLicenseInfo:(NSError **)error {
    uint32_t err = 0;
    uint32_t len = 256;
    char *buf = calloc(len, sizeof(char));

    err = license_get(buf, &len, NULL, 0);
    if (err == 234) {
        buf = realloc(buf, len);
        err = license_get(buf, &len, NULL, 0);
    }
    
    if (err != 0) {
        free(buf);
        CryptoCustomError(error, @"The method 'license_get' completed with an error.", @"get");
        return NULL;
    }
    
    int32_t validity = 0;
    int32_t daysToExpire = 0;

    err = license_check(
        buf,
        NULL,
        NULL,
        &validity,
        NULL,
        NULL,
        NULL,
        &daysToExpire,
        NULL
    );

    if (err != 0) {
        free(buf);
        CryptoCustomError(error, @"The method 'license_check' completed with an error.", NULL);
        return NULL;
    }

    NSMutableDictionary *result = [NSMutableDictionary dictionary];
    result[@"validity"] = @(validity);
    result[@"daysToExpire"] = @(daysToExpire);
    NSString *str = [NSString stringWithUTF8String:buf];
    result[@"serialNumber"] = str;
    
    free(buf);
    return result;
}

/// Set new license
- (NSDictionary *)setLicense:(nonnull NSString *)number error:(NSError **)error {
    uint32_t err;
    char * buf = (char *)number.UTF8String;
    
    int32_t validity = 0;
    err = license_check(
        buf,
        NULL,
        NULL,
        &validity,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL
    );
    
    if (err != 0) {
        CryptoCustomError(error, @"The method 'license_check' completed with an error.", NULL);
        return NULL;
    }
    
    if (validity < 0) {
        NSMutableDictionary *result = [NSMutableDictionary dictionary];
        result[@"success"] = @(NO);
        return result;
    }

    err = license_set(buf, NULL, 0);

    if (err != 0) {
        CryptoCustomError(error, @"The method 'license_set' completed with an error.", NULL);
        return NULL;
    }
    
    NSMutableDictionary *result = [NSMutableDictionary dictionary];
    result[@"success"] = @(YES);
    
    return result;
}

- (NSMutableArray<NSDictionary *> * _Nullable)importPFXContainer:(nonnull NSString *)filePath password:(nonnull NSString *)password newPassword:(NSString * _Nullable)newPassword error:(NSError *__autoreleasing  _Nullable * _Nullable)error {
    // MARK: - Convert password to WCHAR

    WCHAR *pass = ConvertToWideChar(password);
    
    // MARK: - Create data-BLOB with PFX-file
    
    NSData *fileData = [NSData dataWithContentsOfFile:filePath];
    CRYPT_DATA_BLOB blob;
    blob.cbData = (DWORD)[fileData length];
    blob.pbData = (BYTE *)fileData.bytes;
    
    if (!fileData || fileData.length == 0) {
        free(pass);
        CryptoCustomError(error, @"Failed to read PFX file.", NULL);
        return NULL;
    }
    
    // MARK: - Open PFX-store
    
    HCERTSTORE pfxStore = PFXImportCertStore(&blob, pass, PKCS12_IMPORT_SILENT);
    free(pass);
    
    if (!pfxStore) {
        DWORD errorCode = CSP_GetLastError();
        CryptoCustomError(error, @"The method 'PFXImportCertStore' completed with an error.", errorCode == 86 ? @"CustomWrongPasswordException" : NULL);
        return NULL;
    }
    
    // MARK: - Open system-stores
    
    HCERTSTORE myStore   = OpenSystemStore(@"MY");
    HCERTSTORE caStore   = OpenSystemStore(@"CA");
    HCERTSTORE rootStore = OpenRootStore();
    if (!myStore || !caStore || !rootStore) {
        CryptoCustomError(error, @"Failed to initialize one of the system stores.", NULL);
        if (myStore) CertCloseStore(myStore, 0);
        if (caStore) CertCloseStore(caStore, 0);
        if (rootStore) CertCloseStore(rootStore, 0);
        CertCloseStore(pfxStore, 0);
        return NULL;
    }
    
    // MARK: - Add certs from PFX-store to system stores
    
    NSMutableArray<NSDictionary *> *parsedCertificates = [NSMutableArray array];
    PCCERT_CONTEXT cert = NULL;
    
    while ((cert = CertFindCertificateInStore(pfxStore, X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 0,
                                             CERT_FIND_ANY, 0, cert)) != NULL) {

        BOOL hasKey = CertHasPrivateKey(cert);

        HCERTSTORE dst = NULL;

        if (hasKey) {
            
            // MARK: - If certificate has a key, set a password for the container
            
            HCRYPTPROV hKey = 0;
            DWORD dwKeySpec = 0;
            CSP_BOOL callerFree = FALSE;
            
            if (!CryptAcquireCertificatePrivateKey(
                    cert,
                    0,
                    NULL,
                    &hKey,
                    &dwKeySpec,
                    &callerFree))
            {
                CertCloseStore(myStore, 0);
                CertCloseStore(caStore, 0);
                CertCloseStore(rootStore, 0);
                CertCloseStore(pfxStore, 0);
                CryptoCustomError(error, @"The method 'CryptAcquireCertificatePrivateKey' completed with an error.", NULL);
                return NULL;
            }
            
            NSString *finalPassword = newPassword != NULL ? newPassword : password;
            CRYPT_PIN_PARAM pinParam;
            pinParam.type = CRYPT_PIN_PASSWD;
            const char *tmp = [finalPassword cStringUsingEncoding:NSUTF8StringEncoding];
            char *pin = tmp ? strdup(tmp) : NULL;
            pinParam.dest.passwd = pin;

            if (!CryptSetProvParam(hKey, PP_CHANGE_PIN,(BYTE *)&pinParam, 0)) {
                if (callerFree) {
                    CryptReleaseContext(hKey, 0);
                }
                CertCloseStore(myStore, 0);
                CertCloseStore(caStore, 0);
                CertCloseStore(rootStore, 0);
                CertCloseStore(pfxStore, 0);
                free(pin);
                CryptoCustomError(error, @"The method 'CryptSetProvParam' completed with an error.", NULL);
                return NULL;
            }
            
            if (callerFree) {
                CryptReleaseContext(hKey, 0);
            }
            free(pin);
            NSDictionary *certDict = CertificateDictFromCertContext(cert);
            [parsedCertificates addObject:certDict];
            dst = myStore;
        } else {
            if (CertIsSelfSigned(cert)) {
                dst = rootStore;
            } else {
                dst = caStore;
            }
        }

        if (!CertAddCertificateContextToStore(
                dst,
                cert,
                CERT_STORE_ADD_REPLACE_EXISTING,
                NULL
            )) {
                CertCloseStore(myStore, 0);
                CertCloseStore(caStore, 0);
                CertCloseStore(rootStore, 0);
                CertCloseStore(pfxStore, 0);
                CryptoCustomError(error, @"The method 'CertAddCertificateContextToStore' completed with an error.", NULL);
                return NULL;
        }
    }
    
    // MARK: - Release resources and serialize response

    CertCloseStore(myStore, 0);
    CertCloseStore(caStore, 0);
    CertCloseStore(rootStore, 0);
    
    CertCloseStore(pfxStore, 0);
    return parsedCertificates;
}

- (NSString * _Nullable)signHashWith:(PCCERT_CONTEXT)certificateContext
               contentToSign:(NSString * _Nullable)contentToSign
           contentToSignPath:(NSString * _Nullable)contentToSignPath
                 cadesFormat:(NSInteger)cadesFormat
                      tsaUrl:(NSString * _Nullable)tsaUrl
                       error:(NSError **)error {
    // MARK: - Set params
    
    CRYPT_SIGN_MESSAGE_PARA signPara = {sizeof(signPara)};
    signPara.dwMsgEncodingType = X509_ASN_ENCODING | PKCS_7_ASN_ENCODING;
    signPara.pSigningCert = certificateContext;
    signPara.HashAlgorithm.pszObjId = (LPSTR)GetHashOid(certificateContext);

    CADES_SERVICE_CONNECTION_PARA tspConnectionPara = {
            sizeof(tspConnectionPara)};
    WCHAR *tsaWideChar = ConvertToWideChar(tsaUrl);
    if (tsaWideChar) {
        tspConnectionPara.wszUri = (LPCWSTR)tsaWideChar;
    }

    CADES_SIGN_PARA cadesSignPara = {sizeof(cadesSignPara)};
    DWORD cadesFormatDword = cadesFormat == 0 ? CADES_BES : CADES_X_LONG_TYPE_1;
    cadesSignPara.dwCadesType = cadesFormatDword;
    if (cadesFormatDword == CADES_X_LONG_TYPE_1) {
        cadesSignPara.pTspConnectionPara = &tspConnectionPara;
    }

    CADES_SIGN_MESSAGE_PARA para = {sizeof(para)};
    para.pSignMessagePara = &signPara;
    para.pCadesSignPara = &cadesSignPara;
    
    PCRYPT_DATA_BLOB pSignedMessage = 0;
    
    // MARK: - Release resources helper block
    
    void (^releaseResources)(void) = ^{
        if (tsaWideChar) {
            free(tsaWideChar);
        }
        if (pSignedMessage) {
            CadesFreeBlob(pSignedMessage);
        }
    };
    
    // MARK: - Create data to sign
    
    NSData *hashData = NULL;
    
    if (contentToSign != NULL) {
        hashData = [[NSData alloc] initWithBase64EncodedString:contentToSign
                                                options:NSDataBase64DecodingIgnoreUnknownCharacters];

    } else {
        NSData *fileData = [NSData dataWithContentsOfFile:contentToSignPath];
        hashData = [[NSData alloc] initWithBase64EncodedData:fileData
                                          options:NSDataBase64DecodingIgnoreUnknownCharacters];
    }
    // MARK: - Sign hash
    
    if (!CadesSignHash(&para,
                       (const BYTE *)hashData.bytes,
                       (DWORD)hashData.length,
                       szOID_RSA_data,
                       &pSignedMessage))
    {
        releaseResources();
        CryptoCustomError(error, @"The method 'CadesSignHash' completed with an error.", NULL);
        return NULL;
    }
    
    NSData *sig = [NSData dataWithBytes:pSignedMessage->pbData length:pSignedMessage->cbData];
    NSString *b64Signature = [sig base64EncodedStringWithOptions:0];
    
    releaseResources();
    
    return b64Signature;
}

- (NSString * _Nullable)sign:(NSString *)certificateAlias
               contentToSign:(NSString * _Nullable)contentToSign
           contentToSignPath:(NSString * _Nullable)contentToSignPath
                    detached:(BOOL)detached
                    signHash:(BOOL)signHash
     disableOnlineValidation:(BOOL)disableOnlineValidation
                 cadesFormat:(NSInteger)cadesFormat
                 storageName:(NSString * _Nullable)storageName
                    password:(NSString *)password
                      tsaUrl:(NSString * _Nullable)tsaUrl
                       error:(NSError **)error {
    // MARK: - Set sign message params
    
    HCERTSTORE hStoreHandle = NULL;
    PCCERT_CONTEXT context = NULL;
    HCRYPTPROV hKey = 0;
    DWORD dwKeySpec = 0;
    CSP_BOOL callerFree = FALSE;
    WCHAR *tsaWideChar = ConvertToWideChar(tsaUrl);
    
    CMSG_SIGNER_ENCODE_INFO signer;
    ZeroMemory(&signer, sizeof(signer));
    signer.cbSize   = sizeof(signer);

    CMSG_SIGNED_ENCODE_INFO signedInfo;
    ZeroMemory(&signedInfo, sizeof(signedInfo));
    signedInfo.cbSize   = sizeof(signedInfo);
    signedInfo.cSigners = 1;
    signedInfo.rgSigners = &signer;

    CADES_ENCODE_INFO cadesInfo;
    ZeroMemory(&cadesInfo, sizeof(cadesInfo));
    cadesInfo.dwSize = sizeof(cadesInfo);
    cadesInfo.pSignedEncodeInfo = &signedInfo;
    
    DWORD msgFlags = detached ? CMSG_DETACHED_FLAG : 0;
    
    HCRYPTMSG hMsg = NULL;
    
    // MARK: - Release resources helper block
    
    void (^releaseResources)(void) = ^{
        if (tsaWideChar) {
            free(tsaWideChar);
        }
        if (hMsg) {
            CryptMsgClose(hMsg);
        }
        if (hKey && callerFree) {
            CryptReleaseContext(hKey, 0);
        }
        if (context) CertFreeCertificateContext(context);
        if (hStoreHandle) CertCloseStore(hStoreHandle, 0);
    };
    
    // MARK: - Get certificate from store
    
    // Открываем хранилище личных сертификатов
    hStoreHandle = OpenSystemStore(@"MY");
    if (!hStoreHandle) {
        releaseResources();
        CryptoCustomError(error, @"Failed to open 'MY' system store.", NULL);
        return NULL;
    }
    // Получаем сертификат для подписания
    context = GetRecipientCert(hStoreHandle, certificateAlias);
    if (context == NULL) {
        releaseResources();
        CryptoCustomError(error, @"Did not find certificate with given alias in 'MY' system store.", NULL);
        return NULL;
    }
    
    // MARK: - Get private key and check password

    if (!CryptAcquireCertificatePrivateKey(
            context,
            CRYPT_ACQUIRE_SILENT_FLAG,
            NULL,
            &hKey,
            &dwKeySpec,
            &callerFree))
    {
        releaseResources();
        CryptoCustomError(error, @"The method 'CryptAcquireCertificatePrivateKey' completed with an error.", NULL);
        return NULL;
    }
    
    const char *tmp = [password cStringUsingEncoding:NSUTF8StringEncoding];
    char *pin = tmp ? strdup(tmp) : NULL;
    
    if (!CryptSetProvParam(hKey, PP_KEYEXCHANGE_PIN,(BYTE *)pin, 0)) {
        free(pin);
        releaseResources();
        CryptoCustomError(error, @"The method 'CryptSetProvParam' completed with an error.", @"CustomWrongPasswordException");
        return NULL;
    }
    free(pin);
    
    // MARK: - Sign hash (if needed)
    
    if (signHash) {
        NSString *result = [self signHashWith:context contentToSign:contentToSign contentToSignPath:contentToSignPath cadesFormat:cadesFormat tsaUrl:tsaUrl error:error];
        releaseResources();
        return result;
    }
    
    // MARK: - Set signing params
    
    signer.pCertInfo = context->pCertInfo;
    signer.hCryptProv = (HCRYPTPROV)hKey;
    signer.dwKeySpec = dwKeySpec;
    signer.HashAlgorithm.pszObjId = (LPSTR)GetHashOid(context);
    
    hMsg = CadesMsgOpenToEncode(
        X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
        msgFlags,
        &cadesInfo,
        0,
        0
    );
    
    if (!hMsg) {
        releaseResources();
        CryptoCustomError(error, @"CadesMsgOpenToEncode failed", NULL);
        return NULL;
    }
    
    // MARK: - Signing (CADES-BES)
    
    if (contentToSign != NULL) {
        NSData *data = [contentToSign dataUsingEncoding:NSUTF8StringEncoding];
        
        if (!CryptMsgUpdate(hMsg,
                            (const BYTE *)data.bytes,
                            (DWORD)data.length,
                            TRUE))
        {
            releaseResources();
            CryptoCustomError(error, @"The method 'CryptMsgUpdate' completed with an error.", NULL);
            return NULL;
        }
    } else {
        if (contentToSignPath == NULL) {
            releaseResources();
            CryptoCustomError(error, @"contentToSign == NULL && contentToSignPath == NULL", NULL);
            return NULL;
        }
        
        NSFileHandle *fh = [NSFileHandle fileHandleForReadingAtPath:contentToSignPath];
        if (!fh) {
            releaseResources();
            CryptoCustomError(error, @"NSFileHandle == NULL", NULL);
            return NULL;
        }

        const NSUInteger chunkSize = 64 * 1024;
        @autoreleasepool {
            while (true) {
                NSData *chunk = [fh readDataOfLength:chunkSize];
                
                BOOL isLast = chunk.length < chunkSize;
                
                if (!CryptMsgUpdate(hMsg,
                                    (const BYTE *)chunk.bytes,
                                    (DWORD)chunk.length,
                                    isLast))
                {
                    [fh closeFile];
                    releaseResources();
                    CryptoCustomError(error, @"The method 'CryptMsgUpdate' completed with an error.", NULL);
                    return NULL;
                }
                
                if (isLast) {
                    break;
                }
            }
            [fh closeFile];
        }
    }
    
    // MARK: - Enhance to CADES-X Long Type 1 (if needed)
    
    if (cadesFormat == 1 && tsaUrl != NULL) {
        CADES_SERVICE_CONNECTION_PARA tspConnectionPara = {sizeof(tspConnectionPara)};
        tspConnectionPara.wszUri = (LPCWSTR)tsaWideChar;

        CADES_SIGN_PARA signPara = { sizeof(signPara) };
        signPara.dwCadesType = CADES_X_LONG_TYPE_1;
        signPara.pTspConnectionPara = &tspConnectionPara;
        
        if(!CadesMsgEnhanceSignature(hMsg, 0, &signPara))
        {
            releaseResources();
            CryptoCustomError(error, @"The method 'CadesMsgEnhanceSignature' completed with an error.", NULL);
            return NULL;
        }
    }
    
    // MARK: - Get signature

    DWORD outSize = 0;
    if (!CryptMsgGetParam(hMsg, CMSG_CONTENT_PARAM, 0, NULL, &outSize)) {
        releaseResources();
        CryptoCustomError(error, @"The method 'CryptMsgGetParam' completed with an error.", NULL);
        return NULL;
    }

    NSMutableData *signature = [NSMutableData dataWithLength:outSize];
    if (!CryptMsgGetParam(hMsg, CMSG_CONTENT_PARAM, 0, signature.mutableBytes, &outSize)) {
        releaseResources();
        CryptoCustomError(error, @"The method 'CryptMsgGetParam' completed with an error.", NULL);
        return NULL;
    }
    signature.length = outSize;

    NSString *base64 = [signature base64EncodedStringWithOptions:0];
    releaseResources();
    
    return base64;
}

- (NSMutableArray<NSDictionary *> * _Nullable)getInstalledCertificatesWithPrivateKey:(NSError *__autoreleasing  _Nullable * _Nullable)error {
    HCERTSTORE myStore = OpenSystemStore(@"MY");
    if (myStore == NULL) {
        CryptoCustomError(error, @"Failed to initialize system store.", NULL);
        return FALSE;
    }
    
    NSMutableArray<NSDictionary *> *parsedCertificates = [NSMutableArray array];
    PCCERT_CONTEXT cert = NULL;
    
    while ((cert = CertFindCertificateInStore(myStore, X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 0,
                                             CERT_FIND_ANY, 0, cert)) != NULL) {
        if (CertHasPrivateKey(cert)) {
            NSDictionary *certDict = CertificateDictFromCertContext(cert);
            [parsedCertificates addObject:certDict];
        }
    }
    
    CertCloseStore(myStore, 0);
    
    return parsedCertificates;
}

- (BOOL)deleteCertificateFromPersonalStore:(nonnull NSString *)certificateAlias error:(NSError *__autoreleasing  _Nullable * _Nullable)error {
    HCERTSTORE myStore = OpenSystemStore(@"MY");
    if (myStore == NULL) {
        CryptoCustomError(error, @"Failed to initialize system store.", NULL);
        return FALSE;
    }
    
    PCCERT_CONTEXT certToDelete = GetRecipientCert(myStore, certificateAlias);
    if (certToDelete == NULL) {
        CertCloseStore(myStore, 0);
        CryptoCustomError(error, [NSString stringWithFormat:@"Can not found certificate with alias: %@", certificateAlias], NULL);
        return FALSE;
    }
    if (!CertDeleteCertificateFromStore(certToDelete)) {
        CertCloseStore(myStore, 0);
        CryptoCustomError(error, @"The method 'CertDeleteCertificateFromStore' completed with an error.", NULL);
        return FALSE;
    }
    
    CertCloseStore(myStore, 0);
    return TRUE;
}

- (BOOL)addCertificatesToTrustedStore:(nonnull NSArray<NSString *> *)certificatesPaths error:(NSError *__autoreleasing  _Nullable * _Nullable)error {
    HCERTSTORE caStore = OpenSystemStore(@"CA");
    HCERTSTORE rootStore = OpenRootStore();
    
    if (!caStore || !rootStore) {
        if (caStore) CertCloseStore(caStore, 0);
        if (rootStore) CertCloseStore(rootStore, 0);
        CryptoCustomError(error, @"Failed to initialize one of the system stores.", NULL);
        return FALSE;
    }
    
    NSMutableArray<NSString *> * errorList = [NSMutableArray array];
    
    for (NSString *path in certificatesPaths) {
        NSData *data = [NSData dataWithContentsOfFile:path];
        if (!data) {
            [errorList addObject:path];
            continue;
        }

        CRYPT_DATA_BLOB blob;
        blob.cbData = (DWORD)data.length;
        blob.pbData = (BYTE *)data.bytes;

        DWORD enc = 0;
        DWORD contentType = 0;
        DWORD formatType = 0;
        __block HCERTSTORE hStoreFromObj = NULL;
        
        void (^releaseResources)(void) = ^{
            if (hStoreFromObj) {
                CertCloseStore(hStoreFromObj, 0);
            }
            hStoreFromObj = NULL;
        };

        BOOL ok = CryptQueryObject(
            CERT_QUERY_OBJECT_BLOB,
            &blob,
            CERT_QUERY_CONTENT_FLAG_CERT |
            CERT_QUERY_CONTENT_FLAG_PKCS7_SIGNED |
            CERT_QUERY_CONTENT_FLAG_PKCS7_SIGNED_EMBED |
            CERT_QUERY_CONTENT_FLAG_PKCS7_UNSIGNED,
            CERT_QUERY_FORMAT_FLAG_BINARY |
            CERT_QUERY_FORMAT_FLAG_BASE64_ENCODED,
            0,
            &enc,
            &contentType,
            &formatType,
            &hStoreFromObj,
            NULL,
            NULL
        );

        if (!ok) {
            [errorList addObject:path];
            releaseResources();
            continue;
        }

        if (hStoreFromObj) {
            PCCERT_CONTEXT ctx = NULL;
            while ((ctx = CertEnumCertificatesInStore(hStoreFromObj, ctx)) != NULL) {
                if (!AddCertToCaOrRoot(ctx, caStore, rootStore)) {
                    [errorList addObject:path];
                }
            }
        }
        
        releaseResources();
    }
    
    CertCloseStore(caStore, 0);
    CertCloseStore(rootStore, 0);
    
    if (errorList.count > 0) {
        CryptoCustomError(error, [NSString stringWithFormat:@"Failed to add this certificates: %@", [errorList componentsJoinedByString:@", "]], @"SomeCertificatesAreNotAddedToTrustStoreException");
    }
    
    return TRUE;
}

@end
