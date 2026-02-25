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
 * \brief Интерфейс запросов к УЦ на сертификат
 *
 */

#ifndef _UNIXREQUEST_H
#define _UNIXREQUEST_H

#include<stdarg.h>

#ifdef _WIN32
#   include <CPROCSP/Certsrv.h>
#endif//_WIN32
#ifdef UNIX
#   include <CPROCSP/CSP_WinCrypt.h>
#ifndef VARIANT
#   define VARIANT void*
#endif//VARIANT
#endif // UNIX

#include <CPROCSP/SecureBuffer.h>
//#include "UnixEnroll.h"
#include <CPROCSP/BSTR.h>
#include <vector>
#include <string>
#include <CPROCSP/reader/support.h>
#include <map>

#ifndef _WIN32
    typedef unsigned short VARIANT_BOOL;
#endif//_WIN32

//========= CertSrv.h ===========

// TODO:X Есть вероятность конфликта MonoTouch и пр. вероятно нужно 
// внести всё в namespace ru::CryptoPro

#define PROPTYPE_LONG	     0x00000001	// Signed long
#define PROPTYPE_DATE	     0x00000002	// Date+Time
#define PROPTYPE_BINARY	     0x00000003	// Binary data
#define PROPTYPE_STRING	     0x00000004	// Unicode String
#define PROPTYPE_MASK	     0x000000ff

//========= CertCli.h ===========


#define	CR_IN_BASE64HEADER	( 0 )
#define	CR_IN_BASE64	( 0x1 )
//#define	CR_IN_BINARY	( 0x2 )
//#define	CR_IN_ENCODEANY	( 0xff )
#define	CR_IN_ENCODEMASK	( 0xff )
//#define	CR_IN_FORMATANY	( 0 )
#define	CR_IN_PKCS10	( 0x100 )
//#define	CR_IN_KEYGEN	( 0x200 )
//#define	CR_IN_PKCS7	( 0x300 )
//#define	CR_IN_CMC	( 0x400 )
#define	CR_IN_FORMATMASK	( 0xff00 )
//#define	CR_IN_RPC	( 0x20000 )
//#define	CR_IN_FULLRESPONSE	( 0x40000 )
//#define	CR_IN_CRLS	( 0x80000 )
//#define	CR_IN_MACHINE	( 0x100000 )
//#define	CR_IN_ROBO	( 0x200000 )
//#define	CR_IN_CLIENTIDNONE	( 0x400000 )
//#define	CR_IN_CONNECTONLY	( 0x800000 )
//#define	CR_IN_CLIENTFLAGSMASK	( ( ( ( ( CR_IN_ENCODEMASK | CR_IN_RPC )  | CR_IN_MACHINE )  | CR_IN_CLIENTIDNONE )  | CR_IN_CONNECTONLY )  )

#define	CR_DISP_INCOMPLETE	( 0 )
#define	CR_DISP_ERROR	( 0x1 )
#define	CR_DISP_DENIED	( 0x2 )
#define	CR_DISP_ISSUED	( 0x3 )
#define	CR_DISP_ISSUED_OUT_OF_BAND	( 0x4 )
#define	CR_DISP_UNDER_SUBMISSION	( 0x5 )
#define	CR_DISP_REVOKED	( 0x6 )
#define	CR_DISP_REJECTED	( 0x7 )
#define	CR_DISP_CONN_ERR	( 0x8 )

#define	CR_OUT_BASE64HEADER	( 0 )
#define	CR_OUT_BASE64	( 0x1 )
#define	CR_OUT_BINARY	( 0x2 )
#define	CR_OUT_BASE64REQUESTHEADER	( 0x3 )
//#define	CR_OUT_HEX	( 0x4 )
//#define	CR_OUT_HEXASCII	( 0x5 )
#define	CR_OUT_BASE64X509CRLHEADER	( 0x9 )
//#define	CR_OUT_HEXADDR	( 0xa )
//#define	CR_OUT_HEXASCIIADDR	( 0xb )
//#define	CR_OUT_HEXRAW	( 0xc )
#define	CR_OUT_ENCODEMASK	( 0xff )
#define	CR_OUT_CHAIN	( 0x100 )
#define	CR_OUT_CRLS	( 0x200 )
//#define	CR_OUT_NOCRLF	( 0x40000000 )
//#define	CR_OUT_NOCR	( 0x80000000 )
#define CR_UNKNOWN_ERR ( -1 )
#define CR_WRONG_PASS ( -2 )
#define CR_OLD_MARKER ( -3 )
#define CR_NOT_EXIST_MARKER ( -4 )
#define CR_WRONG_MARKER_NAME ( -5 )

#define CR_PROP_NONE               0  // Invalid
//#define CR_PROP_FILEVERSION        1  // String
//#define CR_PROP_PRODUCTVERSION     2  // String
//#define CR_PROP_EXITCOUNT          3  // Long
//
//// CR_PROP_EXITCOUNT Elements:
//#define CR_PROP_EXITDESCRIPTION    4  // String, Indexed
//
//#define CR_PROP_POLICYDESCRIPTION  5  // String
//#define CR_PROP_CANAME             6  // String
//#define CR_PROP_SANITIZEDCANAME    7  // String
//#define CR_PROP_SHAREDFOLDER       8  // String
//#define CR_PROP_PARENTCA           9  // String
//#define CR_PROP_CATYPE            10  // Long
#define CR_PROP_CASIGCERTCOUNT    11  // Long

// CR_PROP_CASIGCERTCOUNT Elements:
#define CR_PROP_CASIGCERT         12  // Binary, Indexed

// CR_PROP_CASIGCERTCOUNT Elements:
#define CR_PROP_CASIGCERTCHAIN    13  // Binary, Indexed

//#define CR_PROP_CAXCHGCERTCOUNT   14  // Long
//
//// CR_PROP_CAXCHGCERTCOUNT Elements:
//#define CR_PROP_CAXCHGCERT        15  // Binary, Indexed
//
//// CR_PROP_CAXCHGCERTCOUNT Elements:
//#define CR_PROP_CAXCHGCERTCHAIN   16  // Binary, Indexed

// CR_PROP_CASIGCERTCOUNT Elements:
// Fetch only if CR_PROP_CRLSTATE[i] == CA_DISP_VALID
// May also be available if CR_PROP_CRLSTATE[i] == CA_DISP_INVALID
#define CR_PROP_BASECRL           17  // Binary, Indexed

// CR_PROP_CASIGCERTCOUNT Elements:
// Fetch only if Delta CRLs enabled && CR_PROP_CRLSTATE[i] == CA_DISP_VALID
// May also be available if CR_PROP_CRLSTATE[i] == CA_DISP_INVALID
#define CR_PROP_DELTACRL          18  // Binary, Indexed

//// CR_PROP_CASIGCERTCOUNT Elements:
//#define CR_PROP_CACERTSTATE       19  // Long, Indexed

// CR_PROP_CASIGCERTCOUNT Elements:
#define CR_PROP_CRLSTATE          20  // Long, Indexed

//#define CR_PROP_CAPROPIDMAX       21  // Long
//#define CR_PROP_DNSNAME           22  // String
//#define CR_PROP_ROLESEPARATIONENABLED 23 // Long
//#define CR_PROP_KRACERTUSEDCOUNT  24  // Long
//#define CR_PROP_KRACERTCOUNT      25  // Long

//// CR_PROP_KRACERTCOUNT Elements:
//#define CR_PROP_KRACERT           26  // Binary, Indexed

//// CR_PROP_KRACERTCOUNT Elements:
//#define CR_PROP_KRACERTSTATE      27  // Long, Indexed

//#define CR_PROP_ADVANCEDSERVER    28  // Long
#define CR_PROP_TEMPLATES         29  // String

//// CR_PROP_CASIGCERTCOUNT Elements:
//// Fetch only if CR_PROP_CRLSTATE[i] == CA_DISP_VALID
//#define CR_PROP_BASECRLPUBLISHSTATUS 30  // Long, Indexed
//
//// CR_PROP_CASIGCERTCOUNT Elements:
// Fetch only if Delta CRLs enabled && CR_PROP_CRLSTATE[i] == CA_DISP_VALID
//#define CR_PROP_DELTACRLPUBLISHSTATUS 31  // Long, Indexed

// CR_PROP_CASIGCERTCOUNT Elements:
#define CR_PROP_CASIGCERTCRLCHAIN 32  // Binary, Indexed

//// CR_PROP_CAXCHGCERTCOUNT Elements:
//#define CR_PROP_CAXCHGCERTCRLCHAIN 33 // Binary, Indexed
//
//// CR_PROP_CASIGCERTCOUNT Elements:
//#define CR_PROP_CACERTSTATUSCODE  34  // Long, Indexed
//
//// CR_PROP_CASIGCERTCOUNT Elements:
//#define CR_PROP_CAFORWARDCROSSCERT 35  // Binary, Indexed
//
//// CR_PROP_CASIGCERTCOUNT Elements:
//#define CR_PROP_CABACKWARDCROSSCERT 36  // Binary, Indexed
//
//// CR_PROP_CASIGCERTCOUNT Elements:
//#define CR_PROP_CAFORWARDCROSSCERTSTATE 37  // Long, Indexed
//
//// CR_PROP_CASIGCERTCOUNT Elements:
//#define CR_PROP_CABACKWARDCROSSCERTSTATE 38  // Long, Indexed
//
//// CR_PROP_CASIGCERTCOUNT Elements:
//#define CR_PROP_CACERTVERSION       39  // Long, Indexed
//#define CR_PROP_SANITIZEDCASHORTNAME 40  // String
//
//// CR_PROP_CERTCDPURLS Elements:
//#define CR_PROP_CERTCDPURLS 41  // String, Indexed
//
//// CR_PROP_CERTAIAURLS Elements:
//#define CR_PROP_CERTAIAURLS 42  // String, Indexed
//
//// CR_PROP_CERTAIAOCSPURLS Elements:
//#define CR_PROP_CERTAIAOCSPURLS 43  // String, Indexed
//
//// CR_PROP_LOCALENAME Elements:
//#define CR_PROP_LOCALENAME 44  // String

#define CR_PROP_TEMPLATES_CA20 300 // String

//========= CertView.h ===========


#define CV_OUT_BASE64HEADER     ( 0 )
#define CV_OUT_BASE64   ( 0x1 )
//#define CV_OUT_BINARY   ( 0x2 )
#define CV_OUT_BASE64REQUESTHEADER      ( 0x3 )
//#define CV_OUT_HEX      ( 0x4 )
//#define CV_OUT_HEXASCII ( 0x5 )
#define CV_OUT_BASE64X509CRLHEADER      ( 0x9 )
//#define CV_OUT_HEXADDR  ( 0xa )
//#define CV_OUT_HEXASCIIADDR     ( 0xb )
//#define CV_OUT_HEXRAW   ( 0xc )
//#define CV_OUT_ENCODEMASK       ( 0xff )
//#define CV_OUT_NOCRLF   ( 0x40000000 )
//#define CV_OUT_NOCR     ( 0x80000000 )


//========= UnixRequest.h ===========

#define	CR_OUT_HTML	( 0xc3 )


//========= CertCli.h ===========

/*! \ingroup EnrollAPI
 *  \brief Тип аутентификации с УЦ
 */
#ifndef _WIN32
typedef 
enum X509EnrollmentAuthFlags
    {
		X509AuthNone		= 0,	/*!< Несуществующее значение */
		X509AuthAnonymous	= 1,	/*!< Доступ без аутентификации */
		X509AuthKerberos	= 2,	/*!< Не поддерживается */
		X509AuthUsername	= 4,	/*!< Аутентификация по имени и паролю */
		X509AuthCertificate	= 8		/*!< Аутентификация по клиентскому сертификату */
    } 	X509EnrollmentAuthFlags;
#endif//_WIN32

//========= UnixRequest.h ===========

/*! \ingroup EnrollAPI
 *  \brief Метод проверки аутентичности TLS соединения при 
 *         первоначальном контакте с УЦ (у нас может не быть цепочки 
 *         сертификатов и/или СОС)
 */
typedef 
enum X509EnrollmentCheckChainFlags
    {
		X509CC_None		= 0,		/*!< Несуществующее значение */
		X509CC_TLS		= 1,		/*!< Стандартная проверка TLS */
		X509CC_NoHostNameCheck= 2,	/*!< Битовый флаг - Не проверять DNS имя (адрес) УЦ.
										Только для методов GetCACertificate() и GetCAProperty() */
		X509CC_NoCheck = 4	/*!< Битовый флаг - не проверять ничего.
							Только для методов GetCACertificate() и GetCAProperty() */
    } 	X509EnrollmentCheckChainFlags;

/*! \ingroup EnrollAPI
 *  \brief Общий интерфейс взаимодействия с УЦ (связь, получение 
 *         параметров, передача запросов, ожидание и получение ответов)
 *
 *  \xmlonly <locfile><header>UnixEnroll.h</header> <ulib>libenroll.so</ulib></locfile>\endxmlonly
 * 
 * Является урезанным аналогом ICertRequest из Microsoft CryptoAPI.
 */
class UnixRequest
{
public:	    ///// UnixRequest
    /*!
     *  \brief Конструктор объекта взаимодействия с УЦ
     *
     *  \param pszCA_type_name [in] Название типа УЦ:
     *                  - "CPCA15";
     *                  - "MSCAstd";
     *                  - "MSCAent" (TODO: в первой версии не поддерживается);
     *
     *	\return Указатель на объект в случае успеха 
     */
    static UnixRequest *URFactory(const char *pszCA_type_name);

    /*!
     *  \brief Интерфейс запросов пользователя
     *
     *  \xmlonly <locfile><header>UnixEnroll.h</header> <ulib>libenroll.so</ulib></locfile>\endxmlonly
     */
    class UserCallbacks
    {
    public:
	/*!
	 *  \brief Запрос имени/ид.маркера/Thumbprint для входа на УЦ
	 *
	 *  \param prompt   [in]  Приглашение для ввода имени
	 *  \param strCredentialCAThumbprint [out] Буфер для имени
	 *
	 *  \return true в случае успешного ввода
	 *
	 *  \note Реализовывать не обязательно в случае если:
	 *	- Задано имя пользователя в аргументах;
	 *	- Не используется TLS;
	 *	- Всегда должны быть сертификаты.
	 *  \note  Полученная BSTR строка должна быть освобождена 
	 *         функцией SysFreeString()
	 */
	virtual bool askCredentialCAThumbprint( 
		    const BSTR prompt,
		    BSTR *strCredentialCAThumbprint){
	    UNUSED(prompt);
	    UNUSED(strCredentialCAThumbprint);
	    return false;	// По умолчанию - отказ
	}

	/*!
	 *  \brief Запрос подтверждения правильности Thumbprint одного 
	 *         из сертификатов УЦ
	 *
	 *  \param prompt   [in]  Приглашение для ввода имени
	 *  \param strCAThumbprint [in] Буфер для имени
	 *
	 *  \return true в случае положительного подтверждения
	 *
	 *  \note Реализовывать не обязательно в случае если:
	 *	- Не используется TLS;
	 *	- Всегда должны быть сертификаты.
	 */
	virtual bool showForCheckCAThumbprint( 
		    const BSTR prompt,
		    const BSTR strCAThumbprint){
	    UNUSED(prompt);
	    UNUSED(strCAThumbprint);
	    return false;	// По умолчанию - отказ
	}

	/*!
	 *  \brief Запрос пароля/маркера для входа на УЦ
	 *
	 *  \param prompt   [in]  Приглашение для ввода пароля
	 *  \param password [out] Буфер для пароля
	 *
	 *  \return true в случае успешного ввода
	 *
	 *  \note Реализовывать не обязательно в случае если
	 *	пароль/маркер задан в аргументах.
	 */
	virtual bool askPassword( 
		    const BSTR prompt,
		    CSecurePin &password){
	    UNUSED(prompt);
	    UNUSED(password);
	    return false;	// По умолчанию - отказ
	}

	/*!
	 *  \brief Клонирование
	 */
	virtual UserCallbacks* clone() const=0;

	/*!
	 *  \brief Деструктор
	 */
	virtual ~UserCallbacks() {}
    };

    virtual ~UnixRequest() {}

    /*!
     *  \brief Устанавливает расширенные параметры метода 
     *         аутентификации при взаимодействии с УЦ
     *
     *  \param pCallbacks [in] Реализация зпросов к пользователю (NULL, 
     *			если не требуется)
     *  \param AuthType   [in] Тип аутентификации:
     *		- X509AuthAnonymous - strCredential и sbPassword 
     *		  	должны быть NULL;
     *		- X509AuthCertificate - используется сертификат 
     *			клиента, SHA-1 Thumbprint которого задаётся 
     *			в strCredential;
     *		- X509AuthKerberos - не поддерживается;
     *		- X509AuthUsername - по имени/ид.маркера и 
     *			паролю/маркеру временного доступа;
     *  \param CheckChainType [in] Метод проверки сертификата TLS 
     *			сервера УЦ
     *  \param strCredential [in] имя пользователя, ид. маркера (NULL, 
     *		если не требуется или используется pCallbacks)
     *  \param sbPassword [in/out] Пароль или маркер временного 
     *		доступа (NULL, если не требуется или используется pCallbacks)
     *  \param UseLocalMachineCert [in] использовать для аутентификации
     *		сертификат из хранилища локального компьютера вместо
     *		хранилища текущего пользователя (по умолчанию FALSE)
     *
     *	\return S_OK - в случае успеха 
     */
    virtual HRESULT SetCredential(
	/* [in] */ UserCallbacks *pCallbacks,
	/* [in] */ X509EnrollmentAuthFlags AuthType,
	/* [in] */ X509EnrollmentCheckChainFlags CheckChainType,
	/* [in] */ const BSTR strCredential,
	/* [in][out] */ CSecurePin *sbPassword,
	/* [in] */ CSP_BOOL UseLocalMachineCert = FALSE) = 0;

    class AttrTriple 
    {
	public:
	    LONG Flags;
	    BSTR Name;
	    BSTR Value;

	    AttrTriple():Flags(0),Name(0),Value(0) { };
	    ~AttrTriple()
	    {
		if (Name)
		    SysFreeString(Name);
		if (Value)
		    SysFreeString(Value);
	    };
    };

    class RequestInfoEx
    {
	    bool has_items;
	public:
	    LONG Disposition;
	    std::string sent_date;
	    std::string approval_date;
	    std::string comment;
	    std::string PKCS;
	    RequestInfoEx():has_items(false),Disposition(-1),
		sent_date(),approval_date(),comment(),PKCS()
	    {
	    };
	    RequestInfoEx(
		LONG Disposition_,
		const std::string & sent_date_,
		const std::string & approval_date_,
		const std::string & comment_,
		const std::string & PKCS_
	    ): has_items(true),
	       Disposition(Disposition_),
	       sent_date(sent_date_),
	       approval_date(approval_date_),
	       comment(comment_),
	       PKCS(PKCS_)
	    {
	    };
	    bool empty()
	    {
		return !has_items;
	    };
    };

    typedef enum 
    {
	CA15None=0,
	CA15Cert=1,
	CA15Request=2,
	CA15Revoke=3
    } ReqType;

    typedef std::map<std::string,RequestInfoEx> RequestMapEx;

    /*! \ingroup EnrollAPI
     *  \brief Получить задаваемые УЦ параметры запроса на сертификат
     *
     *  \param strConfig [in]  URL CA 
     *  \param strTemplate [in]  Имя шаблона
     *  \param pstrRDN     [out] Различительное имя
     *  \param pstrEKUsage [out] Список расширенных использований ключа
     *  \param pKeySpec [out] KeySpec 
     *  \param pAttrs [out] указатель на вектор троек: аргументы метода 
     *    UnixEnroll::addExtensionToRequest
     *
     *	\retval S_OK		Успех
     *	\retval ERROR_MORE_DATA Значение *plCntAttr было мало, 
     *				возвращается необходимый размер
     *
     *  \note  Полученные BSTR строки должны быть освобождены 
     *         функцией SysFreeString()
     */

    virtual HRESULT GetRequestParams( 
	/* [in] */ BSTR strConfig,
	/* [in] */ BSTR strTemplate,
	/* [retval][out] */ BSTR *pstrRDN,
	/* [retval][out] */ BSTR *pstrEKUsage,
	/* [retval][out] */ DWORD *pKeySpec,
	/* [retval] */ std::vector<AttrTriple> *pAttrs
	){
	    UNUSED(strConfig);
	    UNUSED(strTemplate);
	    UNUSED(pstrRDN);
	    UNUSED(pstrEKUsage);
	    UNUSED(pKeySpec);
	    UNUSED(pAttrs);
	    return E_NOTIMPL; /* Реализуем только для CPCA15Request */
    }

    /*!
    *	\brief получить список идентификаторов запросов
    *   \param strConfig [in]  URL CA 
    *  	\param Requests [out] карта идентификаторов запросов
    *	Второй параметр - Disposition     */
    typedef std::map<std::string,LONG> RequestMap;

    virtual HRESULT ListRequests(
	/* [in] */ BSTR strConfig,
	/* [out] */ RequestMap &Requests 
    ) {
	UNUSED(strConfig);
	UNUSED(Requests);
	return E_NOTIMPL;
    }
    /*!
    *	\brief получить расширенный список идентификаторов запросов
    *   \param strConfig [in]  URL CA 
    *  	\param Requests [out] карта идентификаторов запросов
	*	\param type [in] Тип запроса
    *	Второй параметр - Disposition     */

    virtual HRESULT ListRequestsEx(
		BSTR strConfig,
	    RequestMapEx & Requests,
		ReqType type = CA15Request
	) {
	UNUSED(strConfig);
	UNUSED(Requests);
	UNUSED(type);
	return E_NOTIMPL;
    }

    /*! 
     *  \brief Получить ранее отправленный запрос
     *
     *  \param Flags       [in]  Поддерживается только CR_OUT_HTML 
     *			(необходим для печати)
     *  \param pstrRequest [out] Запрос
     *
     *	\return S_OK - в случае успеха 
     *
     *  \note  Полученная BSTR строка должна быть освобождена 
     *         функцией SysFreeString()
     */
    virtual HRESULT GetRequest( 
	/* [in] */ LONG Flags, /* CR_OUT_HTML */
	/* [retval][out] */ BSTR *pstrRequest){
	    UNUSED(Flags);
	    UNUSED(pstrRequest);
	    return E_NOTIMPL; /* Реализуем только для CPCA15Request */
    }

public:	    ///// ICertRequest
    /*!
     *  \brief Отправить запрос на сертификат
     *
     *  \param Flags         [in]  CR_IN_BASE64HEADER|CR_IN_PKCS10
     *  \param strRequest    [in]  Заданный запрос в заданном формате
     *  \param strAttributes [in]  Дополнительные неподписанные атрибуты, 
     *				   прикладываемые к подаваемому запросу
     *				   (не путать с атрибутами самого 
     *				   запроса PKCS#10)
     *  \param strConfig     [in]  Адрес центра, например 
     *				   "https://cpca.example.ru/UI"
     *  \param pDisposition  [out] Результат подачи запроса
     *
     *	\return S_OK - в случае успеха 
     *
     *  \note  Возможные результаты подачи запроса:
     *		- CR_DISP_DENIED - Запрос отвергнут УЦ
     *		- CR_DISP_ERROR - Ошибочная подача запроса 
     *				(требуется повторить)
     *		- CR_DISP_INCOMPLETE - Обработка запроса не завершена 
     *		- CR_DISP_ISSUED - Сертификат выдан (уже можно получать)
     *		- CR_DISP_ISSUED_OUT_OF_BAND - Сертификат выдан 
     *				(будет выдан) непосредственно на УЦ, 
     *				а не по сети
     *		- CR_DISP_UNDER_SUBMISSION - Сертификат находится в 
     *				процессе выдачи, получение будет 
     *				возможно после завершения обработки
     */
    virtual HRESULT Submit( 
	/* [in] */ LONG Flags, /* CR_IN_BASE64HEADER, CR_IN_PKCS10 */
	/* [in] */ const BSTR strRequest,
	/* [in] */ const BSTR strAttributes,
	/* [in] */ const BSTR strConfig,
	/* [retval][out] */ LONG *pDisposition) = 0;

    /*!
     *  \brief Получить статус запроса
     *
     *  \b Не поддерживается в текущей версии
     *
     *  Предназначен для получения текущего статуса, если в предыдущий 
     *  раз были получены статусы, либо CR_DISP_INCOMPLETE, либо 
     *  CR_DISP_UNDER_SUBMISSION.
     *
     *  \param RequestId    [in]  Идентификатор запроса
     *  \param strConfig    [in]  Адрес центра, например 
     *				   "https://cpca.example.ru/UI"
     *  \param pDisposition [out] Текущий результат подачи запроса
     *
     *	\return S_OK - в случае успеха 
     *
     *  \note  Возможные результаты подачи запроса:
     *		- CR_DISP_DENIED - Запрос отвергнут УЦ
     *		- CR_DISP_ERROR - Ошибочная подача запроса 
     *				(требуется повторить)
     *		- CR_DISP_INCOMPLETE - Обработка запроса не завершена, 
     *				получение будет возможно после 
     *				завершения обработки
     *		- CR_DISP_ISSUED - Сертификат выдан (уже можно получать)
     *		- CR_DISP_ISSUED_OUT_OF_BAND - Сертификат выдан 
     *				(будет выдан) непосредственно на УЦ, 
     *				а не по сети
     *		- CR_DISP_UNDER_SUBMISSION - Сертификат находится в 
     *				процессе выдачи, получение будет 
     *				возможно после завершения обработки
     */
    virtual HRESULT RetrievePending( 
	/* [in] */ LONG RequestId,
	/* [in] */ const BSTR strConfig,
	/* [retval][out] */ LONG *pDisposition) = 0;

    /*!
     *  \brief Подтвердить установку сертификата
     *
     *  \b Не поддерживается в текущей версии
     *
     *  Подтверждает установку сертификата и (при вызове через)
     *  TokenId закрывает токен
     *  \param RequestId    [in]  Идентификатор запроса
     *  \param strConfig    [in]  Адрес центра, например 
     *				   "https://cpca.example.ru/UI"
     *	\return S_OK - в случае успеха 
     *
     *  \note  Возможные результаты подачи запроса:
     *		- CR_DISP_DENIED - Запрос отвергнут УЦ
     *		- CR_DISP_ERROR - Ошибочная подача запроса 
     *				(требуется повторить)
     *		- CR_DISP_INCOMPLETE - Обработка запроса не завершена, 
     *				получение будет возможно после 
     *				завершения обработки
     *		- CR_DISP_ISSUED - Сертификат выдан (уже можно получать)
     *		- CR_DISP_ISSUED_OUT_OF_BAND - Сертификат выдан 
     *				(будет выдан) непосредственно на УЦ, 
     *				а не по сети
     *		- CR_DISP_UNDER_SUBMISSION - Сертификат находится в 
     *				процессе выдачи, получение будет 
     *				возможно после завершения обработки
     */
    virtual HRESULT AcknowledgeInstallCert( 
	/* [in] */ LONG RequestId,
	/* [in] */ const BSTR strConfig
	)
    {
	UNUSED(RequestId);
	UNUSED(strConfig);
	return E_NOTIMPL;
    }



    /*!
     *  \brief Получить последний код завершения, полученный при 
     *         обработке текущего запроса
     *
     *  \b Не поддерживается в текущей версии
     *
     *  GetLastStatus возвращает последний код возврата, не следует
     *  путать его с со статусом запроса.
     *
     *  \param pStatus [out] Последний код возврата
     *
     *	\return S_OK - в случае успеха 
     */
    virtual HRESULT GetLastStatus( 
	/* [retval][out] */ LONG *pStatus) {
	UNUSED(pStatus);
	return E_NOTIMPL; /* Вообще не нужно реализовывать */
    }

    /*!
     *  \brief Получить идентификатор текущего запроса
     *
     *  \param pRequestId [out] Идентификатор запроса
     *
     *	\return S_OK - в случае успеха 
     */
    virtual HRESULT GetRequestId( 
	/* [retval][out] */ LONG *pRequestId) = 0;

    /*!
     *  \brief Получить человекочитаемое сообщение для текущего 
     *         статуса обработки запроса
     *
     *  \param pstrDispositionMessage [out] Сообщение
     *
     *	\return S_OK - в случае успеха 
     *
     *  \note  Полученная BSTR строка должна быть освобождена 
     *         функцией SysFreeString()
     */
    virtual HRESULT GetDispositionMessage( 
	/* [retval][out] */ BSTR *pstrDispositionMessage) = 0;

    /*!
     *  \brief Получить сертификат УЦ
     *
     *  \param fExchangeCertificate [in] FALSE - получить сертификат 
     *			подписи сертификатов УЦ, TRUE - получить 
     *			сертификат шифрования, используемый для 
     *			шифрования в адрес УЦ
     *  \param strConfig       [in]  Адрес центра, например 
     *				   "https://cpca.example.ru/UI"
     *  \param Flags           [in]  CR_OUT_BASE64HEADER|CR_OUT_CHAIN
     *  \param pstrCertificate [out] Сертификат
     *
     *	\return S_OK - в случае успеха 
     *
     *  \note  Полученная BSTR строка должна быть освобождена 
     *         функцией SysFreeString()
     */
    virtual HRESULT GetCACertificate( 
	/* [in] */ LONG fExchangeCertificate, /* не поддерживаем */
	/* [in] */ const BSTR strConfig,
	/* [in] */ LONG Flags, /* CR_OUT_BASE64HEADER, CR_OUT_CHAIN */
	/* [retval][out] */ BSTR *pstrCertificate) = 0;

    /*!
     *  \brief Получить запрошенный сертификат УЦ
     *
     *  \param Flags           [in]  CR_OUT_BASE64HEADER|CR_OUT_CHAIN
     *  \param pstrCertificate [out] Сертификат
     *
     *	\return S_OK - в случае успеха 
     *
     *  \note  Полученная BSTR строка должна быть освобождена 
     *         функцией SysFreeString()
     */
    virtual HRESULT GetCertificate( 
	/* [in] */ LONG Flags, /* CR_OUT_HTML, CR_OUT_BASE64HEADER, CR_OUT_CHAIN */
	/* [retval][out] */ BSTR *pstrCertificate) = 0;

public:	    ///// ICertRequest2
    /*!
     *  \brief Получить статус запроса
     *
     *  \b Не поддерживается в текущей версии
     *
     *  Предназначен для получения текущего статуса по номеру запроса
     *  или серийному номеру запрашиваемого сертификата.
     *
     *  Выполняет ту же работу, что и UnixRequest::RetrievePending(),
     *  с дополнительной возможностью опроса по серийному номеру 
     *  сертификата.
     *
     *  \param strConfig       [in]  Адрес центра, например 
     *				   "https://cpca.example.ru/UI"
     *  \param RequestId       [in]  Идентификатор запроса
     *  \param strSerialNumber [in]  Серийный номер сертификата, если 
     *				   RequestId == -1
     *  \param pDisposition [out] Текущий результат подачи запроса
     *
     *	\return S_OK - в случае успеха 
     *
     *  \note  Возможные результаты подачи запроса:
     *		- CR_DISP_DENIED - Запрос отвергнут УЦ
     *		- CR_DISP_ERROR - Ошибочная подача запроса 
     *				(требуется повторить)
     *		- CR_DISP_INCOMPLETE - Обработка запроса не завершена, 
     *				получение будет возможно после 
     *				завершения обработки
     *		- CR_DISP_ISSUED - Сертификат выдан (уже можно получать)
     *		- CR_DISP_ISSUED_OUT_OF_BAND - Сертификат выдан 
     *				(будет выдан) непосредственно на УЦ, 
     *				а не по сети
     *		- CR_DISP_UNDER_SUBMISSION - Сертификат находится в 
     *				процессе выдачи, получение будет 
     *				возможно после завершения обработки
     */
    virtual HRESULT GetIssuedCertificate( 
	/* [in] */ const BSTR strConfig,
	/* [in] */ LONG RequestId,
	/* [in] */ const BSTR strSerialNumber,
	/* [retval][out] */ LONG *pDisposition){
	UNUSED(strConfig);
	UNUSED(RequestId);
	UNUSED(strSerialNumber);
	UNUSED(pDisposition);
	return E_NOTIMPL; /* Вообще не нужно реализовывать */
    }

    /*!
     *  \brief Получить сообщение об ощибке для заданного кода возврата
     *
     *  \b Не поддерживается в текущей версии
     *
     *  \param hrMessage [in] Код завершения
     *  \param Flags     [in] Формат сообщения:
     *				Ноль (0) - только текст сообщения;
     *				CR_GEMT_HRESULT_STRING - дополнительно 
     *				    добавить код завершения в десятичном 
     *				    и шестнадцатиричном виде;
     *  \param pstrErrorMessageText [out] Сообщение
     *
     *	\return S_OK - в случае успеха 
     *
     *  \note  Полученная BSTR строка должна быть освобождена 
     *         функцией SysFreeString()
     */
    virtual HRESULT GetErrorMessageText( 
	/* [in] */ LONG hrMessage,
	/* [in] */ LONG Flags,
	/* [retval][out] */ BSTR *pstrErrorMessageText){
	UNUSED(hrMessage);
	UNUSED(Flags);
	UNUSED(pstrErrorMessageText);
	return E_NOTIMPL; /* Вообще не нужно реализовывать */
    }
    /*!
     *  \brief Зарегистрировать пользователя     
     *  \param bstrConfig       [in]  Адрес центра, например "https://cpca.example.ru/UI"
     *  \param pUserInfo       [in] Данные о пользователе 
     *			    зависят от типа УЦ
    *
     *	\return S_OK - в случае успеха 
     *
     */
    virtual HRESULT RegisterUser( 
	/* [in] */ BSTR bstrConfig,
	/* [in] */ void * pUserInfo
	)
    {
	UNUSED(bstrConfig);
	UNUSED(pUserInfo);
	return E_NOTIMPL; /* Реализуем для CPCA1.5 */
    }
    /*!
     *  \brief Получить данные формы регистрации пользователя     
     *  \param bstrConfig       [in]  Адрес центра, например 
     *  \param pUserInfo       [in][out] Данные о пользователе 
     *			    зависят от типа УЦ
    *
     *	\return S_OK - в случае успеха 
     *
     */
    virtual HRESULT GetUserRegisterInfo( 
	/* [in] */ BSTR bstrConfig,
	/* [in] */ void * pUserInfo
	)
    {
	UNUSED(bstrConfig);
	UNUSED(pUserInfo);
	return E_NOTIMPL; /* Реализуем для CPCA1.5 */
    }

    /*!
     *  \brief Получить данные формы регистрации пользователя     
     *  \param bstrConfig	    [in]  Адрес центра, например 
     *  \param pUserRegisterId	    [out] ID запроса на регистрацию пользователя
     *  \param pUserRegisterStatus  [out] Статус запроса на регистрацию пользователя
     *  		В случае неуспеха в статус записываетя развернутый код ошибки
     *  		Возможные варианты :
     *  		CR_UNKNOWN_ERR - неизвестная ошибка
     *  		CR_WRONG_PASS - указан неверный пароль
     *  		CR_OLD_MARKER - просроченный маркер доступа
     *  		CR_NOT_EXIST_MARKER - указан несуществующий маркер доступа
     *  		CR_WRONG_MARKER_NAME - ошибка в имени маркера доступа
     *	\return S_OK - в случае успеха 
     *
     */
    virtual HRESULT GetUserRegisterStatus( 
	/* [in] */ BSTR bstrConfig,
	/* [in] */ LONG * pUserRegisterId,
	/* [in] */ LONG * pUserRegisterStatus
	)
    {
	UNUSED(bstrConfig);
	UNUSED(pUserRegisterId);
	UNUSED(pUserRegisterStatus);
	return E_NOTIMPL; /* Реализуем для CPCA1.5 */
    }


    /*!
     *  \brief Возвращает свойства УЦ
     *
     *  Этот метод функционально идентичен методу ICertAdmin2::GetCAProperty
     *  Microsoft CryptoAPI, полное описание следует читать на 
     *  msdn.microsoft.com.
     *
     *  \param strConfig [in] Адрес центра, например 
     *				   "https://cpca.example.ru/UI"
     *  \param PropId    [in] Ид. свойства. Поддерживаются:
     *		- CR_PROP_CASIGCERTCOUNT - кол-во ключей УЦ (LONG);
     *		- CR_PROP_CASIGCERTCHAIN - цепочка PropIndex ключа УЦ (BINARY);
     *		- CR_PROP_BASECRL - СОС PropIndex ключа УЦ (BINARY);
     *		- CR_PROP_DELTACRL - разностный СОС PropIndex ключа УЦ (BINARY);
     *		- CR_PROP_TEMPLATES - список шаблонов УЦ (STRING);
     *  \param PropIndex [in] Начиающийся с нуля идекс свойства;
     *  \param PropType  [in] Тип свойства. Поддерживаются:
     *		- PROPTYPE_LONG;
     *		- PROPTYPE_BINARY;
     *		- PROPTYPE_STRING;
     *  \param Flags     [in] Требуемый Форматсвойства. Поддерживаются:
     *		- CV_OUT_BASE64HEADER;
     *		- CV_OUT_BASE64REQUESTHEADER;
     *		- CV_OUT_BASE64X509CRLHEADER;
     *  \param pvarPropertyValue [out] Значение
     *
     *	\return S_OK - в случае успеха 
     *
     *  \note  Полученная BSTR строка должна быть освобождена 
     *         функцией SysFreeString()
     */
    virtual HRESULT GetCAProperty( 
	/* [in] */ const BSTR strConfig,
	/* [in] */ LONG PropId, /* CR_PROP_BASECRL, CR_PROP_DELTACRL, CR_PROP_TEMPLATES */
	/* [in] */ LONG PropIndex, 
	/* [in] */ LONG PropType, /* PROPTYPE_BINARY, PROPTYPE_STRING */
	/* [in] */ LONG Flags, /* CV_OUT_BASE64HEADER, CV_OUT_BASE64REQUESTHEADER, CV_OUT_BASE64X509CRLHEADER */
	/* [retval][out] */ VARIANT *pvarPropertyValue) = 0;

    /*!
     *  \brief Возвращает формат свойства УЦ
     *
     *  \b Не поддерживается в текущей версии
     *
     *  \param strConfig  [in] Адрес центра, например 
     *				   "https://cpca.example.ru/UI"
     *  \param PropId     [in] Ид. свойства. Описание смотри 
     *		UnixRequest::GetCAProperty и ICertAdmin2::GetCAProperty
     *  \param pPropFlags [out] Формат
     *
     *	\return S_OK - в случае успеха 
     */
    virtual HRESULT GetCAPropertyFlags( 
	/* [in] */ const BSTR strConfig,
	/* [in] */ LONG PropId,
	/* [retval][out] */ LONG *pPropFlags){
	UNUSED(strConfig);
	UNUSED(PropId);
	UNUSED(pPropFlags);
	return E_NOTIMPL; /* Вообще не нужно реализовывать */
    }

    /*!
     *  \brief Возвращает отображаемое имя свойства УЦ
     *
     *  \b Не поддерживается в текущей версии
     *
     *  \param strConfig  [in] Адрес центра, например 
     *				   "https://cpca.example.ru/UI"
     *  \param PropId     [in] Ид. свойства. Описание смотри 
     *		UnixRequest::GetCAProperty и ICertAdmin2::GetCAProperty
     *  \param pstrDisplayName [out] Отображаемое имя
     *
     *	\return S_OK - в случае успеха 
     */
    virtual HRESULT GetCAPropertyDisplayName( 
	/* [in] */ const BSTR strConfig,
	/* [in] */ LONG PropId,
	/* [retval][out] */ BSTR *pstrDisplayName){
	UNUSED(strConfig);
	UNUSED(PropId);
	UNUSED(pstrDisplayName);
	return E_NOTIMPL; /* Вообще не нужно реализовывать */
    }

    /*!
     *  \brief Возвращает данные кэша, которые были получены из УЦ
     *
     *  \b Не поддерживается в текущей версии
     *
     *  \param PropId    [in] Ид. свойства
     *  \param PropIndex [in] Начиающийся с нуля идекс свойства
     *  \param PropType  [in] Тип свойства
     *  \param Flags     [in] Требуемый Формат свойства
     *  \param pvarPropertyValue [out] Значение
     *
     *	\return S_OK - в случае успеха 
     *
     *  \note  Полученная BSTR строка должна быть освобождена 
     *         функцией SysFreeString()
     */
    virtual HRESULT GetFullResponseProperty( 
	/* [in] */ LONG PropId,
	/* [in] */ LONG PropIndex,
	/* [in] */ LONG PropType,
	/* [in] */ LONG Flags,
	/* [retval][out] */ VARIANT *pvarPropertyValue){
	UNUSED(PropId);
	UNUSED(PropIndex);
	UNUSED(PropType);
	UNUSED(Flags);
	UNUSED(pvarPropertyValue);
	return E_NOTIMPL; /* Вообще не нужно реализовывать */
    }

public:	    ///// ICertRequest3
    /*!
     *  \brief Устанавливает параметры метода 
     *         аутентификации при взаимодействии с УЦ
     *
     *  \param hWnd [in] Дескриптор окна, на Unix не используется.
     *  \param AuthType   [in] Тип аутентификации:
     *		- X509AuthAnonymous - strCredential и sbPassword 
     *		  	должны быть NULL;
     *		- X509AuthCertificate - используется сертификат 
     *			клиента, SHA-1 Thumbprint которого задаётся 
     *			в strCredential;
     *		- X509AuthKerberos - не поддерживается;
     *		- X509AuthUsername - по имени/ид.маркера и 
     *			паролю/маркеру временного доступа;
     *  \param strCredential [in] имя пользователя, ид. маркера или 
     *		Thumbprint клиентского сертификата (NULL, 
     *		если не требуется)
     *  \param strPassword [in] Пароль или маркер временного 
     *		доступа (NULL, если не требуется)
     *
     *	\return S_OK - в случае успеха 
     */
    virtual HRESULT SetCredential( 
        /* [in] */ LONG hWnd,
        /* [in] */ X509EnrollmentAuthFlags AuthType,
        /* [in] */ BSTR strCredential,
	/* [in] */ BSTR strPassword) = 0;
    /*!
     *  \brief Получить идентификатор текущего запроса в виде строки
     *
     *  \b Не поддерживается в текущей версии
     *
     *  \param pstrRequestId [out] Идентификатор запроса
     *
     *	\return S_OK - в случае успеха 
     *
     *  \note  Полученная BSTR строка должна быть освобождена 
     *         функцией SysFreeString()
     */
    virtual HRESULT GetRequestIdString( 
	/* [retval][out] */ BSTR *pstrRequestId){
	UNUSED(pstrRequestId);
	return E_NOTIMPL; /* Вообще не нужно реализовывать */
    }

    /*!
     *  \brief Получить статус запроса
     *
     *  \b Не поддерживается в текущей версии
     *
     *  Предназначен для получения текущего статуса, по номеру запроса
     *  или серийному номеру сертифката запрашиваемого сертификата.
     *
     *  Выполняет туже работу, что и UnixRequest::RetrievePending(),
     *  с дополнительной возможностью опроса по серийному номеру 
     *  сертификата.
     *
     *  \param strConfig       [in]  Адрес центра, например 
     *				   "https://cpca.example.ru/UI"
     *  \param strRequestId    [in]  Идентификатор запроса в виде строки
     *  \param strSerialNumber [in]  Серийный номер сертификата, если 
     *				   RequestId == -1. Состоит из чётного
     *                             числа шестндцатиричных цифр, при 
     *				   необходимости может быть добавлено 
     *				   не более одно лидирующего нуля
     *  \param pDisposition [out] Текущий результат подачи запроса
     *
     *	\return S_OK - в случае успеха 
     *
     *  \note  Возможные результаты подачи запроса:
     *		- CR_DISP_DENIED - Запрос отвергнут УЦ
     *		- CR_DISP_ERROR - Ошибочная подача запроса 
     *				(требуется повторить)
     *		- CR_DISP_INCOMPLETE - Обработка запроса не завершена, 
     *				получение будет возможно после 
     *				завершения обработки
     *		- CR_DISP_ISSUED - Сертификат выдан (уже можно получать)
     *		- CR_DISP_ISSUED_OUT_OF_BAND - Сертификат выдан 
     *				(будет выдан) непосредственно на УЦ, 
     *				а не по сети
     *		- CR_DISP_UNDER_SUBMISSION - Сертификат находится в 
     *				процессе выдачи, получение будет 
     *				возможно после завершения обработки
     */
    virtual HRESULT GetIssuedCertificate2( 
	/* [in] */ BSTR strConfig,
	/* [in] */ BSTR strRequestId,
	/* [in] */ BSTR strSerialNumber,
	/* [retval][out] */ LONG *pDisposition){
	UNUSED(strConfig);
	UNUSED(strRequestId);
	UNUSED(strSerialNumber);
	UNUSED(pDisposition);
	return E_NOTIMPL; /* Вообще не нужно реализовывать */
    }

    /*!
     *  \brief Получить признак обновления политики выпуска 
     *         сертификатов клиента
     *
     *  \b Не поддерживается в текущей версии
     *
     *  \param pValue [out] Признак обновления
     *
     *	\return S_OK - в случае успеха 
     */
    virtual HRESULT GetRefreshPolicy( 
	/* [retval][out] */ VARIANT_BOOL *pValue){
	UNUSED(pValue);
	return E_NOTIMPL; /* Вообще не нужно реализовывать */
    }

};
#endif /* _UNIXEREQUEST_H */
