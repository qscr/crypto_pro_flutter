/* [Windows 1251]
 * [Use `iconv -f WINDOWS-1251', if needed]
 */
/*
 * Copyright(C) 2005-2011
 *
 * Этот файл содержит информацию, являющуюся
 * собственностью компании Крипто-Про.
 *
 * Любая часть этого файла не может быть скопирована,
 * исправлена, переведена на другие языки,
 * локализована или модифицирована любым способом,
 * откомпилирована, передана по сети с или на
 * любую компьютерную систему без предварительного
 * заключения соглашения с компанией Крипто-Про.
 *
 * This is proprietary information of
 * Crypto-Pro company.
 *
 * Any part of this file can not be copied, 
 * corrected, translated into other languages,
 * localized or modified by any means,
 * compiled, transferred over a network from or to
 * any computer system without preliminary
 * agreement with Crypto-Pro company
 */

/*!
 * \version $Revision: 56941 $
 * \date $Date: 2009-09-11 14:09:02 +0400 (РџС‚, 11 СЃРµРЅ 2009) $
 * \author $Author: dedal $
 *
 * \brief Интерфейс запросов к УЦ на сертификат реализация для Standalone CA
 *
 */

#ifndef __MS_STD_CA_H__
#define __MS_STD_CA_H__
#include<CPROCSP/UnixRequestImpl.h>

/*! \ingroup EnrollAPI
 *  \class MSCAstdRequest::UnixRequest
 *  \brief Реализация интерфеса взаимодействия с Microsoft Standalone CA
 *
 *  \xmlonly <locfile><header>UnixEnroll.h</header> <ulib>libenroll.so</ulib></locfile>\endxmlonly
 * 
 * Отличия от других реализаций интерфейсов:
 * 	- X509AuthAnonymous - поддерживается
 * 	- X509AuthKerberos - не поддерживается
 * 	- X509AuthUsername - не поддерживается (?)
 * 	- X509AuthCertificate - не поддерживается (?)
 * 	- strConfig - может начинаться, как с https, так и с http
 * 	- GetRequest() - не поддерживается
 * 	- GetCertificate(...CR_OUT_HTML...) - не поддерживается
 *
 * strConfig обычно должен иметь вид "http://msca.w2k.example.ru/certsrv"
 *
 * CR_PROP_TEMPLATES - не применим для Microsoft Standalone CA
 * CR_PROP_CASIGCERTCOUNT - в предварительной версии возвращает всегда 1
 *
 */
 /*
 *  TODO: CR_PROP_CASIGCERTCOUNT в предварительной версии возвращает всегда 1
 */

class MSCAstdRequest: public UnixRequestImpl
{
protected:
    LONG RequestId;
    bool fCertificate;
    std::string strCertificate;

public:
    MSCAstdRequest():UnixRequestImpl(),RequestId(-1),
		     fCertificate(false),strCertificate()
    {
	
    };

    virtual HRESULT GetRequestParams( 
	/* [in] */ BSTR strConfig,
	/* [in] */ BSTR strTemplate,
	/* [retval][out] */ BSTR *pstrRDN,
	/* [retval][out] */ BSTR *pstrEKUsage,
	/* [retval][out] */ DWORD *pKeySpec,
	/* [retval] */ std::vector<AttrTriple> *pAttrs
	);

    virtual HRESULT Submit( 
	/* [in] */ LONG Flags, /* CR_IN_BASE64HEADER, CR_IN_PKCS10 */
	/* [in] */ const BSTR strRequest,
	/* [in] */ const BSTR strAttributes,
	/* [in] */ const BSTR strConfig,
	/* [retval][out] */ LONG *pDisposition);
    virtual HRESULT GetRequestId(
	      LONG* pRequestId
	      )
    {
	if (RequestId != -1)
	{
	    *pRequestId = RequestId;
	    return S_OK;
	}
	else
	    return NTE_FAIL;
    };
    virtual HRESULT GetCertificate(
       LONG Flags,
       BSTR* pstrCertificate
    );
    virtual HRESULT GetCACertificate( 
	/* [in] */ LONG fExchangeCertificate, /* не поддерживаем */
	/* [in] */ const BSTR strConfig,
	/* [in] */ LONG Flags, /* CR_OUT_BASE64HEADER, CR_OUT_CHAIN */
	/* [retval][out] */ BSTR *pstrCertificate) ;


    virtual HRESULT RetrievePending( 
	/* [in] */ LONG RequestId,
	/* [in] */ const BSTR strConfig,
	/* [retval][out] */ LONG *pDisposition);

    virtual HRESULT GetCAProperty( 
	/* [in] */ const BSTR strConfig,
	/* [in] */ LONG PropId, /* CR_PROP_BASECRL, CR_PROP_DELTACRL, CR_PROP_TEMPLATES */
	/* [in] */ LONG PropIndex, 
	/* [in] */ LONG PropType, /* PROPTYPE_BINARY, PROPTYPE_STRING */
	/* [in] */ LONG Flags, /* CV_OUT_BASE64HEADER, CV_OUT_BASE64REQUESTHEADER, CV_OUT_BASE64X509CRLHEADER */
	/* [retval][out] */ VARIANT *pvarPropertyValue);
};
#endif

