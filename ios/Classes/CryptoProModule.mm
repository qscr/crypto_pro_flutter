//
//  CryptoProModule.mm
//  crypto_pro_flutter
//
//  Created by Maxim Petrov on 25.11.2022.
//

#import "CryptoProModule.h"

extern bool USE_CACHE_DIR;
bool USE_CACHE_DIR = false;

#import <CPROCSP/CPROCSP.h>
#import <CPROCSP/CPCrypt.h>
//#import <CPROPKI/cades.h>
#import <CPROCSP/DisableIntegrity.h>

/// Инициализация контекста
static HCRYPTPROV phProv = 0;

NSString* getError() {
    DWORD error = CSP_GetLastError();
    
    printf("\nОшибка\n");
    printf("%d\n", error);
    
    wchar_t buf[256];
    CSP_FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                       NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                       buf, (sizeof(buf) / sizeof(wchar_t)), NULL);
    printf("%ls\n", buf);
    
    return [[NSString alloc] initWithBytes:buf length:wcslen(buf)*sizeof(*buf) encoding:NSUTF32LittleEndianStringEncoding];
}

NSString* getErrorResponse(NSString* message) {
    NSDictionary *map = [[NSDictionary alloc] initWithObjectsAndKeys: @(false), @"success", message, @"message", getError(), @"exception", nil];
    NSData *jsonData = [NSJSONSerialization dataWithJSONObject:map
                                                       options:NSJSONWritingPrettyPrinted
                                                         error:NULL];
    
    return [[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];
}

wchar_t *convertCharArrayToLPCWSTR(const char* charArray)
{
    /// освободить память после new
    wchar_t* wString=new wchar_t[4096];
    MultiByteToWideChar(CP_ACP, 0, charArray, -1, wString, 4096);
    return wString;
}

/// Инициализация провайдера и получение списка контейнеров
bool initCSP()
{
    printf("\nИнициализация провайдера и получение списка контейнеров\n\n");
    
    DisableIntegrityCheck();
    
    
    if (!CryptAcquireContextA(&phProv, NULL, NULL, PROV_GOST_2012_256, CRYPT_VERIFYCONTEXT)) {
        DWORD error = CSP_GetLastError();
        if(CSP_GetLastError() != (DWORD)NTE_BAD_KEYSET &&
           CSP_GetLastError() != (DWORD)NTE_KEYSET_NOT_DEF)
        {
            printf("A general error occurred running CryptAcquireContext");
        }
        return false;
    }
    
    printf("\nКонтекст инициализирован\n");
    printf("Context HCRYPTPROV = %d\n", (LONG)phProv);
    
    /// Получение списка контейнеров
    DWORD pdwDataLen = 0;
    DWORD flag = 1;
    
    printf("\nПолучение списка контейнеров\n\n");
    if (!CryptGetProvParam(phProv, PP_ENUMCONTAINERS, NULL, &pdwDataLen, flag)) {
        DWORD error = CSP_GetLastError();
        if (error == ERROR_NO_MORE_ITEMS) {
            printf("Список контейнеров пуст\n");
            CryptReleaseContext(phProv, 0);
            return true;
        }
        
        printf("Не удалось получить список контейнеров CryptGetProvParam (PP_ENUMCONTAINERS)\n");
        CryptReleaseContext(phProv, 0);
        return false;
    }
    
    BYTE* data = (BYTE*)malloc(pdwDataLen);
    
    int i = 1;
    
    while (CryptGetProvParam(phProv, PP_ENUMCONTAINERS, data, &pdwDataLen, flag)) {
        printf("\nКонтейнер #%d\n", i);
        printf("%s\n", data);
        flag = 2;
        i++;
    };
    
    free(data);
    CryptReleaseContext(phProv, 0);
    
    return true;
}

/// Получить хранилище личных сертификатов
HCERTSTORE getMyStore() {
    return CertOpenStore(CERT_STORE_PROV_SYSTEM,0, NULL, CERT_SYSTEM_STORE_LOCAL_MACHINE, L"MY");
}

int arrSize(const char *a) {
  int it = 0;
  int size = 0;
  while (a[it] != '\0') {
    ++size;
    ++it;
  }
  return size;
}

NSString* addCertificate(NSString* pathToCert, NSString* password) {
    const char *pathToCertFileChar = [pathToCert UTF8String];
    const char *passwordChar = [password UTF8String];
    
    NSString* result;
    NSString* containerName;
    
    printf("\nУстановка контейнера\n\n");
    
    CRYPT_DATA_BLOB certBlob;
    
    FILE *file = fopen(pathToCertFileChar, "rb");
    fseek(file, 0, SEEK_END);
    certBlob.cbData = (DWORD)ftell(file);
    fseek(file, 0, SEEK_SET);
    certBlob.pbData = (BYTE*)malloc(certBlob.cbData);
    fread(certBlob.pbData, 1, certBlob.cbData, file);
    fclose(file);
    
    LPCWSTR passwordL = convertCharArrayToLPCWSTR(passwordChar);
    
    /// Хранилище для личных сертификатов
    HCERTSTORE myStore = getMyStore();
    
    /// Хранилище с сертификатами, которые необходимо установить
    HCERTSTORE certStore = PFXImportCertStore(&certBlob, passwordL, CRYPT_SILENT | CRYPT_EXPORTABLE);
    
    free((void*)passwordL);
    
    if (!certStore)
        return getErrorResponse(@"Не удалось добавить контейнер закрытого ключа (PFXImportCertStore).\nПроверьте правильность введенного пароля");
    printf("\nКонтейнер успешно добавлен\n\n");
    
    /// Вывод информации о сертификате
    PCCERT_CONTEXT certWithPrivateKey = NULL;
    
    /// Вывод информации о сертификате
    PCCERT_CONTEXT pPrevCertContext = NULL;
    printf("Информация о сертификатах в конейнере\n");
    
    do {
        pPrevCertContext = CertEnumCertificatesInStore(certStore, pPrevCertContext);
        if (pPrevCertContext != NULL) {
            DWORD csz = CertNameToStrA(X509_ASN_ENCODING, &pPrevCertContext->pCertInfo->Subject, CERT_SIMPLE_NAME_STR, NULL, 0);
            LPSTR psz = (LPSTR)malloc(csz);
            
            CertNameToStrA(X509_ASN_ENCODING, &pPrevCertContext->pCertInfo->Subject, CERT_SIMPLE_NAME_STR, psz, csz);
            
            if (!CertAddCertificateContextToStore(myStore, pPrevCertContext, CERT_STORE_ADD_REPLACE_EXISTING, NULL)) {
                printf("Сертификат: %s\n", psz, "добавлен в личное хранилище неуспешно");
            }
            
            printf("Сертификат: %s\n", psz);
            
            HCRYPTPROV hProv = 0;
            if (CryptAcquireCertificatePrivateKey(pPrevCertContext, CRYPT_ACQUIRE_SILENT_FLAG, NULL, &hProv, NULL, NULL)) {
                certWithPrivateKey = pPrevCertContext;
                CertAddCertificateContextToStore(myStore, pPrevCertContext, CERT_STORE_ADD_REPLACE_EXISTING, NULL);
                
                printf("Сертификат связанный с приватным ключем: %s\n", psz);
                
                NSData *certificateEncodedData = [NSData dataWithBytes:pPrevCertContext->pbCertEncoded length:pPrevCertContext->cbCertEncoded];
                NSString *base64Certificate = [certificateEncodedData base64EncodedStringWithOptions:0];
                
                DWORD pdwDataLen = 0;
                CryptGetProvParam(hProv, PP_CONTAINER, NULL, &pdwDataLen, NULL);
                BYTE* data = (BYTE*)malloc(pdwDataLen);
                CryptGetProvParam(hProv, PP_CONTAINER, data, &pdwDataLen, NULL);
                const char *ch = reinterpret_cast<const char*>(data);
                containerName = [NSString stringWithFormat:@"%s", ch];
                NSDictionary *map = [[NSDictionary alloc] initWithObjectsAndKeys:base64Certificate, @"certificate", containerName, @"alias", @(true), @"success", nil];
                NSData *jsonData = [NSJSONSerialization dataWithJSONObject:map
                                                                   options:NSJSONWritingPrettyPrinted
                                                                     error:NULL];
                
                result = [[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];
            }
            
            free(psz);
        }
    } while (pPrevCertContext != NULL);
    
    /// Закрытие certStore и дескриптора сертификата
    if (certStore) CertCloseStore(certStore, CERT_CLOSE_STORE_FORCE_FLAG);
    if (pPrevCertContext) CertFreeCertificateContext(pPrevCertContext);
    /// Закрытие myStore
    if (myStore) CertCloseStore(myStore, CERT_CLOSE_STORE_FORCE_FLAG);
    
//    HCRYPTPROV hProv = 0;
//    NSString *pathString = [@"\\\\.\\HDIMAGE\\" stringByAppendingString:containerName];
//    const char *path = [pathString UTF8String];
//    if(!CryptAcquireContext(
//                            &hProv,
//                            _TEXT(path),
//                            NULL,
//                            PROV_GOST_2012_256,
//                            CRYPT_SILENT))
//    {
//        return getErrorResponse(@"Не удалось Закрытие certStore и дескриптора сертификата (CryptAcquireContext)");
//    }
    
    //--------------------------------------------------------------------
    // Установка параметров в соответствии с паролем.
    
//    printf("\nУстановка пароля на ключевой контейнер\n\n");
//
//    CRYPT_PIN_PARAM param;
//    param.type = CRYPT_PIN_PASSWD;
//    param.dest.passwd = (char*)passwordChar;
//
//    if(!CryptSetProvParam(
//                          hProv,
//                          PP_CHANGE_PIN,
//                          (BYTE*)&param,
//                          0))
//    {
//        return getErrorResponse(@"Не удалось установить пароль на ключевой контейнер (CryptSetProvParam (PP_CHANGE_PIN))");
//    }
//
//    return result;
    return result;
}



NSString* getInstalledCertificate() {
    HCERTSTORE myStore = getMyStore();
    
    /// Сертификат с приватным ключем
    PCCERT_CONTEXT certWithPrivateKey = NULL;
    
    /// Вывод информации о сертификате
    PCCERT_CONTEXT pPrevCertContext = NULL;
    
    do {
        pPrevCertContext = CertEnumCertificatesInStore(myStore, pPrevCertContext);
        if (pPrevCertContext != NULL) {
            DWORD csz = CertNameToStrA(X509_ASN_ENCODING, &pPrevCertContext->pCertInfo->Subject, CERT_SIMPLE_NAME_STR, NULL, 0);
            LPSTR psz = (LPSTR)malloc(csz);
            
            CertNameToStrA(X509_ASN_ENCODING, &pPrevCertContext->pCertInfo->Subject, CERT_SIMPLE_NAME_STR, psz, csz);
            
            printf("Сертификат: %s\n", psz);
            
            HCRYPTPROV hProv = 0;
            if (CryptAcquireCertificatePrivateKey(pPrevCertContext, CRYPT_ACQUIRE_SILENT_FLAG, NULL, &hProv, NULL, NULL)) {
                certWithPrivateKey = pPrevCertContext;
                printf("Сертификат связанный с приватным ключем: %s\n", psz);
                
                NSData *certificateEncodedData = [NSData dataWithBytes:pPrevCertContext->pbCertEncoded length:pPrevCertContext->cbCertEncoded];
                NSString *base64Certificate = [certificateEncodedData base64EncodedStringWithOptions:0];
                
                DWORD pdwDataLen = 0;
                CryptGetProvParam(hProv, PP_CONTAINER, NULL, &pdwDataLen, NULL);
                BYTE* data = (BYTE*)malloc(pdwDataLen);
                CryptGetProvParam(hProv, PP_CONTAINER, data, &pdwDataLen, NULL);
                const char *ch = reinterpret_cast<const char*>(data);
            }
            
            free(psz);
            CryptReleaseContext(hProv, 0);
        }
    } while (pPrevCertContext != NULL);
    

    return @"";
}

NSString* sign() {
    NSString* alias = @"pfx-be11526e-ebdb-7d96-370a-92959cf1c761";
    NSString* password = @"";
    NSString* digest = @"TEST";
    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;
    
    BYTE *pbSignature = NULL;
    DWORD cbSignature;
    
    const char *dataChar = [digest UTF8String];
    DWORD cbHash;
    CryptStringToBinaryA(dataChar, strlen(dataChar), CRYPT_STRING_BASE64, NULL, &cbHash, 0, NULL);
    BYTE* pbHash = (BYTE*)malloc(cbHash);
    CryptStringToBinaryA(dataChar, strlen(dataChar), CRYPT_STRING_BASE64, pbHash, &cbHash, 0, NULL);
    
    printf("Получение дескриптора провайдера\n");
    NSString *pathString = [@"\\\\.\\HDIMAGE\\" stringByAppendingString:alias];
    const char *path = [pathString UTF8String];
    if(!CryptAcquireContext(
                            &hProv,
                            _TEXT(path),
                            NULL,
                            PROV_GOST_2012_256,
                            CRYPT_SILENT))
    {
        return getErrorResponse(@"Не удалось получить дескриптор провайдера (CryptAcquireContext)");
    }
    
    printf("Установка параметров в соответствии с паролем\n");
    const char *passwordChar = [password UTF8String];
    
    if(!CryptSetProvParam(
                          hProv,
                          PP_KEYEXCHANGE_PIN,
                          (BYTE*)passwordChar,
                          0))
    {
        return getErrorResponse(@"Не удалось установить параметры в соответствии с паролем (CryptSetProvParam(PP_KEYEXCHANGE_PIN))");
    }
    
    printf("Создание объекта функции хэширования\n");
    if(!CryptCreateHash(
                        hProv,
                        CALG_GR3411_2012_256,
                        0,
                        0,
                        &hHash))
    {
        return getErrorResponse(@"Не удалось создать объект функции хэширования (CryptCreateHash)");
    }
    
    
    if(!CryptSetHashParam(hHash,
                          HP_HASHVAL,
                          pbHash,
                          0))
    {
        return getErrorResponse(@"Не удалось установить хэш у дескриптора хэширования (CryptSetHashParam(HP_HASHVAL))");
    }
    
    //--------------------------------------------------------------------
    // Определение размера подписи и распределение памяти.
    if(!CryptSignHash(
                      hHash,
                      AT_KEYEXCHANGE,
                      NULL,
                      0,
                      NULL,
                      &cbSignature))
    {
        return getErrorResponse(@"Не удалось определить размер подписи (CryptSignHash)");
    }
    
    //--------------------------------------------------------------------
    // Распределение памяти под буфер подписи.
    
    pbSignature = (BYTE *)malloc(cbSignature);
    
    if(!pbSignature)
    {
        return getErrorResponse(@"Out of memmory");
    }
    
    // Подпись объекта функции хэширования.
    printf("Подпись объекта функции хэширования\n");
    if(!CryptSignHash(
                      hHash,
                      AT_KEYEXCHANGE,
                      NULL,
                      0,
                      pbSignature,
                      &cbSignature))
    {
        return getErrorResponse(@"Не удалось подписать объект функции хэширования (CryptSignHash)");
    }
    
    DWORD signatureBase64Len;
    CryptBinaryToStringA(pbSignature, cbSignature, CRYPT_STRING_BASE64, NULL, &signatureBase64Len);
    LPSTR signatureBase64String = (char*)malloc(signatureBase64Len);
    CryptBinaryToStringA(pbSignature, cbSignature, CRYPT_STRING_BASE64, signatureBase64String, &signatureBase64Len);
    
    printf("Сигнатура: ");
    printf("%s", signatureBase64String);
    
    NSDictionary *map = [[NSDictionary alloc] initWithObjectsAndKeys: @(true), @"success", digest, @"digest", [NSString stringWithUTF8String:signatureBase64String], @"signature", @"CALG_GR3410_12_256", @"signatureAlgorithm", nil];
    NSData *jsonData = [NSJSONSerialization dataWithJSONObject:map
                                                       options:NSJSONWritingPrettyPrinted
                                                         error:NULL];
    
    // Освобождение памяти
    if(pbHash)
        free(pbHash);
    if(pbSignature)
        free(pbSignature);
    
    // Уничтожение объекта функции хэширования.
    if(hHash)
        CryptDestroyHash(hHash);
    
    // Освобождение дескриптора провайдера.
    if(hProv)
        CryptReleaseContext(hProv, 0);
    
    return [[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];
}


NSString* signMessage(NSString* message, NSString *alias, NSString* password, bool isDetached, bool signHash) {
//    sign();
    HCERTSTORE myStore = getMyStore();

    /// Сертификат с приватным ключем
    PCCERT_CONTEXT certWithPrivateKey = NULL;

    /// Вывод информации о сертификате
    PCCERT_CONTEXT pPrevCertContext = NULL;

    do {
        pPrevCertContext = CertEnumCertificatesInStore(myStore, pPrevCertContext);
        if (pPrevCertContext != NULL) {
            DWORD csz = CertNameToStrA(X509_ASN_ENCODING, &pPrevCertContext->pCertInfo->Subject, CERT_SIMPLE_NAME_STR, NULL, 0);
            LPSTR psz = (LPSTR)malloc(csz);

            CertNameToStrA(X509_ASN_ENCODING, &pPrevCertContext->pCertInfo->Subject, CERT_SIMPLE_NAME_STR, psz, csz);

            printf("Сертификат: %s\n", psz);

            HCRYPTPROV hProv = 0;
            if (CryptAcquireCertificatePrivateKey(pPrevCertContext, CRYPT_ACQUIRE_SILENT_FLAG, NULL, &hProv, NULL, NULL)) {
                certWithPrivateKey = pPrevCertContext;
                printf("Сертификат связанный с приватным ключем: %s\n", psz);

                NSData *certificateEncodedData = [NSData dataWithBytes:pPrevCertContext->pbCertEncoded length:pPrevCertContext->cbCertEncoded];
                NSString *base64Certificate = [certificateEncodedData base64EncodedStringWithOptions:0];

                DWORD pdwDataLen = 0;
                CryptGetProvParam(hProv, PP_CONTAINER, NULL, &pdwDataLen, NULL);
                BYTE* data = (BYTE*)malloc(pdwDataLen);
                CryptGetProvParam(hProv, PP_CONTAINER, data, &pdwDataLen, NULL);
                break;
            }
        }
    } while (pPrevCertContext != NULL);


    // Получили сертификат с приватным ключем

    CRYPT_DATA_BLOB messageBlob;
    DWORD cbsignMessage = 0;
    CRYPT_DATA_BLOB signedMessage;
    CRYPT_SIGN_MESSAGE_PARA sigParams;
    

    const char *messageArr = [message UTF8String];

    // String to ByteArray
//    messageBlob.cbData = (DWORD) arrSize(messageArr);
//    messageBlob.pbData = new BYTE[messageBlob.cbData];
//    memcpy(messageBlob.pbData, messageArr, messageBlob.cbData);
    
    BYTE* pbMessage = (BYTE*)messageArr;
    DWORD cbMessage = (strlen((CHAR*)pbMessage));
    
    PCCERT_CONTEXT* arrayOfCertificates[] = {&certWithPrivateKey};
    
    PCCRYPT_OID_INFO pOidInfo = CPCryptGetDefaultHashOIDInfo(certWithPrivateKey->pCertInfo->SubjectPublicKeyInfo.Algorithm.pszObjId);

    // Initialize the signature structure.
    ZeroMemory(&sigParams, sizeof(CRYPT_SIGN_MESSAGE_PARA));
    sigParams.cbSize = sizeof(CRYPT_SIGN_MESSAGE_PARA);
    sigParams.dwMsgEncodingType = X509_ASN_ENCODING | PKCS_7_ASN_ENCODING;
    sigParams.pvHashAuxInfo = NULL;
    sigParams.cMsgCert = 1;
    sigParams.cMsgCrl = 0;
    sigParams.rgpMsgCrl = NULL;
    sigParams.cAuthAttr = 0;
    sigParams.rgAuthAttr = NULL;
    sigParams.rgUnauthAttr = NULL;
    sigParams.cUnauthAttr = 0;
    sigParams.dwFlags = 0;
    sigParams.dwInnerContentType = 0;
    sigParams.HashAlgorithm.pszObjId = (LPSTR)(pOidInfo->pszOID);
    sigParams.HashAlgorithm.Parameters.cbData = 0;
    sigParams.HashAlgorithm.Parameters.pbData = NULL;
    sigParams.pSigningCert = certWithPrivateKey;
    sigParams.rgpMsgCert = &certWithPrivateKey;

    // Create the MessageArray and the MessageSizeArray
    const BYTE* MessageArray[] = {pbMessage};
    DWORD MessageSizeArray[1];
    MessageSizeArray[0] = cbMessage;

    

    // First, get the size of the signed BLOB.
    if(CryptSignMessage(
        &sigParams,
        FALSE,
        1,
        MessageArray,
        MessageSizeArray,
        NULL,
        &cbsignMessage))
    {
        printf("test");
    } else {
        getError();
    }
    
    BYTE* pbSignMessage = new BYTE[cbsignMessage];


    if (CryptSignMessage(
        &sigParams,
        FALSE,
        1,
        MessageArray,
        MessageSizeArray,
        pbSignMessage,
        &cbsignMessage))
    {
        printf("test");
    } else {
    getError();
    }
    
    DWORD signatureBase64Len;
    CryptBinaryToStringA(pbSignMessage, cbsignMessage, CRYPT_STRING_BASE64, NULL, &signatureBase64Len);
    LPSTR signatureBase64String = (char*)malloc(signatureBase64Len);
    CryptBinaryToStringA(pbSignMessage, cbsignMessage, CRYPT_STRING_BASE64, signatureBase64String, &signatureBase64Len);
    
    printf("Сигнатура: ");
    printf("%s", signatureBase64String);
    
    NSDictionary *map = [[NSDictionary alloc] initWithObjectsAndKeys: @(true), @"success", [NSString stringWithUTF8String:signatureBase64String], @"signature", @"CALG_GR3410_12_256", @"signatureAlgorithm", nil];

    return @"";
}

//NSString* signHash() {
////    sign();
//    NSString* message = @"Тест";
//    HCERTSTORE myStore = getMyStore();
//
//    /// Сертификат с приватным ключем
//    PCCERT_CONTEXT certWithPrivateKey = NULL;
//
//    /// Вывод информации о сертификате
//    PCCERT_CONTEXT pPrevCertContext = NULL;
//
//    do {
//        pPrevCertContext = CertEnumCertificatesInStore(myStore, pPrevCertContext);
//        if (pPrevCertContext != NULL) {
//            DWORD csz = CertNameToStrA(X509_ASN_ENCODING, &pPrevCertContext->pCertInfo->Subject, CERT_SIMPLE_NAME_STR, NULL, 0);
//            LPSTR psz = (LPSTR)malloc(csz);
//
//            CertNameToStrA(X509_ASN_ENCODING, &pPrevCertContext->pCertInfo->Subject, CERT_SIMPLE_NAME_STR, psz, csz);
//
//            printf("Сертификат: %s\n", psz);
//
//            HCRYPTPROV hProv = 0;
//            if (CryptAcquireCertificatePrivateKey(pPrevCertContext, CRYPT_ACQUIRE_SILENT_FLAG, NULL, &hProv, NULL, NULL)) {
//                certWithPrivateKey = pPrevCertContext;
//                printf("Сертификат связанный с приватным ключем: %s\n", psz);
//
//                NSData *certificateEncodedData = [NSData dataWithBytes:pPrevCertContext->pbCertEncoded length:pPrevCertContext->cbCertEncoded];
//                NSString *base64Certificate = [certificateEncodedData base64EncodedStringWithOptions:0];
//
//                DWORD pdwDataLen = 0;
//                CryptGetProvParam(hProv, PP_CONTAINER, NULL, &pdwDataLen, NULL);
//                BYTE* data = (BYTE*)malloc(pdwDataLen);
//                CryptGetProvParam(hProv, PP_CONTAINER, data, &pdwDataLen, NULL);
//                break;
//            }
//        }
//    } while (pPrevCertContext != NULL);
//
//
//    // Получили сертификат с приватным ключем
//
//    CRYPT_DATA_BLOB messageBlob;
//    DWORD cbsignMessage = 0;
//    CRYPT_DATA_BLOB signedMessage;
//    CRYPT_SIGN_MESSAGE_PARA sigParams;
//
//
//    const char *messageArr = [message UTF8String];
//
//    // String to ByteArray
////    messageBlob.cbData = (DWORD) arrSize(messageArr);
////    messageBlob.pbData = new BYTE[messageBlob.cbData];
////    memcpy(messageBlob.pbData, messageArr, messageBlob.cbData);
//
//    BYTE* pbMessage = (BYTE*)messageArr;
//    DWORD cbMessage = (strlen((CHAR*)pbMessage));
//
//    PCCERT_CONTEXT* arrayOfCertificates[] = {&certWithPrivateKey};
//
//    PCCRYPT_OID_INFO pOidInfo = CPCryptGetDefaultHashOIDInfo(certWithPrivateKey->pCertInfo->SubjectPublicKeyInfo.Algorithm.pszObjId);
//
//    // Initialize the signature structure.
//    ZeroMemory(&sigParams, sizeof(CRYPT_SIGN_MESSAGE_PARA));
//    sigParams.cbSize = sizeof(CRYPT_SIGN_MESSAGE_PARA);
//    sigParams.dwMsgEncodingType = X509_ASN_ENCODING | PKCS_7_ASN_ENCODING;
//    sigParams.pvHashAuxInfo = NULL;
//    sigParams.cMsgCert = 1;
//    sigParams.cMsgCrl = 0;
//    sigParams.rgpMsgCrl = NULL;
//    sigParams.cAuthAttr = 0;
//    sigParams.rgAuthAttr = NULL;
//    sigParams.rgUnauthAttr = NULL;
//    sigParams.cUnauthAttr = 0;
//    sigParams.dwFlags = 0;
//    sigParams.dwInnerContentType = 0;
//    sigParams.HashAlgorithm.pszObjId = (LPSTR)(pOidInfo->pszOID);
//    sigParams.HashAlgorithm.Parameters.cbData = 0;
//    sigParams.HashAlgorithm.Parameters.pbData = NULL;
//    sigParams.pSigningCert = certWithPrivateKey;
//    sigParams.rgpMsgCert = &certWithPrivateKey;
//
//    // Create the MessageArray and the MessageSizeArray
//    const BYTE* MessageArray[] = {pbMessage};
//    DWORD MessageSizeArray[1];
//    MessageSizeArray[0] = cbMessage;
//
//
//    PCRYPT_DATA_BLOB ppSignedBlob;
//
//    CADES_SIGN_MESSAGE_PARA cadesSigParams;
//
//    // Initialize the signature structure.
//    ZeroMemory(&cadesSigParams, sizeof(CADES_SIGN_MESSAGE_PARA));
//    cadesSigParams.dwSize = sizeof(CADES_SIGN_MESSAGE_PARA);
//
//
//
//    CadesSignHash(&cadesSigParams, pbMessage, cbMessage, NULL, &ppSignedBlob);
//
//
////    DWORD signatureBase64Len;
////    CryptBinaryToStringA(ppSignedBlob, cbsignMessage, CRYPT_STRING_BASE64, NULL, &signatureBase64Len);
////    LPSTR signatureBase64String = (char*)malloc(signatureBase64Len);
////    CryptBinaryToStringA(pbSignMessage, cbsignMessage, CRYPT_STRING_BASE64, signatureBase64String, &signatureBase64Len);
//
////    printf("Сигнатура: ");
////    printf("%s", signatureBase64String);
////
////    NSDictionary *map = [[NSDictionary alloc] initWithObjectsAndKeys: @(true), @"success", [NSString stringWithUTF8String:signatureBase64String], @"signature", @"CALG_GR3410_12_256", @"signatureAlgorithm", nil];
//
//    return @"";
//}
