/*
 * Copyright(C) 2004-2006 Проект ИОК
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
 * \version $Revision: 194763 $
 * \date $Date:: 2019-06-17 06:39:27 -0700#$
 * \author $Author: sdenis $
 *
 * \brief Классы для работы с OCSP (Online Certificate Status Protocol)
 */

#ifndef _OCSP_H_INCLUDED
#define _OCSP_H_INCLUDED

#if !defined UNIX
#   include <windows.h>
#   include <wincrypt.h>
#endif	/* !UNIX */

#include <list>
#include <cplib/Blob.h>
#include <cplib/DateTime.h>
#include <asn1/CMP.h>
#include <asn1/algidex.h>

#if defined _WIN32 && !defined CRYPTCP && !defined OCSP_STATIC
#   if defined OCSP_DLL
#	define OCSP_CLASS __declspec(dllexport)
#	define OCSP_API __declspec(dllexport)
#	define OCSP_DATA __declspec(dllexport)
#	define OCSP_EXTERN_TEMPLATE
#   else // defined OCSP_DLL
#	define OCSP_CLASS __declspec(dllimport)
#       define OCSP_API __declspec(dllimport)
#	define OCSP_DATA __declspec(dllimport)
#       define OCSP_EXTERN_TEMPLATE extern
#       ifndef CP_IGNORE_PRAGMA_MANIFEST
#           include "pki_ocsp_assembly.h"
#           ifdef _WIN64
#               pragma comment(linker,"/manifestdependency:\"type='win32' " \
                "name='" PKI_OCSP_ASSEMBLY_NAME_X64 "' " \
                "version='" PKI_OCSP_ASSEMBLY_VERSION_X64 "' " \
                "processorArchitecture='amd64' " \
                "language='*' " \
                "publicKeyToken='" PKI_OCSP_ASSEMBLY_PUBLICKEYTOKEN_X64 "'\"")
#           else
#               pragma comment(linker,"/manifestdependency:\"type='win32' " \
                "name='" PKI_OCSP_ASSEMBLY_NAME_X86 "' " \
                "version='" PKI_OCSP_ASSEMBLY_VERSION_X86 "' " \
                "processorArchitecture='x86' " \
                "language='*' " \
                "publicKeyToken='" PKI_OCSP_ASSEMBLY_PUBLICKEYTOKEN_X86 "'\"")
#           endif
#       endif
#   endif // !defined OCSP_DLL
#else // defined _WIN32 && !defined CRYPTCP && !defined OCSP_STATIC
#   define OCSP_CLASS
#   define OCSP_API
#   define OCSP_DATA
#   define OCSP_EXTERN_TEMPLATE
#   define NO_EXPIMP_CDLLLIST_ITERATORS
#endif // !defined _WIN32

namespace CryptoPro {
namespace PKI {
namespace OCSP {

/**
 * \mainpage Низкоуровневый интерфейс КриптоПро OCSP
 *
 * Библиотека OCSP предоставляет низкоуровневый интерфейс для работы с 
 * протоколом OCSP (Online Certificate Status Protocol, RFC 2560, 
 * draft-ietf-pkix-ocspv2-ext-01).

 * Низкоуровневый интерфейс из состава КриптоПро OCSP SDK предназначен для работы с протоколом проверки статуса сертификатов (OCSP). Использование данного интерфейса упрощает реализацию служб OCSP и делает возможным встраивание службы в существующую систему.
 * 
 * Низкоуровневый интерфейс представляет собой библиотеку классов C++.
 *
 * В библиотеке реализована поддержка структур данных, описанных в RFC 2560 и draft-ietf-pkix-ocspv2-ext-01.
 *
 * \section contents Состав
 *
 * Библиотека предоставляет классы для работы с OCSP запросами и ответами:
 *  - \link CryptoPro::PKI::OCSP::CRequestMessage CRequestMessage\endlink - OCSP запрос
 *  - \link CryptoPro::PKI::OCSP::CResponseMessage CResponseMessage\endlink - OCSP ответ
 *
 * Также предоставляются вспомогательные классы:
 *  - \link CryptoPro::PKI::OCSP::CSingleResponse CSingleResponse\endlink - запрос статуса отдельного сертификата;
 *  - \link CryptoPro::PKI::OCSP::CCertID CCertID\endlink - идентификатор сертификата (из хэша имени и публичного ключа издателя,
 *		    а также серийного номера сертификата);
 *  - \link CryptoPro::PKI::OCSP::CCertIdWithSignature CCertIdWithSignature\endlink - идентификатор сертификата (из имени издателя, серийного номера
 *		    и подписи сертификата);
 *  - \link CryptoPro::PKI::OCSP::CReqCert CReqCert\endlink - структура, содержащая идентификатор сертификата;
 *  - \link CryptoPro::PKI::OCSP::CBasicResponse CBasicResponse\endlink - базовый ответ, содержащийся в OCSP_ResponseMessage;
 *  - \link CryptoPro::PKI::OCSP::CSingleResponse CSingleResponse\endlink - ответ на один запрос о статусе сертификата (OCSP_SingleResponse)
 *  - \link CryptoPro::PKI::OCSP::CCertStatus CCertStatus\endlink - статус сертификата
 *
 * \section usage Особенности
 *
 * Доступ к полям структур в библиотеке осуществляется следующим способом:
 * - если поле обязательное, то оно устанавливается и возвращается как ссылка
 *   на объект. Эта ссылка дейcтвительна только на время жизни объекта. После того, как
 *   отработал деструктор объекта, содержание этой ссылки неопределено;
 * - если поле опциональное, то оно устанавливается и возвращается как указатель
 *   на объект. Если указатель равен NULL, это означает, что поле отсутствует.
 *
 * Основные классы библиотеки (запросы, ответы) содержат методы encode()/decode(),
 * выполняющие операцию кодирования в DER-представление и декодирования из него. Классы, работающие
 * со структурами, которые содержат подпись (\link CryptoPro::PKI::OCSP::CRequestMessage CRequestMessage\endlink,
 * \link CryptoPro::PKI::OCSP::CBasicResponse CBasicResponse\endlink), 
 * обеспечивают операции подписи и проверки подписи (sign()/verify()), а также доступ 
 * к информации о подписи (SignerInfo).
 * Используя конструктор по умолчанию, можно создать пустой объект, для последующего вызова функции decode(). Во всех остальных случаях, если существует конструктор с аргументами, использование конструктора по умолчанию не рекомендуется, так как перед вызовом encode() все обязательные поля структуры должны быть проинициализированы корректными значениями. Конструкторы с аргументами заполняют эти поля при создании объекта, а конструкторы по умолчанию - нет, что допускает вероятность того, что объект не будет правильно проинициализирован перед вызовом encode(). 
 *
 * Представление объектов в виде DER - Distinguished Encoding Rules, используется для
 * их транспортировки между клиентом и сервером. Таким образом, есть два сценария
 * работы с классами библиотеки:
 *    - создание сообщения (запроса или ответа) и его кодирование в DER для последующей 
 *	отправки;
 *    - получение сообщения в DER-представлении, его декодирование и извлечение содержащейся
 *      в нем информации.
 *
 * При возникновении ошибок методы классов данной библиотеки 
 * могут генерировать следующие виды исключений: std::exception и ATL::CAtlException.
 *
 * \section s1 Создание сообщения
 *
 * Основные этапы создания сообщения:
 *	-# Создание объекта
 *	-# Заполнение полей в случае необходимости (добавление опциональных или изменение
 *	   обязательных, заданных по умолчанию)
 *	-# В случае, если сообщение содержить подпись, вызов метода sign()
 *	-# Вызов метода encode() для получения DER-представления объекта в виде блока
 *	   бинарных данных
 * 
 * Пример (создание OCSP запроса):
 * \dontinclude ocsp_sample.cpp
 * \skip example_3_begin
 * \skip создание списка запросов
 * \until CBlob encodedRequestMessage = message.encode()
 * 
 * Основные методы декодирования сообщения:
 *	-# Создание объекта конструктором по умолчанию
 *	-# Вызов метода decode(), аргументом которого является DER-представление сообщения
 *	-# Вызов методов get_...() для получения полей сообщения.
 * 
 * Пример (создание OCSP ответа):
 * \dontinclude ocsp_sample.cpp
 * \skip example_4_begin
 * \skip создание списка ответов
 * \until CBlob encodedResponseMessage = response.encode()
 */

#define sz_id_kp_OCSPSigning "1.3.6.1.5.5.7.3.9"
#define sz_id_pkix_ocsp_basic "1.3.6.1.5.5.7.48.1.1"
#define sz_id_pkix_ocsp_crl "1.3.6.1.5.5.7.48.1.3"
#define sz_id_pkix_ocsp_nonce "1.3.6.1.5.5.7.48.1.2"
#define sz_id_pkix_ocsp_service_locator "1.3.6.1.5.5.7.48.1.7"
#define sz_id_pkix_ocsp_response "1.3.6.1.5.5.7.48.1.4"
#define sz_id_pkix_ocsp_archive_cutoff "1.3.6.1.5.5.7.48.1.6"
#define sz_id_CryptoPro_ocsp_treats_exp_key_or_exp_cert_rev "1.2.643.2.2.47.1"
#define sz_id_CryptoPro_ocsp_crl_locator "1.2.643.2.2.47.2"
#define sz_id_CryptoPro_ocsp_instant_revocation_indicator "1.2.643.2.2.47.3"
#define sz_id_CryptoPro_ocsp_revocation_announcement_reference "1.2.643.2.2.47.4"
#define sz_id_CryptoPro_ocsp_historical_request "1.2.643.2.2.47.5"

/// Версия OCSP (RFC 2560, раздел 4.1.1)
enum Version
{
    Version_1 = 0,
    Version_2 = 1
};

/// Cтатусы OCSP ответа (RFC 2560, раздел 4.2.1)
enum ResponseStatus
{
    /// запрос обработан успешно
    ResponseStatus_successful = 0,
    /// неправильный запрос
    ResponseStatus_malformedRequest =  1,
    /// запрос не обработан из-за внутренней ошибки
    ResponseStatus_internalError = 2,
    /// сервис временно не может ответить, попробуйте позже
    ResponseStatus_tryLater = 3,
    /// запрос должен быть подписан
    ResponseStatus_sigRequired = 5,
    /// отправитель запроса не уполномочен осуществлять такой запрос
    ResponseStatus_unauthorized = 6,
    /// ошибка при обработке CRL
    ResponseStatus_badCRL = 8
};

class CSingleRequestList;
class CSingleRequest;

// VS2008 bug/feature: Экспорт инстанцированных шаблонов вложенных классов
// (здесь - итераторов) должен предварять экспорт инстанцированного шаблона
// внешнего класса (здесь - CDllList<>), иначе из DLL не будут экспортированы
// методы вложенных классов.
EXPIMP_CDLLLIST_ITERATORS(CSingleRequest, OCSP_EXTERN_TEMPLATE, OCSP_CLASS);

class CReqCert;

/**
 * \class CRequestMessage
 * \brief Запрос на проверку статуса сертификата(ов) (RFC 2560 раздел 4.1.1)
 *
 * Соответвует ASN.1 структуре OCSPRequest. Предоставляет операции подписи sign() и
 * проверки подписи verify(). Подпись опциональна, то есть сформировать и закодировать
 * запрос можно как с вызовом sign() так и без него. В первом случае запрос будет подписанным,
 * во втором - нет.
 * \sa CSingleRequest, CDllList
 */
class OCSP_CLASS CRequestMessage
{
public:
    /// Создает пустой объект
    CRequestMessage();
    /// Уничтожает объект
    ~CRequestMessage();

    /**
     * \brief Создает копию заданного объекта.
     * \param src [in] объект, копия которого создается
     */
    CRequestMessage( const CRequestMessage& src);
    /**
     * \brief Копирует заданный объект в текущий.
     * \param src [in] объект, который копируется в текущий
     * \return Ссылка на текущий объект.
     */
    CRequestMessage& operator=( const CRequestMessage& src);

    /**
     * \brief Кодирует объект в формате ASN.1 DER и возвращает
     * получившуюся двоичную последовательность
     * \sa decode()
     */
    CBlob encode() const;
    /**
     * \brief Инициализирует объект из заданной закодированной двоичной последовательности
     * \sa encode()
     */
    void decode( const CBlob& encodedRequest);

    /**
     * \brief Подпись запроса
     *
     * \param hCryptProv [in] контекст криптопровайдера, открытый на закрытом ключе службы
     * \param dwKeySpec [in] назначение ключа
     * \param signatureAlgorithm [in] алгоритм, который будет использоваться для подписи данных
     * \param additionalCertificates [in] дополнительные сертификаты, вкладываемые в запрос
     *
     * \remark Данная функция должна вызываться после заполнения всех полей, непосредственно
     *		перед кодированием запроса
     * \sa verify()
     */
    void sign(
	HCRYPTPROV hCryptProv,
	DWORD dwKeySpec,
	const ASN1::CAlgorithmIdentifierEx& signatureAlgorithm,
	const ASN1::CEncodedCertificateList* additionalCertificates = 0);
    /// проверка подписи сообщения (ВАЖНО: подписи может не быть)
    /**
     * \brief Проверка подписи запроса
     * 
     * \param encodedCertificate [in] закодированное двоичное представление 
     * сертификата использующегося для проверки подписи
     *
     * \return true, если подпись проверена успешно, иначе false
     * \remark Так как подпись опциональна, то перед вызывом функции
     * необходимо убедится, что запрос подписан. В случае попытки проверить
     * неподписанный запрос генерируется исключение.
     */
    bool verify(
	const CBlob& encodedCertificate) const;

    // get (возврат указателя обозначает опциональный атрибут)
    /// возвращает версию запроса
    Version get_version() const;
    /// возвращает закодированное имя отправителя запроса (опциональное поле)
    const CBlob* get_requestorName() const;
    /// возвращает список запросов на статус отдельных сертификатов
    const CSingleRequestList& get_requestList() const;
    /// возвращает список расширений, вложенных в запрос
    const ASN1::CExtensions* get_requestExtensions() const;
    // опциональные аттрибуты заполняющиеся при вызове sign()
    /**
     * \brief возвращает алгоритм подписи
     * \remark Это опциональное поле заполняется после вызова sign()
     */
    const ASN1::CAlgorithmIdentifierEx* get_signatureAlgorithm() const;
    /**
     * \brief возвращает двоичное значение подписи
     * \remark Это опциональное поле заполняется после вызова sign()
     */
    const CBlob* get_signature() const;
    /**
     * \brief возвращает вложенные в запрос сертификаты
     * \remark Это опциональное поле заполняется после вызова sign()
     */
    const ASN1::CEncodedCertificateList* get_signatureCerts() const;
    // set (только опциональные аттрибуты, NULL убирает атрибут)
    /// устанавливает имя отправителя запроса (опциональное поле)
    void put_requestorName( const CBlob* encodedName);
    /// устанавливает список запросов на статус отдельных сертификатов
    void put_requestList( const CSingleRequestList& requestList);
    /// устанавливает список расширений вложенных в запрос
    void put_requestExtensions( const ASN1::CExtensions* extensions);    
private:
    void clear();
    class Impl;
    Impl* pImpl_;
};

/**
 * \brief Запрос на статус одного сертификата (RFC 2560, раздел 4.1.1)
 *
 * ASN.1 структура Request. Содержит идентификатор сертификата, статус
 * которого запрашивается, и опционалный список расширений.
 * \sa CRequestMessage, CReqCert
 */
class OCSP_CLASS CSingleRequest
{
public:
    CSingleRequest();
    /**
     * \brief Создает и инициализирует объект
     * \param reqCert [in] идентификатор сертификата
     */
    CSingleRequest( const CReqCert& reqCert);

    /**
     * \brief Создает копию заданного объекта.
     * \param src [in] объект, копия которого создается
     */
    CSingleRequest( const CSingleRequest& src);
    /**
     * \brief Копирует заданный объект в текущий.
     * \param src [in] объект, который копируется в текущий
     * \return Ссылка на текущий объект.
     */
    CSingleRequest& operator=( const CSingleRequest& src );

    /// Уничтожает объект
    ~CSingleRequest();

    // get (возврат указателя обозначает опциональный атрибут)
    /// возвращает идентификатор сертификата
    const CReqCert& get_reqCert() const;
    /// возвращает список расширений, содержащихся в запросе (опциональное поле)
    const ASN1::CExtensions* get_singleRequestExtensions() const;
    // set (только опциональные аттрибуты, NULL убирает атрибут)
    /// устанавливает список расширений, содержащихся в запросе (опциональное поле)
    void put_singleRequestExtensions( const ASN1::CExtensions* extensions);
private:
    void clear();
    class Impl;
    Impl* pImpl_;
};

/**
 * \class CSingleRequestList
 * \brief Список объектов CSingleRequest
 *
 * \sa CSingleRequest, CDllList
 */
class OCSP_CLASS CSingleRequestList: public CDllList<CSingleRequest>
{
};

/**
 * \brief ASN.1 структура CertID. Идентификатор сертификата (RFC 2560, раздел 4.1.1).
 *
 * Сертификат идентифицируется хэшами имени и открытого ключа издателя сертификата,
 * а также серийным номером.
 * \remark Так как в идентификаторе участвует хэш открытого ключа издателя, то для создания
 * такого идентификатора сертификата необходимо найти также и сертификат издателя.
 * \sa CReqCert, CCertIdWithSignature
 */
class OCSP_CLASS CCertID
{
public:
    /// Создает пустой объект
    CCertID();
    /**
     * \brief Создает и инициализирует объект
     * \param hashAlgorithm [in] алгоритм содержащихся в идентификаторе хэшей
     * \param issuerNameHash [in] значение хэша закодрованного имени издателя сертификата
     * \param issuerKeyHash [in] значение хэша открытого ключа издателя сертификата
     * \param serialNumber [in] серийный номер сертификата
     */
    CCertID(
	const ASN1::CAlgorithmIdentifier& hashAlgorithm,
	const CBlob& issuerNameHash,
	const CBlob& issuerKeyHash,
	const ASN1::CBigInteger& serialNumber);
    /**
     * \brief Создает и инициализирует объект
     * \param hashAlgorithm [in] алгоритм содержащихся в идентификаторе хэшей
     * \param certificate [in] закодированный сертификат, идентификатор которого создается
     * \param issuerCertificate [in] закодированный сертификат издателя
     */
    CCertID(
	const ASN1::CAlgorithmIdentifierEx& hashAlgorithm,
	const CBlob& certificate,
	const CBlob& issuerCertificate);
    /// Уничтожает объект
    ~CCertID();

    /**
     * \brief Создает копию заданного объекта.
     * \param src [in] объект, копия которого создается
     */
    CCertID( const CCertID& src);
    /**
     * \brief Копирует заданный объект в текущий.
     * \param src [in] объект, который копируется в текущий
     * \return Ссылка на текущий объект.
     */
    CCertID& operator=( const CCertID& src);

    /**
     * \brief Кодирует объект в формате ASN.1 DER и возвращает
     * получившуюся двоичную последовательность
     * \sa decode()
     */
    CBlob encode() const;
    /**
     * \brief Инициализирует объект из заданной закодированной двоичной последовательности
     * \sa encode()
     */
    void decode( const CBlob& encoded);

    //get
    /// возвращает поле hashAlgorithm
    const ASN1::CAlgorithmIdentifier& get_hashAlgorithm() const;
    /// возвращает поле issuerNameHash
    const CBlob& get_issuerNameHash() const;
    /// возвращает поле issuerKeyHash
    const CBlob& get_issuerKeyHash() const;
    /// возвращает поле serialNumber
    const ASN1::CBigInteger& get_serialNumber() const;
    //set
    /// устанавливает поле hashAlgorithm
    void put_hashAlgorithm(
	const ASN1::CAlgorithmIdentifier& hashAlgorithm);
    /// устанавливает поле issuerNameHash
    void put_issuerNameHash( const CBlob& issuerNameHash);
    /// устанавливает поле issuerKeyHash
    void put_issuerKeyHash( const CBlob& issuerKeyHash);
    /// устанавливает поле serialNumber
    void put_serialNumber( const ASN1::CBigInteger& serialNumber);
private:
    ASN1::CAlgorithmIdentifier hashAlgorithm_;
    CBlob issuerNameHash_;
    CBlob issuerKeyHash_;
    ASN1::CBigInteger serialNumber_;
};

/// проверяет объекты на равенство
OCSP_API bool operator==( const CCertID& lhs, const CCertID& rhs);
/// проверяет объекты на неравенство
OCSP_API bool operator!=( const CCertID& lhs, const CCertID& rhs);

/**
 * \brief ASN.1 структура CertIdWithSignature. Идентификатор сертификата (draft-ietf-pkix-ocspv2-ext-01.txt, раздел 5.1.1).
 *
 * Сертификат идентифицируется хэшом закодированного поля tbsCertificate
 * сертификата, с использованием алгоритмя хэша соответствующего алгоритму подписи
 * сертификата, значением подписи, именем издателя и серийным номером.
 * \sa CReqCert, CCertID
 */
class OCSP_CLASS CCertIdWithSignature
{
public:
    /// Создает пустой объект
    CCertIdWithSignature();
    /**
     * \brief Создает и инициализирует объект
     * \param issuer [in] закодированное имя издателя
     * \param serialNumber [in] серийный номер
     * \param tbsCertificateHash [in] значение хэша закодированного поля tbsCertificate
     * \param signatureAlgorithm [in] алгоритм подписи
     * \param signatureValue [in] значение подписи
     */
    CCertIdWithSignature(
	const CBlob& issuer,
	const ASN1::CBigInteger& serialNumber,
	const CBlob& tbsCertificateHash,
	const ASN1::CAlgorithmIdentifier& signatureAlgorithm,
	const CBlob& signatureValue);
    /**
     * \brief Создает и инициализирует объект
     * \param certificate [in] закодированный сертификат, идентификатор которого создается
     */
    CCertIdWithSignature( const CBlob& certificate);
    /// Уничтожает объект
    ~CCertIdWithSignature();

    /**
     * \brief Создает копию заданного объекта.
     * \param src [in] объект, копия которого создается
     */
    CCertIdWithSignature( const CCertIdWithSignature& src);
    /**
     * \brief Копирует заданный объект в текущий.
     * \param src [in] объект, который копируется в текущий
     * \return Ссылка на текущий объект.
     */
    CCertIdWithSignature& operator=( 
	const CCertIdWithSignature& src);

    /**
     * \brief Кодирует объект в формате ASN.1 DER и возвращает
     * получившуюся двоичную последовательность
     * \sa decode()
     */
    CBlob encode() const;
    /**
     * \brief Инициализирует объект из заданной закодированной двоичной последовательности
     * \sa encode()
     */
    void decode( const CBlob& encoded);

    //get
    /// возвращает закодированное имя издателя сертификата
    const CBlob& get_issuer() const;
    /// возвращает серийный номер сертификата
    const ASN1::CBigInteger& get_serialNumber() const;
    /// возвращает хэш закодированного поля tbsCertificate сертификата
    const CBlob& get_tbsCertificateHash() const;
    /// возвращает алгоритм подписи сертификата
    const ASN1::CAlgorithmIdentifier& get_signatureAlgorithm() const;
    /// возвращает значение  подписи сертификата
    const CBlob& get_signatureValue() const;
    //set
    /// устанавливает значение закодированного имени изателя сертификата
    void put_issuer( const CBlob& issuer);
    /// устанавливает значение серийного номера сертификата
    void put_serialNumber( const ASN1::CBigInteger& serialNumber);
    /// устанавливает значение хэша поля tbsCertificate сертификата
    void put_tbsCertificateHash( const CBlob& tbsCertificateHash);
    /// устанавливает значение алгоритма подписи сертификата
    void put_signatureAlgorithm( 
	const ASN1::CAlgorithmIdentifier& signatureAlgorithm);
    /// устанавливает значение подписи сертификата
    void put_signatureValue( const CBlob& signatureValue);
private:
    CBlob issuer_;
    ASN1::CBigInteger serialNumber_;
    CBlob tbsCertificateHash_;
    ASN1::CAlgorithmIdentifier signatureAlgorithm_;
    CBlob signatureValue_;
};

/// проверяет объекты на равенство
OCSP_API bool operator==( 
    const CCertIdWithSignature& lhs, const CCertIdWithSignature& rhs);
/// проверяет объекты на неравенство
OCSP_API bool operator!=(
    const CCertIdWithSignature& lhs, const CCertIdWithSignature& rhs);

/**
 * \brief ASN.1 структура ReqCert (draft-ietf-pkix-ocspv2-ext-01.txt, раздел 5.1.1)
 *
 * Имеет три типа содержимого: CCertID, CCertIdWithSignature
 * и просто сертификат в закодированном виде(fullCertificate). Тип содержимого
 * определяет версию OCSP. Если в сообщении все идентификаторы типа CCertID, то
 * версия равна v1, если присутствует хотя бы один идентификатор другого
 * типа, то версия равна v2.
 */
class OCSP_CLASS CReqCert
{
public:
    /// тип сертификата
    enum fullCertificateType 
    { 
	/// сертификат открытого ключа
	Certificate, 
	/// сертификат свойств
	AttributeCertificate 
    };

    CReqCert();
    /**
     * \brief Создает и инициализирует объект
     * \param certId [in] идентификатор сертификата типа CCertID
     */
    CReqCert( const CCertID& certId);
    /**
     * \brief Создает и инициализирует объект
     * \param type [in] тип сертификата
     * \param encodedCertificate [in] закодированный сертификат
     */
    CReqCert( fullCertificateType type, const CBlob& encodedCertificate);
    /**
     * \brief Создает и инициализирует объект
     * \param certIdWithSignature [in] идентификатор сертификата типа CCertIdWithSignature
     */
    CReqCert( const CCertIdWithSignature& certIdWithSignature);

    /**
     * \brief Создает копию заданного объекта.
     * \param src [in] объект, копия которого создается
     */
    CReqCert( const CReqCert& src);
    /**
     * \brief Копирует заданный объект в текущий.
     * \param src [in] объект, который копируется в текущий
     * \return Ссылка на текущий объект.
     */
    CReqCert& operator=( const CReqCert& src);

    /// Уничтожает объект
    ~CReqCert();

    //get (возврат указателя обозначает опциональный атрибут)
    // из функций get не 0 возвращают только соответсвующие
    // типу идентификатора содержащегося в объекте
    /// возвращает идентификатор сертификата типа CCertID если он содержится в структуре, иначе - 0
    const CCertID* get_certID() const;
    /// возвращает сертификат если он содержится в структуре, иначе - 0
    const CBlob* get_fullCert() const;
    /// возвращает тип сертификата если он содержится в структуре, иначе - 0
    const fullCertificateType* get_fullCertType() const;
    /// возвращает идентификатор сертификата типа CCertIdWithSignature, если он содержится в структуре, иначе - 0
    const CCertIdWithSignature* get_certIdWithSignature() const;
    //set (изменение идентификатора сертификата)
    /// заполняет структуру объектом типа CCertID
    void put_certID( const CCertID& certId);
    /**
     * \brief заполняет структуру закодированным представлением сертификата
     * \param type [in] тип сертификата
     * \param encodedCertificate [in] закодированный сертификат
     */
    void put_fullCert( fullCertificateType type, const CBlob& encodedCertificate);
    /// заполняет структуру объектом типа CCertIdWithSignature
    void put_certIdWithSignature( const CCertIdWithSignature& certIdWithSignature);
public:
    class Impl;
private:
    void clear();
    Impl* pImpl_;
};

// response
class CCertStatus;
/**
 * \brief ASN.1 структура SingleResponse (RFC 2560, раздел 4.2.1)
 * 
 * Ответ на отдельный запрос на проверку сертификата на отзыв.
 * Содержит идентификатор сертификата (поле reqCert), определенный статус
 * сертификата (поле certStatus) и время, на которое эта информация
 * была верной (поле thisUpdate). Также ответ может содержать время
 * следующего обновления (поле nextUpdate) и дополнительные
 * расширения (поле singleExtensions).
 * 
 * \sa CResponseMessage, CBasicResponse, CSingleRequest
 */
class OCSP_CLASS CSingleResponse
{
public:
    /// Создает пустой объект
    CSingleResponse();
    /**
     * \brief Создает и инициализирует объект
     * \param reqCert [in] идентификатор сертификата
     * \param certStatus [in] статус сертификата
     * \param thisUpdate [in] время, на которое была действительна информация в ответе
     */
    CSingleResponse(
	const CReqCert& reqCert,
	const CCertStatus& certStatus,
	const CDateTime& thisUpdate);
    /**
     * \brief Создает копию заданного объекта.
     * \param src [in] объект, копия которого создается
     */
    CSingleResponse( const CSingleResponse& src);
    /**
     * \brief Копирует заданный объект в текущий.
     * \param src [in] объект, который копируется в текущий
     * \return Ссылка на текущий объект.
     */
    CSingleResponse& operator=( const CSingleResponse& src);

    /// Уничтожает объект
    ~CSingleResponse();

    // get (возврат указателя обозначает опциональный атрибут)
    /// возвращает идентификатор сертификата
    const CReqCert& get_reqCert() const;
    /// возвращает статус сертификата
    const CCertStatus& get_certStatus() const;
    /// возвращает время, на которое была действительна информация в ответе
    const CDateTime& get_thisUpdate() const;
    /// возвращает время, когда будет доступна более свежая информация о статусе этого сертификата (опциональное поле)
    const CDateTime* get_nextUpdate() const;
    /// возвращает список расширений, содержащийся в ответе (опциональное поле)
    const ASN1::CExtensions* get_singleExtensions() const;
    // set (только опциональные аттрибуты, NULL убирает атрибут)
    /// устанавливает время, когда будет доступна более свежая информация по статусу сертификата (опциональное поле)
    void put_nextUpdate( const CDateTime* nextUpdate);
    /// устанавливает список расширений (опциональное поле)
    void put_singleExtensions( const ASN1::CExtensions* extensions);
private:
    void clear();
    class Impl;
    Impl* pImpl_;
};

enum UnknownInfo { ocspUINull = 0xffff };

// VS2008 bug/feature: Экспорт инстанцированных шаблонов вложенных классов
// (здесь - итераторов) должен предварять экспорт инстанцированного шаблона
// внешнего класса (здесь - CDllList<>), иначе из DLL не будут экспортированы
// методы вложенных классов.
EXPIMP_CDLLLIST_ITERATORS(CSingleResponse, OCSP_EXTERN_TEMPLATE, OCSP_CLASS);

/**
 * \class CSingleResponseList
 * \brief Список объектов CSingleResponse
 *
 * \sa CSingleResponse, CDllList
 */
class OCSP_CLASS CSingleResponseList: public CDllList<CSingleResponse>
{
};
class CBasicResponse;
class CCertStatus;
class CResponderID;

/**
 * \brief OCSP ответ (RFC 2560, раздел 4.2.1)
 * 
 * ASN.1 структура OCSPResponse. Содержит в себе статус ответа, тип ответа, задаваемый OID'ом
 * (поле responseType) и закодированный ответ (поле response), содержи. Поля responseType и 
 * response опциональны, и не содержатся в ответе при неудачной обработке запроса.
 * \sa BasicResponse
 */
class OCSP_CLASS CResponseMessage
{
public:
    /**
     * \brief Создает пустой объект
     * \remark responseStatus по умолчанию internalError
     */
    CResponseMessage();

    /// Уничтожает объект
    ~CResponseMessage();

    /**
     * \brief Создает копию заданного объекта.
     * \param src [in] объект, копия которого создается
     */
    CResponseMessage( const CResponseMessage& src);
    /**
     * \brief Копирует заданный объект в текущий.
     * \param src [in] объект, который копируется в текущий
     * \return Ссылка на текущий объект.
     */
    CResponseMessage& operator=( const CResponseMessage& src);

    /**
     * \brief Кодирует объект в формате ASN.1 DER и возвращает
     * получившуюся двоичную последовательность
     * \sa decode()
     */
    CBlob encode() const;
    /**
     * \brief Кодирует объект в формате ASN.1 DER и возвращает
     * получившуюся двоичную последовательность
     * \sa decode()
     */
    void decode( const CBlob& encodedResponse);

    // get (возврат указателя обозначает опциональный атрибут)
    /// возвращает поле responseStatus
    ResponseStatus get_responseStatus() const;
    /// возвращает поле responseType
    const char* get_responseType() const;
    /// возвращает поле response
    const CBlob* get_response() const;
    // set 
    /// устанавливает поле responseStatus
    void put_responseStatus(ResponseStatus status);
    /**
     * \brief устанавливает значение ответа
     * \param responseType [in] OID типа ответа
     * \param response [in] закодированное представление ответа
     */
    void put_response(const char *responseType, const CBlob *response);
private:
    void clear();
    class Impl;
    Impl* pImpl_;
};

/**
 * \brief ASN.1 структура BasicOCSPResponse (RFC 2560, раздел 4.2.1)
 * 
 * OCSP ответ типа sz_id_pkix_ocsp_basic. Содержит список ответов на запросы
 * статусов сертификатов (поле responses) и дополнительную информацию о времени 
 * создания ответа (поле producedAt) и о сертификате ответчика (поле responderID).
 * В ответе также могут содержаться дополнительные расширения. Ответ должен быть подписан,
 * для этого предочтавляются функции создания и проверки подписи (sign()/verify()).
 * 
 * \sa CResponseMessage, CSingleResponse
 */
class OCSP_CLASS CBasicResponse
{
public:
    /**
     * \brief Создает и инициализирует объект
     * \param producedAt [in] время создания ответа
     * \param responderId [in] идентификатор ответчика
     */
    CBasicResponse( 
	const CDateTime& producedAt,
	const CResponderID& responderId);

    /// Создает пустой объект
    CBasicResponse();

    /// Уничтожает объект
    ~CBasicResponse();

    /**
     * \brief Создает копию заданного объекта.
     * \param src [in] объект, копия которого создается
     */
    CBasicResponse( const CBasicResponse& src);
    /**
     * \brief Копирует заданный объект в текущий.
     * \param src [in] объект, который копируется в текущий
     * \return Ссылка на текущий объект.
     */
    CBasicResponse& operator=( const CBasicResponse& src);

    /**
     * \brief Кодирует объект в формате ASN.1 DER и возвращает
     * получившуюся двоичную последовательность
     * \sa decode()
     */
    CBlob encode() const;
    /**
     * \brief Инициализирует объект из заданной закодированной двоичной последовательности
     * \sa encode()
     */
    void decode( const CBlob& encodedResponse);

    /**
     * \brief Подпись ответа
     *
     * \param hCryptProv [in] контекст криптопровайдера, открытый на закрытом ключе службы
     * \param dwKeySpec [in] назначение ключа
     * \param signatureAlgorithm [in] алгоритм, который будет использоваться для подписи данных
     *
     * \remark Данная функция должна вызываться после заполнения всех полей, непосредственно
     *		перед кодированием ответа.
     * \sa verify()
     */
    void sign(
	HCRYPTPROV hCryptProv,
	DWORD dwKeySpec,
	const ASN1::CAlgorithmIdentifierEx& signatureAlgorithm);
    /**
     * \brief Проверка подписи ответа
     * 
     * \param encodedCertificate [in] закодированное двоичное представление 
     * сертификата использующегося для проверки подписи
     *
     * \return true, если подпись проверена успешно, иначе false
     */
    bool verify( 
	const CBlob& encodedCertificate) const;

    // get (возврат указателя обозначает опциональный атрибут)
    /// возвращает версию ответа
    Version get_version() const;
    /// возвращает время создания ответа
    const CDateTime& get_producedAt() const;
    /// возвращает идентификатор ответчика
    const CResponderID& get_responderID() const;
    // signature и signatureAlgorithm заполняются вызовом sign()
    /**
     * \brief возвращает алгоритм подписи
     * \remark Заполняется вызовом sign()
     */
    const ASN1::CAlgorithmIdentifierEx* get_signatureAlgorithm() const;
    /**
     * \brief возвращает значение подписи
     * \remark Заполняется вызовом sign()
     */
    const CBlob* get_signature() const;
    /// возвращает список ответов по проверке на отзыв отдельных сертификатов
    const CSingleResponseList& get_responses() const;
    /// возвращает список сертификатов вложенных в ответ (опциональное поле)
    const ASN1::CEncodedCertificateList* get_certs() const;
    /// возвращает список расширений вложенный в ответ (опциональное поле)
    const ASN1::CExtensions* get_responseExtensions() const;
    // set (только опциональные аттрибуты, NULL убирает атрибут)
    /// устанавливает список ответов на отделные запросы на проверку отзыва сертификатов
    void put_responses( const CSingleResponseList& responses);
    /// устанавливает список сертификатов вкладываемых в ответ
    void put_certs( const ASN1::CEncodedCertificateList* certs);
    /// устанавливает список расширений вкладываемых в ответ
    void put_responseExtensions( const ASN1::CExtensions* extensions);
    /// установить количество знаков в дробной части секунд
    /// для полей thisUpdate, nextUpdate, producedAt
    void put_clockPrecisionDigits( int count );
private:
    void clear();
    class Impl;
    Impl* pImpl_;
};


/**
 * \brief ASN.1 структура CertStatus (RFC 2560, раздел 4.2.1)
 * 
 * Информация о статусе сертификата. Статут сертификата определяется
 * типом содержимого - в случае типа Good сертификат не отозван, в
 * случае Revoked - отозван, в случае Unknown - статус не определен.
 * В случае, если сертификат отозван, предоставляется время отзыва и
 * (опционально) причина отзыва.
 * 
 * \sa CSingleResponse
 */
class OCSP_CLASS CCertStatus
{
public:
    /// тип статуса сертификата
    enum Type 
    {
	/// сертификат не отозван
	Good,
	/// сертификат отозван
	Revoked, 
	/// статус сертификата неизвестен
	Unknown 
    };
    
    /**
     * \brief Создает пустой объект
     * \remark По умолчанию статус - Unknown
     */
    CCertStatus();
    /**
     * \brief Создает и инициализирует объект (статус Revoked)
     * \param revocationTime [in] время отзыва
     * \param revocationReason [in] причина отзыва (опционально)
     */
    CCertStatus(
	const CDateTime& revocationTime,
	const ASN1::CrlReason* revocationReason = 0);

    /**
     * \brief Создает копию заданного объекта.
     * \param src [in] объект, копия которого создается
     */
    CCertStatus( const CCertStatus& src);
    /**
     * \brief Копирует заданный объект в текущий.
     * \param src [in] объект, который копируется в текущий
     * \return Ссылка на текущий объект.
     */
    CCertStatus& operator=( const CCertStatus& src);

    /// Уничтожает объект
    ~CCertStatus();

    //get (возврат указателя обозначает опциональный атрибут)
    /// возвращает true если статус Good, иначе false
    bool isGood() const;
    /// возвращает true если статус Revoked, иначе false
    bool isRevoked() const;
    /// возвращает true если статус Unknown, иначе false
    bool isUnknown() const;
    /// возвращает статус
    Type get_type() const;
    /// если статус Revoked, возвращает время отзыва, иначе 0
    const CDateTime* get_revocationTime() const;
    /// если статус Revoked, возвращает время отзыва, иначе 0 (опциональное поле)
    const ASN1::CrlReason* get_revocationReason() const;
    //set (для каждого статуса свой набор параметров,
    // указатель обозначает опциональный параметр)
    /// устанавливает статус Good
    void put_good();
    /** 
     * \brief устанавливает статус Revoked
     * \param revocationTime [in] время отзыва
     * \param revocationReason [in] причина отзыва (опциональное поле)
     */
    void put_revoked(
	const CDateTime& revocationTime,
	const ASN1::CrlReason* revocationReason = 0);
    /// устанавливает статус Unknown
    void put_unknown();
private:
    void clear();
    class Impl;
    Impl* pImpl_;
};

/**
* \brief ASN.1 структура ResponderID (RFC 2560, раздел 4.2.1)
* 
* Идентификатор создателя ответа OCSP (службы). Служба может
* идентифицироватья по имени (ByName) или по SHA1 хэшу открытого
* ключа.
*
* \sa CBasicResponse
*/
class OCSP_CLASS CResponderID
{
public:
    /// тип идентификации ответчика - по имени или по хэшу открытого ключа
    enum Type 
    {
	/// идентификация по имени
	ByName,
	/// идентификация по хэшу открытого ключа
	ByHash 
    };

    /**
    * \brief Создает и инициализирует объект
    * \param type [in] тип идентификатора
    * \param value [in] закодированный идентификатор ответчика (содержание зависит от типа)
    */
    CResponderID( 
	Type type,
	const CBlob &value);

    /// Создает пустой объект
    CResponderID();

    /**
    * \brief Создает идентификатор на основе сертивиката
    * \param type [in] тип идентификатора
    * \param encodedCertificate [in] закодированный сертификат
    */
    static CResponderID fromCert( Type type,
	const CBlob &encodedCertificate );

    /// Уничтожает объект
    ~CResponderID();

    /**
    * \brief Создает копию заданного объекта.
    * \param src [in] объект, копия которого создается
    */
    CResponderID( const CResponderID& src);
    /**
    * \brief Копирует заданный объект в текущий.
    * \param src [in] объект, который копируется в текущий
    * \return Ссылка на текущий объект.
    */
    CResponderID& operator=( const CResponderID& src);

    /**
    * \brief Кодирует объект в формате ASN.1 DER и возвращает
    * получившуюся двоичную последовательность
    * \sa decode()
    */
    CBlob encode() const;
    /**
    * \brief Инициализирует объект из заданной закодированной двоичной последовательности
    * \sa encode()
    */
    void decode( const CBlob& encoded);

    /// возвращает тип идентификатора
    Type get_type() const;
    /// возвращает значение идентификатора
    const CBlob& get_value() const;

    /// устанавливает значение идентификатора
    void put_value( Type type, const CBlob& value);
private:
    Type type_;
    CBlob value_;
};

/// проверяет объекты на равенство
OCSP_API bool operator==( const CResponderID& lhs, const CResponderID& rhs);
/// проверяет объекты на неравенство
OCSP_API bool operator!=( const CResponderID& lhs, const CResponderID& rhs);

// расширения

// CRL Reference

class CCrlIDImpl;

/**
 * \brief Расширение CRL Reference (RFC 2560, раздел 4.4.2)
 * 
 * Содержит ссылку на CRL в котором был найден отозванный или
 * приостановленный CRL
 * 
 * \sa CExtValue, CExtension
 */
class OCSP_CLASS CExtCrlID : public ASN1::CExtValue
{
public:
    /// Создает пустой объект
    CExtCrlID();
    /**
     * \brief Создает и инициализирует объект
     * \param value [in] закодированное представление расширения
     */
    CExtCrlID( const CBlob& value);

    /// Уничтожает объект
    ~CExtCrlID();

    /**
     * \brief Создает копию заданного объекта.
     * \param src [in] объект, копия которого создается
     */
    CExtCrlID( const CExtCrlID& src);
    /**
     * \brief Копирует заданный объект в текущий.
     * \param src [in] объект, который копируется в текущий
     * \return Ссылка на текущий объект.
     */
    CExtCrlID& operator=( const CExtCrlID& src);

    //get
    /// возвращает поле crlUrl
    const char * get_crlUrl() const;
    /// возвращает поле crlNum
    const DWORD* get_crlNum() const;
    /// возвращает поле crlTime
    const CDateTime* get_crlTime() const;
    //set
    /// устанавливает поле crlUrl
    void put_crlUrl( const char * crlUrl);
    /// устанавливает поле crlNum
    void put_crlNum( const DWORD* crlNum);
    /// устанавливает поле crlTime
    void put_crlTime( const CDateTime* crlTime);
private:
    void clear();
    CCrlIDImpl* pImpl_;
};

/**
 * \brief Расширение Historical Request
 * 
 * Запрос состояния сертификата на определенный момент в прошлом.
 * 
 * \sa CExtValue, CExtension
 */
class OCSP_CLASS CExtHistoricalRequest : public ASN1::CExtValue
{
public:
    /// Создает пустой объект
    CExtHistoricalRequest();
    /**
     * \brief Создает и инициализирует объект
     * \param value [in] закодированное представление расширения
     */
    CExtHistoricalRequest( const CBlob& value);

    /// Уничтожает объект
    ~CExtHistoricalRequest();

    /**
     * \brief Создает копию заданного объекта.
     * \param src [in] объект, копия которого создается
     */
    CExtHistoricalRequest( const CExtHistoricalRequest& src);
    /**
     * \brief Копирует заданный объект в текущий.
     * \param src [in] объект, который копируется в текущий
     * \return Ссылка на текущий объект.
     */
    CExtHistoricalRequest& operator=( const CExtHistoricalRequest& src);

    //get
    /// возвращает поле crlUrl
    const char * get_crlUrl() const;
    /// возвращает поле crlNum
    const DWORD* get_crlNum() const;
    /// возвращает поле crlTime
    const CDateTime* get_crlTime() const;
    //set
    /// устанавливает поле crlUrl
    void put_crlUrl( const char * crlUrl);
    /// устанавливает поле crlNum
    void put_crlNum( const DWORD* crlNum);
    /// устанавливает поле crlTime
    void put_crlTime( const CDateTime* crlTime);
private:
    CCrlIDImpl* pImpl_;
};

// CRL Locator

/**
 * \brief Расширение CRL Locator (draft-ietf-pkix-ocspv2-ext-01.txt, раздел 6)
 * 
 * Ссылка на место, где лежит CRL по которому отпределялся статус сертификата.
 * 
 * \sa CExtValue, CExtension
 */
class OCSP_CLASS CExtCRLLocator: public ASN1::CExtValue
{
public:
    /// Создает пустой объект
    CExtCRLLocator();
    /**
     * \brief Создает и инициализирует объект
     * \param distributionPoints [in] список точек распространения CRL
     */
    CExtCRLLocator( 
	const ASN1::CCRLDistPointsSyntax& distributionPoints);
    /**
     * \brief Создает и инициализирует объект
     * \param value [in] закодированное представление расширения
     */
    CExtCRLLocator( const CBlob& value);

    /// Уничтожает объект
    ~CExtCRLLocator();

    /**
     * \brief Создает копию заданного объекта.
     * \param src [in] объект, копия которого создается
     */
    CExtCRLLocator( const CExtCRLLocator& src);
    /**
     * \brief Копирует заданный объект в текущий.
     * \param src [in] объект, который копируется в текущий
     * \return Ссылка на текущий объект.
     */
    CExtCRLLocator& operator=( const CExtCRLLocator& src);

    //get
    /// возвращает список точек распространения CRL
    const ASN1::CCRLDistPointsSyntax& get_distributionPoints() const;
    //set
    /// устанавливает список точек распространения CRL
    void put_distributionPoints( 
	const ASN1::CCRLDistPointsSyntax& distributionPoints);
private:
    ASN1::CCRLDistPointsSyntax distributionPoints_;
    void encode();
    void decode();
};

// Nonce

/**
 * \brief Расширение Nonce (RFC 2560, раздел 4.4.1)
 * 
 * Больщое случайное число для определения соответствия ответа запросу.
 * 
 * \sa CExtValue, CExtension
 */
class OCSP_CLASS CExtNonce: public ASN1::CExtValue
{
public:
    /// Создает пустой объект
    CExtNonce();
    /**
     * \brief Создает и инициализирует объект
     * \param value [in] закодированное представление расширения
     */
    CExtNonce( const CBlob& value);

    /// Уничтожает объект
    ~CExtNonce();

    /**
     * \brief Создает копию заданного объекта.
     * \param src [in] объект, копия которого создается
     */
    CExtNonce( const CExtNonce& src);
    /**
     * \brief Копирует заданный объект в текущий.
     * \param src [in] объект, который копируется в текущий
     * \return Ссылка на текущий объект.
     */
    CExtNonce& operator=( const CExtNonce& src);

    //get
    /// Возвращает значение числа.
    const CBlob& get_nonce() const;
    //set
    /// Устанавливает значение числа.
    void put_nonce( const CBlob& nonce);
};

// Instant revocation indicator

/**
 * \brief Расширение Instant Revocation Indicator
 * 
 * Расширение говорящее о том, что сертификат был отозван оператором
 * службы OCSP.
 * 
 * \sa CExtValue, CExtension
 */
class OCSP_CLASS CExtInstantRevocationIndicator: public ASN1::CExtValue
{
public:
    /// Создает пустой объект
    CExtInstantRevocationIndicator();
    /**
     * \brief Создает и инициализирует объект
     * \param value [in] закодированное представление расширения
     */
    CExtInstantRevocationIndicator( const CBlob& value);

    /// Уничтожает объект
    ~CExtInstantRevocationIndicator();

    /**
     * \brief Создает копию заданного объекта.
     * \param src [in] объект, копия которого создается
     */
    CExtInstantRevocationIndicator( 
	const CExtInstantRevocationIndicator& src);
    /**
     * \brief Копирует заданный объект в текущий.
     * \param src [in] объект, который копируется в текущий
     * \return Ссылка на текущий объект.
     */
    CExtInstantRevocationIndicator& operator=(
	const CExtInstantRevocationIndicator& src);
};

// Revocation announcement reference

/**
 * \brief Расширение Revocation Announcement Reference
 * 
 * Анонс отзыва, который использовался 
 * при определении статуса сертификата.
 * 
 * \sa CExtValue, CExtension
 */
class OCSP_CLASS CExtRevocationAnnouncementReference: public ASN1::CExtValue
{
public:
    /// Создает пустой объект
    CExtRevocationAnnouncementReference();
    /**
     * \brief Создает и инициализирует объект
     * \param value [in] закодированное представление расширения
     */
    CExtRevocationAnnouncementReference( const CBlob& value);

    /// Уничтожает объект
    ~CExtRevocationAnnouncementReference();

    /**
     * \brief Создает копию заданного объекта.
     * \param src [in] объект, копия которого создается
     */
    CExtRevocationAnnouncementReference( 
	const CExtRevocationAnnouncementReference& src);
    /**
     * \brief Копирует заданный объект в текущий.
     * \param src [in] объект, который копируется в текущий
     * \return Ссылка на текущий объект.
     */
    CExtRevocationAnnouncementReference& operator=(
	const CExtRevocationAnnouncementReference& src);
};

// Service locator

class CExtServiceLocatorImpl;
/**
 * \brief Расширение Service Locator (RFC 2560, раздел 4.4.6)
 * 
 * Указывает на службу, которая может определить
 * статус сертификата.
 * 
 * \sa CExtValue, CExtension
 */
class OCSP_CLASS CExtServiceLocator: public ASN1::CExtValue
{
public:
    /// Создает пустой объект
    CExtServiceLocator();
    /**
     * \brief Создает и инициализирует объект
     * \param value [in] закодированное представление расширения
     */
    CExtServiceLocator( const CBlob& value);

    /// Уничтожает объект
    ~CExtServiceLocator();

    /**
     * \brief Создает копию заданного объекта.
     * \param src [in] объект, копия которого создается
     */
    CExtServiceLocator( const CExtServiceLocator& src);
    /**
     * \brief Копирует заданный объект в текущий.
     * \param src [in] объект, который копируется в текущий
     * \return Ссылка на текущий объект.
     */
    CExtServiceLocator& operator=( const CExtServiceLocator& src);

    //get
    /// возвращает поле issuer
    const CBlob& get_issuer() const;
    /// возвращает поле locator (опциональное поле)
    const ASN1::CAuthorityInfoAccessSyntax* get_locator() const;
    //set
    /// устанавливает поле issuer
    void put_issuer( const CBlob& issuer);
    /// устанавливает поле locator (опциональное поле)
    void put_locator( const ASN1::CAuthorityInfoAccessSyntax* locator);
private:
    CExtServiceLocatorImpl *pImpl_;
    void encode();
    void decode();
};

// Acceptable responses

/**
 * \brief Список OID'ов допустимых типов ответов (RFC 2560, раздел 4.4.3)
 * 
 * Расширением данного класса является класс CExtAcceptableResponses.
 */
typedef CStringList CAcceptableResponses;

/**
 * \brief Расширение Acceptable Responses (RFC 2560, раздел 4.4.3)
 * 
 * Список OID'ов типов ответов которые понимает клиент.
 * 
 * \sa CExtValue, CExtension
 */
class OCSP_CLASS CExtAcceptableResponses: public ASN1::CExtValue
{
public:
    /// Создает пустой объект
    CExtAcceptableResponses();
    /**
     * \brief Создает и инициализирует объект
     * \param acceptableResponses [in] список OID'ов допустимых ответов
     */
    CExtAcceptableResponses( 
	const CAcceptableResponses& acceptableResponses);
    /**
     * \brief Создает и инициализирует объект
     * \param value [in] закодированное представление расширения
     */
    CExtAcceptableResponses( const CBlob& value);

    /// Уничтожает объект
    ~CExtAcceptableResponses();

    /**
     * \brief Создает копию заданного объекта.
     * \param src [in] объект, копия которого создается
     */
    CExtAcceptableResponses( const CExtAcceptableResponses& src);
    /**
     * \brief Копирует заданный объект в текущий.
     * \param src [in] объект, который копируется в текущий
     * \return Ссылка на текущий объект.
     */
    CExtAcceptableResponses& operator=( const CExtAcceptableResponses& src);

    //get
    /// возвращает список OID'ов допустимых ответов
    const CAcceptableResponses& get_acceptableResponses() const;
    //set
    /// устанавливает список OID'ов допустимых ответов
    void put_acceptableResponses( 
	const CAcceptableResponses& acceptableResponses);
private:
    CAcceptableResponses acceptableResponses_;
    void encode();
    void decode();
};

// Archive Cutoff

/**
 * \brief Расширение Archive Cutoff (RFC 2560, раздел 4.4.4)
 * 
 * Время, до которого хранится архив службы OCSP
 * 
 * \sa CExtValue, CExtension
 */
class OCSP_CLASS CExtArchiveCutoff: public ASN1::CExtValue
{
public:
    /// Создает пустой объект
    CExtArchiveCutoff();
    /**
     * \brief Создает и инициализирует объект
     * \param archiveCutoff [in] время до которого хранится архив службы OCSP
     */
    CExtArchiveCutoff( 
	const CDateTime& archiveCutoff);
    /**
     * \brief Создает и инициализирует объект
     * \param value [in] закодированное представление расширения
     */
    CExtArchiveCutoff( const CBlob& value);

    /// Уничтожает объект
    ~CExtArchiveCutoff();

    /**
     * \brief Создает копию заданного объекта.
     * \param src [in] объект, копия которого создается
     */
    CExtArchiveCutoff( const CExtArchiveCutoff& src);
    /**
     * \brief Копирует заданный объект в текущий.
     * \param src [in] объект, который копируется в текущий
     * \return Ссылка на текущий объект.
     */
    CExtArchiveCutoff& operator=( const CExtArchiveCutoff& src);

    //get
    /// возвращает время до которого хранится архив службы OCSP
    const CDateTime& get_archiveCutoff() const;
    //set
    /// устанавливает время до которого хранится архив службы OCSP
    void put_archiveCutoff( 
	const CDateTime& archiveCutoff);
private:
    CDateTime archiveCutoff_;
    void encode();
    void decode();
};

} // namespace OCSP
} // namespace PKI
} // namespace CryptoPro

#endif // _OCSP_H_INCLUDED
