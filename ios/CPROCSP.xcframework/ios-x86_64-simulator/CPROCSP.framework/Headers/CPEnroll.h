#ifndef _CPENROLL_H
#define _CPENROLL_H

#include<CPROCSP/reader/support.h>
#include <string>
#include <memory>
#include <list>
#include <vector>
#include<CPROCSP/SecureBuffer.h>
#include<CPROCSP/BSTR.h>

#define XECR_PKCS10_V2_0     0x1
#define XECR_PKCS7           0x2
#define XECR_CMC             0x3
#define XECR_PKCS10_V1_5     0x4
#define XECR_PKCS7_TWICE     0x5

/*! \ingroup EnrollAPI
     *  \brief Создание запросов на сертификат и установка 
     *	   полученных сертификатов
     *
     *  \xmlonly <locfile><header>UnixEnroll.h</header><ulib>libenroll.so</ulib></locfile>\endxmlonly
*/
class CPEnroll
{
public:
    /*! \ingroup EnrollAPI
     *  \brief Интерфейс запросов пользователя
     *
     *  \xmlonly <locfile><header>UnixEnroll.h</header><ulib>libenroll.so</ulib></locfile>\endxmlonly
     */
    class UserCallbacks
    {
    public:
	/*! 
	 *  \brief Запрос на разрешение установки сертификата в 
	 *         хранилище доверенных сертификатов
	 *
	 *  \param pbCert   [in] Устанавливаемый сертификат
	 *  \param cbCert   [in] Длина сертификата
	 *
	 *  \return true - в случае согласия на установку сертификата
	 *
	 *  \note Реализовывать не обязательно, если используется 
	 *        put_SupressAddRootUI()
	 */
	virtual bool askPermissionToAddToRootStore( 
		const BYTE* pbCert, DWORD cbCert, bool = false) const=0;

	/*! 
	 *  \brief Клонирование
	 */
	virtual UserCallbacks* clone() const=0;

	/*! 
	 *  \brief Деструктор
	 */
	virtual ~UserCallbacks() {}
    };
public:
    

    /*! 
     *  \fn UnixEnroll::~UnixEnroll
     *  \brief Деструктор
     */
    virtual ~CPEnroll() {};
    /*! 
     *  \fn UnixEnroll::CPEFactory
     *  \brief Метод для создания объектов класса CPEnroll
     *  \param callbacks    [in] коллбеки для запроса разрешения на установку корневого сертификата. Используются 
     *  на Unix-системах. При вызове метода на Unix параметр callbacks должен быть задан. На Windows параметр игнорируется.
     *  \param handlePKCS7  [in] TRUE - объект будет работать с PKCS7, FALSE - объект будет работать с PKCS10.
     */
    static CPEnroll *CPEFactory(const UserCallbacks* callbacks = 0, bool handlePKCS7 = false);

    /*!
     *  \brief Установить атрибуты S/MIME в запрос PKCS#10 (не реализовано)
     *
     *  \param value	[in] TRUE - установить.
     *
     *	\return S_OK - в случае успеха 
     */
    virtual HRESULT put_EnableSMIMECapabilities( bool value){
	UNUSED(value);
	return E_NOTIMPL;
    }
    
    /*! 
     *  \brief Использовать существующий ключ в контейнере (не реализовано)
     *
     *  \param value	[in] TRUE - существующий, FALSE - создать новый.
     *
     *	\return S_OK - в случае успеха 
     */
    virtual HRESULT put_UseExistingKeySet(bool value){
	UNUSED(value);
	return E_NOTIMPL;
    }
    
    /*! 
     *  \brief Задать алгоритм хэш-функции для PKCS#10
     *
     *  \param hashOid	[in] OID хэш-функции.
     *
     *	\return S_OK - в случае успеха 
     */
    virtual HRESULT put_HashAlgorithm( BSTR hashOid){
	UNUSED(hashOid);
	return E_NOTIMPL;
    }
    
    /*! 
     *  \brief Задать имя контейнера
     *
     *  Если имя контейнера не задано, то оно генерируется как 
     *  случайный GUID. 
     *
     *  \param cName	[in] Имя контейнера.
     *
     *	\return S_OK - в случае успеха 
     *
     *  \note
     *  При использовании UseExistingKeySet, требуется
     *  задавать имя контейнера.
     */
    virtual HRESULT put_ContainerName( BSTR cName){
	UNUSED(cName);
	return E_NOTIMPL;
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
    virtual HRESULT put_ProviderType( DWORD pType){
	UNUSED(pType);
	return E_NOTIMPL;
    }
    
    /*! 
     *  \brief Задать имя CSP для создания ключа
     *
     *  Определяет CSP в котором создаётся закрытый ключ.
     *
     *  \param pName	[in] Имя CSP.
     *
     *	\return S_OK - в случае успеха 
     */
    virtual HRESULT put_ProviderName( BSTR pName){
	UNUSED(pName);
	return E_NOTIMPL;
    }
    
    /*! 
     *  \brief Задать свойства закрытого ключа
     *
     *  По умолчанию экспорт закрытого ключа запрещён, для его
     *  разрешения можно задать CRYPT_EXPORTABLE. 
     *
     *  \param keyFlags	[in] Флаги создания закрытого ключа.
     *
     *	\return S_OK - в случае успеха 
     *
     *  \note
     *  Полный список флагов смотри в описании CPGenKey() и CryptGenKey().
     */
    virtual HRESULT put_GenKeyFlags( DWORD keyFlags){
	UNUSED(keyFlags);
	return E_NOTIMPL;
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
    virtual HRESULT put_RequestStoreFlags( DWORD storeFlags){
	UNUSED(storeFlags);
	return E_NOTIMPL;
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
    virtual HRESULT put_ProviderFlags( DWORD provFlags){
	UNUSED(provFlags);
	return E_NOTIMPL;
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
     *  Смотри так же описания CPGenKey() и CryptGenKey().
     *
     *  \todo
     *  TODO:XXX В отличии от ICertEnroll нет значения по умолчанию ???
     */
    virtual HRESULT put_KeySpec( DWORD _dwKeySpec){
	UNUSED(_dwKeySpec);
	return E_NOTIMPL;
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
     *  Если значения свойства FALSE (0), то запрос содержит следующие 
     *  значение KeyUsage:
     *	    - CERT_DATA_ENCIPHERMENT_KEY_USAGE
     *	    - CERT_KEY_ENCIPHERMENT_KEY_USAGE
     *	    - CERT_DIGITAL_SIGNATURE_KEY_USAGE
     *	    - CERT_NON_REPUDIATION_KEY_USAGE
     *
     *  \note
     *  Если значения свойства TRUE (!=0), то запрос содержит следующие 
     *  значение KeyUsage:
     *	    - CERT_DATA_ENCIPHERMENT_KEY_USAGE
     *	    - CERT_KEY_ENCIPHERMENT_KEY_USAGE
     */
    virtual HRESULT put_LimitExchangeKeyToEncipherment( int value){
	UNUSED(value);
	return E_NOTIMPL;
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
    virtual HRESULT put_CAStoreFlags( DWORD flags){
	UNUSED(flags);
	return E_NOTIMPL;
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
    virtual HRESULT put_MyStoreFlags( DWORD flags){
	UNUSED(flags);
	return E_NOTIMPL;
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
    virtual HRESULT put_RootStoreFlags( DWORD flags){
	UNUSED(flags);
	return E_NOTIMPL;
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
    virtual HRESULT put_MyStoreName( BSTR name){
	UNUSED(name);
	return E_NOTIMPL;
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
     *  запрошенного сертификата, псевдосертификат удаляется. Однако,
     *  для целей отладки или тестирования можно запретить удаление 
     *  этого псевдосертификата установкой свойства DeleteRequestCert
     *  в значение false.
     */
    virtual HRESULT put_DeleteRequestCert( bool value){
	UNUSED(value);
	return E_NOTIMPL;
    }
    
    /*!
     *  \brief Требуется ли устанавливать сертификат в ключевой контейнер
     *
     *  Значение по умолчанию false.
     *  \todo TODO:XXX В отличии от ICertEnroll, у которого оно true ???
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
     *  не будет возвращать ошибку даже, если ключевой носитель не 
     *  сможет сохранить сертификат. В частности, если это значение
     *  true, но ключевой носитель (смарт-карта, USB-токен, 
     *  USB-флэш и т.п.) не подключен, то ошибки ключевого носителя 
     *  будут проигнорированны.
     * 
     *  \note
     *  Для того, что бы не вызывалась функция записи сертификата 
     *  требуется установить значение false.
     *
     *  \note
     *  Используется методами:
     *	    - acceptPKCS7()
     *
     *  \note
     *  Смотри также описания CPSetKeyParam() и CryptSetKeyParam().
     */
    virtual HRESULT put_WriteCertToCSP( bool value){
	UNUSED(value);
	return E_NOTIMPL;
    }

    /*!
     *  \brief Задать PIN (пароль) контейнера
     *
     *  Значение по умолчанию - не задан.
     *
     *  \param pin [in] PIN (пароль) контейнера.
     *
     *	\return S_OK - в случае успеха 
     *
     *  \note
     *  Задаёт PIN (пароль) для создании запроса или 
     *  получении сертификата для работы в режиме CRYPT_SILENT 
     *  (или при недоступности устройства текстового или графического 
     *  интерфейса пользователя (TUI или GUI)).
     */
    virtual HRESULT put_PIN( const CSecurePin& pin){
	UNUSED(pin);
	return E_NOTIMPL;
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
     *  При установке значения true, метод 
     *  UnixEnroll::askPermissionToAddToRootStore()
     *  не будет вызываться.
     */
    virtual HRESULT put_SupressAddRootUI( bool value){
	UNUSED(value);
	return E_NOTIMPL;
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
    virtual HRESULT addExtensionToRequest( LONG flags, BSTR name, BSTR value){
	UNUSED(flags);
	UNUSED(name);
	UNUSED(value);
	return E_NOTIMPL;
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
    virtual HRESULT get_InstallToContainerStatus( DWORD *pdwStatus){
	UNUSED(pdwStatus);
	return E_NOTIMPL;
    }

     /*! 
     *  \brief Возвращает имя контейнера
     *
     *	\retval S_OK		успешно
     */
    virtual HRESULT get_ContainerName ( BSTR *cName )
    {
	UNUSED ( cName );
	return E_NOTIMPL;
    }


    /*! 
     *  \brief Создать запрос PKCS#10
     *
     *  \param rdn     [in]  Различительное имя (DN) владельца ключа
     *  \param usage   [in]  Список OID расширенного использования 
     *			     ключа (Extended Key Usage (EKU))
     *  \param request [out] Запрос, закодированный в BASE64
     *
     *	\return S_OK - успешно
     *
     *  \note
     *  Игнорирует расширения заданные методом addExtensionToRequest()
     *
     *	\note
     *	Полученная BSTR строка запроса, должна быть освобождена 
     *  функцией SysFreeString()
     */
    virtual HRESULT createPKCS10( BSTR rdn, BSTR usage, BSTR *request){
	UNUSED(rdn);
	UNUSED(usage);
	UNUSED(request);
	return E_NOTIMPL;
    }
    
    /*! \ingroup EnrollAPI
     *  \brief Создать запрос сертификат
     *
     *  \param flags   [in]  Тип запроса на сертификат
     *  \param rdn     [in]  Различительное имя (DN) владельца ключа
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
     *  Добавляет расширения заданные методом addExtensionToRequest()
     *
     *	\note
     *	Полученная BSTR строка запроса, должна быть освобождена 
     *  функцией SysFreeString()
     */
    virtual HRESULT createRequest( LONG flags, BSTR rdn, BSTR usage, BSTR *request){
	UNUSED(flags);
	UNUSED(rdn);
	UNUSED(usage);
	UNUSED(request);
	return E_NOTIMPL;
    }
    
    /*! \ingroup EnrollAPI
     *  \brief Обработать PKCS#7 ответ от УЦ
     *
     *  \param msg [in] PKCS#7 сообщение, закодированное в BASE64, 
     *		        которое содержит сертификат или цепочку 
     *		        сертификатов соответствующую запросу
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
    virtual HRESULT acceptPKCS7( BSTR msg){
	UNUSED(msg);
	return E_NOTIMPL;
    }

     /*! \ingroup EnrollAPI
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
     *  \param plCertEncoded [out] число сертификатов, 
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
    virtual HRESULT installPKCS7( BSTR msg){
	UNUSED(msg);
	return E_NOTIMPL;
    }

    /*! \ingroup EnrollAPI
     *  \brief Установить сертификат, на котором будет подписан запрос в PKCS#7
     *  
     *
     *  \param pSignerCert  [in]  Информация о сертификате 
     *	\retval S_OK            успешно
     */
 
    virtual HRESULT SetSignerCertificate(
      /* [in] */  PCCERT_CONTEXT pSignerCert
      ){
	UNUSED(pSignerCert);
	return E_NOTIMPL;
    }

    virtual HRESULT AddCProLicenseExt(){
	return E_NOTIMPL;
    }
};

#endif /* _CPENROLL_H */
