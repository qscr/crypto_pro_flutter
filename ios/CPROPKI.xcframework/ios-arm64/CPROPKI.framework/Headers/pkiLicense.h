#ifndef PKI_LICENSE_H_
#define PKI_LICENSE_H_

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

    HRESULT setPkiLicense(LPCWSTR serialStr);
    HRESULT getOCSPLicenseValidTo(LPCWSTR validTo, DWORD *dwLen);
    HRESULT getTSPLicenseValidTo(LPCWSTR validTo, DWORD *dwLen);
    HRESULT getOCSPLicense(LPCWSTR serialNumber, DWORD *dwLen);
    HRESULT getTSPLicense(LPCWSTR serialNumber, DWORD *dwLen);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // PKI_LICENSE_H_
