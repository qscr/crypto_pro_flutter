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
 * \version $Revision: 257202 $
 * \date $Date:: 2023-06-06 01:53:19 -0700#$
 * \author $Author: kadykov $
 *
 * \brief Интерфейс запросов на сертификат
 *
 */

#ifndef _UNIXENROLL_H
#define _UNIXENROLL_H

#include <string>
#include <memory>
#include <list>
#include <vector>
#include <CPROCSP/CSP_WinCrypt.h>
#include <CPROCSP/SecureBuffer.h>
#include <CPROCSP/BSTR.h>
#include <CPROCSP/CPEnrollImpl.h>

#if __GNUC__==2
typedef std::basic_string <wchar_t> std::wstring;
#endif
#ifdef ANDROID
namespace std {
	typedef basic_string<wchar_t> wstring;
}
#endif

// TODO:X Есть вероятность конфликта MonoTouch и пр. вероятно нужно 
// внести всё в namespace ru::CryptoPro

#define XECR_PKCS10_V2_0     0x1
#define XECR_PKCS7           0x2
#define XECR_CMC             0x3
#define XECR_PKCS10_V1_5     0x4

// BSTR SysAllocStringLen( const wchar_t *str, unsigned int cch);

/*! \ingroup EnrollAPI
 *  \brief Создание запросов на сертификат и установка 
 *	   полученных сертификатов
 *
 *  \xmlonly <locfile><header>UnixEnroll.h</header> <ulib>libenroll.so</ulib></locfile>\endxmlonly
 * 
 * Является урезанным аналогом ICertEnroll из Microsoft CryptoAPI.
 */
class UnixEnroll: public CPEnrollImpl
{
protected:
	bool handlePKCS7_;
public:
    /*! 
     *  \brief Конструктор
     *
     *  \param callbacks    [in] Функции запроса у пользователя
     *  \param handlePKCS7  [in] В данной версии не используется
     */
    UnixEnroll(
	const CPEnroll::UserCallbacks& callbacks,
	bool handlePKCS7 = false);

    /*! 
     *  \fn UnixEnroll::~UnixEnroll
     *  \brief Деструктор
     */
    ~UnixEnroll();

    /*!
     *  \brief Установить атрибуты S/MIME в запрос PKCS#10 (не реализовано)
     *
     *  \param value	[in] true - установить.
     *
     *	\return S_OK - в случае успеха 
     */
    virtual HRESULT put_EnableSMIMECapabilities(bool value)
    { 
	UNUSED(value);
	return S_OK;
    }
    
    /*! 
     *  \brief Использовать существующий ключ в контейнере (не реализовано)
     *
     *  \param value	[in] true - существующий, false - создать новый.
     *
     *	\return S_OK - в случае успеха 
     */
    virtual HRESULT put_UseExistingKeySet(bool value);
    
    /*! 
     *  \brief Задать имя контейнера
     *
     *  Если имя контейнера не задано, то оно генерируется как 
     *  случайный GUID. 
     *
     *  \param cName	[in] Имя контейнера.
     *	\return S_OK - в случае успеха 
     *
     *  \note
     *  При использовании UseExistingKeySet, требуется
     *  задавать имя контейнера.
     */
    virtual HRESULT put_ContainerName( BSTR cName)
    {
	containerName_ = SysAllocString(cName);
	return S_OK;
    }
    
    /*! 
     *  \brief Задать тип CSP для создания ключа
     *
     *  Тип CSP определяет алгоритм закрытого ключа.
     *
     *  \param pType	[in] Тип CSP.
     *
     *	\return S_OK - в случае успеха 
     */
    virtual HRESULT put_ProviderType( DWORD pType)
    {
	providerType_ = pType;
	return S_OK;
    }
    
    /*! 
     *  \brief Задать имя CSP для создания ключа
     *
     *  Определяет CSP, в котором создаётся закрытый ключ.
     *
     *  \param pName	[in] Имя CSP.
     *
     *	\return S_OK - в случае успеха 
     */
    virtual HRESULT put_ProviderName( BSTR pName)
    {
	providerName_ = SysAllocString(pName);
	return S_OK;
    }
    
    /*! 
     *  \brief Задать свойства закрытого ключа
     *
     *  По умолчанию экспорт закрытого ключа запрещён. Для его
     *  разрешения следует использовать флаг CRYPT_EXPORTABLE. 
     *
     *  \param keyFlags	[in] Флаги создания закрытого ключа.
     *
     *	\return S_OK - в случае успеха 
     *
     *  \note
     *  Полный список флагов смотри в описании CPGenKey() и CryptGenKey().
     */
    virtual HRESULT put_GenKeyFlags( DWORD keyFlags)
    {
	keyFlags_ = keyFlags;
	return S_OK;
    }
    
    /*! 
     *  \brief Задать хранилище запроса и флаги его открытия 
     *
     *  Значение по умолчанию CERT_SYSTEM_STORE_CURRENT_USER.
     *
     *  \param storeFlags [in] Флаги открытия хранилища.
     *
     *	\return S_OK - в случае успеха 
     *
     *  \note
     *  Для Unix систем допустимые значения хранилищ:
     *	    - CERT_SYSTEM_STORE_CURRENT_USER
     *	    - CERT_SYSTEM_STORE_LOCAL_MACHINE
     *  
     *  Описания дополнительных флагов открытия хранилища 
     *  смотри в описании CertOpenStore().
     */
    virtual HRESULT put_RequestStoreFlags( DWORD storeFlags)
    {
	requestStoreFlags_ = storeFlags;
	return S_OK;
    }
    
    /*! 
     *  \brief Задать флаги открытия CSP
     *
     *  Значение по умолчанию 0.
     *
     *  \param provFlags [in] Флаги открытия CSP.
     *
     *	\return S_OK - в случае успеха 
     *  
     *  \note
     *  Описания дополнительных флагов открытия CSP
     *  смотри в описании CPAcquireContext() и CryptAcquireContext().
     */
    virtual HRESULT put_ProviderFlags( DWORD provFlags)
    {
	provFlags_ |= provFlags; /* TODO:XXX почему OR ??? */
	return S_OK;
    }
    
    /*! 
     *  \brief Задать тип закрытого ключа
     *
     *  \param _dwKeySpec [in] Тип закрытого ключа.
     *
     *	\return S_OK - в случае успеха 
     *
     *  \note
     *  Обычно используемые типы:
     *	    - AT_KEYEXCHANGE - и согласование сессионных ключей 
     *		шифрования, и ЭЦП;
     *	    - AT_SIGNATURE - только ЭЦП;
     *  
     *  \note
     *  Смотри также описания CPGenKey() и CryptGenKey().
     */
     /*
     *  TODO:XXX В отличии от ICertEnroll нет значения по умолчанию ???
     */
    virtual HRESULT put_KeySpec( DWORD _dwKeySpec)
    {
	dwKeySpec = _dwKeySpec;
	return S_OK;
    }
    
    /*! 
     *  \brief Задать ограничения ключа AT_KEYEXCHANGE
     *
     *  \param value [in] Ограничения ключа AT_KEYEXCHANGE.
     *
     *	\return S_OK - в случае успеха 
     *
     *  \note
     *  Это свойство относится только к ключам типа AT_KEYEXCHANGE. Для
     *  ключей типа AT_SIGNATURE его значение может быть произвольным.
     *
     *  \note
     *  Если значения свойства 0 (false), то запрос содержит следующие 
     *  значение KeyUsage:
     *	    - CERT_DATA_ENCIPHERMENT_KEY_USAGE
     *	    - CERT_KEY_ENCIPHERMENT_KEY_USAGE
     *	    - CERT_DIGITAL_SIGNATURE_KEY_USAGE
     *	    - CERT_NON_REPUDIATION_KEY_USAGE
     *
     *  \note
     *  Если значения свойства не 0 (true), то запрос содержит следующие 
     *  значение KeyUsage:
     *	    - CERT_DATA_ENCIPHERMENT_KEY_USAGE
     *	    - CERT_KEY_ENCIPHERMENT_KEY_USAGE
     */
    virtual HRESULT put_LimitExchangeKeyToEncipherment( int value)
    {
	limitExchangeKeyToEncipherment_ = value ? true : false;
	return S_OK;
    }
    
    /*! 
     *  \brief Задать хранилище промежуточных УЦ и флаги его открытия 
     *
     *  Значение по умолчанию CERT_SYSTEM_STORE_CURRENT_USER.
     *
     *  \param flags [in] Флаги открытия хранилища.
     *
     *	\return S_OK - в случае успеха 
     *
     *  \note
     *  Для Unix систем допустимые значения хранилищ:
     *	    - CERT_SYSTEM_STORE_CURRENT_USER
     *	    - CERT_SYSTEM_STORE_LOCAL_MACHINE
     *  
     *  Описания дополнительных флагов открытия хранилища 
     *  смотри в описании CertOpenStore().
     */
    virtual HRESULT put_CAStoreFlags( DWORD flags)
    {
	caStoreFlags_ = flags;
	return S_OK;
    }
    
    /*! 
     *  \brief Задать хранилище сертификатов пользователя и флаги 
     *		его открытия 
     *
     *  Значение по умолчанию CERT_SYSTEM_STORE_CURRENT_USER.
     *
     *  \param flags [in] Флаги открытия хранилища.
     *
     *	\return S_OK - в случае успеха 
     *
     *  \note
     *  Для Unix систем допустимые значения хранилищ:
     *	    - CERT_SYSTEM_STORE_CURRENT_USER
     *	    - CERT_SYSTEM_STORE_LOCAL_MACHINE
     *  
     *  Описания дополнительных флагов открытия хранилища 
     *  смотри в описании CertOpenStore().
     */
    virtual HRESULT put_MyStoreFlags( DWORD flags)
    {
	myStoreFlags_ = flags;
	return S_OK;
    }
    
    /*! 
     *  \brief Задать доверенное хранилище корневых сертификатов и 
     *		флаги его открытия 
     *
     *  Значение по умолчанию CERT_SYSTEM_STORE_CURRENT_USER.
     *
     *  \param flags [in] Флаги открытия хранилища.
     *
     *	\return S_OK - в случае успеха 
     *
     *  \note
     *  Для Unix систем допустимые значения хранилищ:
     *	    - CERT_SYSTEM_STORE_CURRENT_USER
     *	    - CERT_SYSTEM_STORE_LOCAL_MACHINE
     *  
     *  Описания дополнительных флагов открытия хранилища 
     *  смотри в описании CertOpenStore().
     */
    virtual HRESULT put_RootStoreFlags( DWORD flags)
    {
	rootStoreFlags_ = flags;
	return S_OK;
    }
    
    /*! 
     *  \brief Задать имя хранилища сертификатов пользователя
     *
     *  Значение по умолчанию "My".
     *
     *  \param name [in] Имя хранилища.
     *
     *	\return S_OK - в случае успеха 
     *
     *  \note
     *  Описания возможных имён хранилища 
     *  смотри в описании CertOpenStore().
     */
    virtual HRESULT put_MyStoreName( BSTR name)
    {
	myStoreName_ = SysAllocString(name);
	return S_OK;
    }
    
    /*! 
     *  \brief Требуется ли удалять псевдосертификат в хранилище 
     *		запросов на сертификат
     *
     *  Значение по умолчанию true.
     *
     *  \param value [in] Требуется ли удалять.
     *
     *	\return S_OK - в случае успеха 
     *
     *  \note
     *  При создании ключа и запроса PKCS#10 создаётся псевдосертификат
     *  в хранилище запросов. Он служит для ссылки на закрытый ключ и др.
     *  до тех пор, пока УЦ не обработает запрос и не вернёт PKCS#7
     *  ответ. По получении PKCS#7 ответа и успешной установки 
     *  запрошенного сертификата, псевдосертификат удаляется.
     *  Для целей отладки или тестирования можно запретить удаление 
     *  этого псевдосертификата установкой свойства DeleteRequestCert
     *  в значение false.
     */
    virtual HRESULT put_DeleteRequestCert( bool value)
    {
	deleteRequest_ = value;
	return S_OK;
    }
    
    /*!
     *  \brief Требуется ли устанавливать сертификат в ключевой контейнер
     *
     *  Значение по умолчанию false.                                     
     *
     *  \param value [in] Требуется ли устанавливать.
     *
     *	\return S_OK - в случае успеха 
     *
     *  \note

     *  Установка сертификата в контейнер дополнительно к его 
     *  установке в хранилище "My", в основном, предназначено
     *  для отделяемых ключевых носителей. При задании значения true,
     *  происходит попытка записи сертификата в контейнер, но операция 
     *  не будет возвращать ошибку даже в том случае, если ключевой носитель не 
     *  сможет сохранить сертификат. В частности, если это значение
     *  true, но ключевой носитель (смарт-карта, USB-токен, 
     *  USB-флэш и т.п.) не подключен, то ошибки ключевого носителя 
     *  будут проигнорированны.
     * 
     *  \note
     *  Для того, чтобы не вызывалась функция записи сертификата 
     *  требуется установить значение false.
     *
     *  \note
     *  Используется методами:
     *	    - acceptPKCS7()
     *
     *  \note
     *  Смотри также описания CPSetKeyParam() и CryptSetKeyParam().
     */
     /*
     *  Значение по умолчанию false.
     *  TODO:XXX В отличии от ICertEnroll, у которого оно true ???
     */
    virtual HRESULT put_WriteCertToCSP( bool value)
    {
	writeToCSP_ = value;
	return S_OK;
    }
    
    /*!
     *  \brief Задать PIN (пароль) контейнера
     *
     *  Значение по умолчанию не задано.
     *
     *  \param pin [in] PIN (пароль) контейнера.
     *
     *	\return S_OK - в случае успеха 
     *
     *  \note
     *  Задаёт PIN (пароль) при создании запроса или 
     *  получении сертификата для работы в режиме CRYPT_SILENT 
     *  (или при недоступности устройства текстового или графического 
     *  интерфейса пользователя (TUI или GUI)).
     */
    virtual HRESULT put_PIN( const CSecurePin& pin)
    {
	//ожидаем что пин имеет на конце ноль, иначе всё будет плохо
	if (pin.ptr()[pin.len() - 1] != 0)
	    return NTE_BAD_DATA;
	pin_.copy(pin);
	return S_OK;
    }
    
    /*!
     *  \brief Не запрашивать подтверждения при установке корневого
     *		сертификата в хранилище доверенных сертификатов
     *
     *  Значение по умолчанию - false.
     *
     *  \param value [in] PIN (пароль) контейнера.
     *
     *	\return S_OK - в случае успеха 
     *
     *  \note
     *  Метод больше не используется.
     */
    virtual HRESULT put_SupressAddRootUI( bool value)
    {
	supressAddRootUi_ = value;
	return S_OK;
    }

    /*!
     *  \brief Добавить расширение в запрос на сертификат
     *
     *  \param flags [in] Является ли расширение критичным (critical)
     *  \param name  [in] OID расширения
     *  \param value [in] Расширение, закодированное в BASE64
     *
     *	\return S_OK - успешно
     *
     *  \note
     *  Метод createRequest() добавляет расширения в запрос, а метод
     *  createPKCS10() нет.
     */
    virtual HRESULT addExtensionToRequest( LONG flags, BSTR name, BSTR value) {
        std::vector<BYTE> binary;
        HRESULT hr = UnixEnroll::getMessageFromBSTR(value,binary);
        if( S_OK != hr ) {
            return hr;
        }
        
        const char *oid = ConvertBSTRToString(name);
        
        extensions_.push_back(Extension(oid, flags, binary) );

        delete[] oid;
        
        return S_OK;
    }

    /*! 
     *  \brief Возвращает код ошибки при установке сертификата в 
     *		контейнер
     *
     *  \param pdwStatus [out] код ошибки при установке сертификата в 
     *				контейнер
     *
     *	\retval S_OK		успешно (в pdwStatus скопирован код 
     *				ошибки)
     *	\retval E_INVALIDARG	pdwStatus был не задан (==NULL)
     */
    virtual HRESULT get_InstallToContainerStatus( DWORD *pdwStatus) {
        if(!pdwStatus) {
            return E_INVALIDARG;
        }
        *pdwStatus = installToContainerStatus_;
        return S_OK;
    }
    
    /*! 
     *  \brief Создать запрос PKCS#10
     *
     *  \param rdn     [in]  Уникальное имя (DN) владельца ключа
     *  \param usage   [in]  Список OID расширенного использования 
     *			     ключа (Extended Key Usage (EKU))
     *  \param request [out] Запрос, закодированный в BASE64
     *
     *	\return S_OK - успешно
     *
     *  \note
     *  Игнорирует расширения, заданные методом addExtensionToRequest()
     *
     *	\note
     *	Полученная BSTR строка запроса должна быть освобождена 
     *  функцией SysFreeString()
     */
    virtual HRESULT createPKCS10( BSTR rdn, BSTR usage, BSTR *request) {
        return
	    createPKCSRequest(rdn, usage, request, false,false);
    }
    
    /*!
     *  \brief Создать запрос сертификат
     *
     *  \param flags   [in]  Тип запроса на сертификат
     *  \param rdn     [in]  Уникальное имя (DN) владельца ключа
     *  \param usage   [in]  Список OID расширенного использования 
     *			     ключа (Extended Key Usage (EKU))
     *  \param request [out] Запрос, закодированный в BASE64
     *
     *	\return S_OK - успешно
     *
     *  \note
     *  Поддерживаются следующие типы запросов на сертификат:
     *	    - XECR_PKCS10_V1_5
     *	    - XECR_PKCS10_V2_0
     *
     *  \note
     *  Добавляет расширения, заданные методом addExtensionToRequest()
     *
     *	\note
     *	Полученная BSTR строка запроса должна быть освобождена 
     *  функцией SysFreeString()
     */
    virtual HRESULT createRequest( LONG flags, BSTR rdn, BSTR usage, BSTR *request) {
        if( XECR_PKCS10_V2_0 == flags || XECR_PKCS10_V1_5 == flags ) {
            return createPKCSRequest(rdn, usage, request, true, false);
        } else if (flags == XECR_PKCS7) {
            return createPKCSRequest(rdn, usage, request, true, true);
	} else if (flags == XECR_PKCS7_TWICE) {
	    return createPKCSRequest(rdn, usage, request, true, 2);
        } else
            return E_NOTIMPL;
    }
    
    /*!
     *  \brief Обработка PKCS#7 ответа от УЦ
     *
     *  \param msg [in] PKCS#7 сообщение, закодированное в BASE64.
     *		        Содержит сертификат или цепочку 
     *		        сертификатов, соответствующую запросу
     *
     *	\return S_OK - успешно
     *
     *  \note
     *  Цепочка сертификатов, как может содержать корневой сертификат, 
     *  так и нет, в зависимости от политики конкретного УЦ. Сертификат,
     *  ссылающийся на созданный закрытый ключ из запроса, помещается
     *  в хранилище MY. Корневой сертификат помещается в хранилище
     *  ROOT, остальные сертификаты промежуточных УЦ помещаются в
     *  хранилище CA. Если в сообщение PKCS#7 встречается какой-либо
     *  корневой сертификат, то у пользователя запрашивается 
     *  подтверждение.
     *  Пользователь может запретить установку самоподписанных 
     *  сертификатов, которым он не доверяет по тем или иным причинам.
     *  Отказ пользователя в установке сертификатов в хранилище ROOT
     *  не вызывает отказа для всей операции.
     *
     *  \note
     *  По умолчанию используются хранилища: MY, CA, ROOT и REQUEST, 
     *  однако пользователь может изменить имена с помощью следующих 
     *  методов:
     *	    - put_MyStoreName()
     */
    virtual HRESULT acceptPKCS7( BSTR msg);

     /*!
     *  \brief Установить сертификат или цепочку сертификатов PKCS#7
     *
     *  Метод installPKCS7 в отличии от acceptPKCS7 не предназначен
     *  для установки сертификатов, выпущенных по запросу пользователя.
     *  Он предназанчен просто для установки сертификата или цепочки
     *  в стандартные хранилища сертификатов.
     *
     *  \param msg [in] PKCS#7 сообщение, закодированное в BASE64, 
     *		        которое содержит сертификат или цепочку 
     *
     *	\retval S_OK            успешно
     *	\retval ERROR_CANCELLED операция прервана пользователем 
     *
     *  \note
     *  Если в сообщение PKCS#7 встречается какой-либо
     *  корневой сертификат, то у пользователя запрашивается 
     *  подтверждение на его установку.
     */
    virtual HRESULT installPKCS7( BSTR msg);
    /*!
     *  \brief Установить сертификат или цепочку сертификатов PKCS#7
     *
     *  Метод installPKCS7Ex в отличии от acceptPKCS7 не предназначен
     *  для установки сертификатов, выпущенных по запросу пользователя.
     *  Он предназанчен просто для установки сертификата или цепочки
     *  в стандартные хранилища сертификатов.
     *
     *  \param msg [in] PKCS#7 сообщение, закодированное в BASE64, 
     *		        которое содержит сертификат или цепочку 
     *
     *  \param plCertInstalled [out] число сертификатов, 
     *		        установленных в локальные хранилища
     *
     *	\retval S_OK            успешно
     *	\retval ERROR_CANCELLED операция прервана пользователем 
     *
     *  \note
     *  Если в сообщение PKCS#7 встречается какой-либо
     *  корневой сертификат, то у пользователя запрашивается 
     *  подтверждение на его установку.
     */
    
    HRESULT installPKCS7Ex( BSTR msg, LONG * plCertInstalled);
     /*!
     *  \brief Установить сертификат, на котором будет подписан запрос в PKCS#7
     *  
     *
     *  \param pSignerCert  [in]  Информация о сертификате 
     *	\retval S_OK            успешно
     */
 
private:
    HRESULT findCertificateInRequestStore(
	const BYTE* pbCert, DWORD cbCert,
	std::vector<BYTE>& foundedCert,
	cpcrypt_store_handle& store) const;
    HRESULT createCerificateContextFromRequestStore(
	const BYTE* pbCert, DWORD cbCert,
	PCCERT_CONTEXT& pCertContext,
	cpcrypt_store_handle& handle) const;
    HRESULT installCertificateToStore(
	PCCERT_CONTEXT pPrivateKeyCertContext,
	const BYTE* pbCert, DWORD cbCert);
    std::string prepareKeyUsageString( const std::string& usage );
    HRESULT encodeRequestToPKCS7(std::vector<BYTE>& Request); 
    HRESULT prepareKeyUsage(
	const std::string& usage,
	std::vector<std::string>& usageArray);
};

#endif /* _UNIXENROLL_H */
