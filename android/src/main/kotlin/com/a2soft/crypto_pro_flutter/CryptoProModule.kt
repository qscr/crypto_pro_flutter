package com.a2soft.crypto_pro_flutter

import android.content.Context
import com.a2soft.crypto_pro_flutter.exceptions.CustomWrongPasswordException
import com.a2soft.crypto_pro_flutter.exceptions.NoPrivateKeyFound
import com.a2soft.crypto_pro_flutter.exceptions.SomeCertificatesAreNotAddedToTrustStoreException
import org.json.JSONArray
import org.json.JSONObject
import ru.CryptoPro.AdES.AdESConfig
import ru.CryptoPro.AdES.Options
import ru.CryptoPro.CAdES.CAdESSignature
import ru.CryptoPro.CAdES.CAdESType
import ru.CryptoPro.JCP.KeyStore.JCPPrivateKeyEntry
import ru.CryptoPro.JCP.params.JCPProtectionParameter
import ru.CryptoPro.JCP.tools.Encoder
import ru.CryptoPro.JCSP.CSPConfig
import ru.CryptoPro.JCSP.JCSP
import ru.CryptoPro.JCSP.exception.WrongPasswordException
import ru.CryptoPro.JCSP.support.BKSTrustStore
import ru.CryptoPro.reprov.RevCheck
import java.io.*
import java.security.KeyStore
import java.security.KeyStore.PasswordProtection
import java.security.KeyStore.ProtectionParameter
import java.security.KeyStoreException
import java.security.Security
import java.security.cert.CertificateFactory
import java.security.cert.X509Certificate

/** Модуль для работы с Crypto Pro */
class CryptoProModule {
    companion object Factory {
        private var instance: CryptoProModule? = null

        fun getInstance(): CryptoProModule {
            if (instance == null) instance = CryptoProModule()
            return instance!!
        }
    }

    /** Инициализация провайдера */
    fun initCSP(context: Context): Boolean {
        val initCode = CSPConfig.init(context)
        val initOk = initCode == CSPConfig.CSP_INIT_OK
        if (initOk) {
            if (Security.getProvider(JCSP.PROVIDER_NAME) == null) {
                Security.addProvider(JCSP())
            }
            if (Security.getProvider(RevCheck.PROVIDER_NAME) == null) {
                Security.addProvider(RevCheck())
            }
            //
            // Отключаем проверку цепочки штампа времени (CAdES-T),
            // чтобы не требовать него CRL.
            //
            System.setProperty("ru.CryptoPro.CAdES.validate_tsp", "false")

            // Таймауты для CRL на всякий случай.
            System.setProperty("com.sun.security.crl.timeout", "20")
            System.setProperty("ru.CryptoPro.crl.read_timeout", "20")


            // Задание провайдера по умолчанию для CAdES.
            AdESConfig.setDefaultProvider(JCSP.PROVIDER_NAME)

            // Включаем возможность онлайновой проверки статуса
            // сертификата.
            //
            // В случае создания подписей формата BES или T можно отключить
            // проверку цепочки сертификатов подписанта (и службы) с помощью
            // параметра:
            // cAdESSignature.setOptions((new Options()).disableCertificateValidation()); // CAdES
            // или
            // xAdESSignature.setOptions((new Options()).disableCertificateValidation()); // XAdES
            // перед добавлением подписанта.
            // По умолчанию проверка цепочки сертификатов подписанта всегда
            // включена.
            System.setProperty("com.sun.security.enableCRLDP", "true")
            System.setProperty("com.ibm.security.enableCRLDP", "true")
            System.setProperty("com.sun.security.enableAIAcaIssuers", "true")
            System.setProperty("ru.CryptoPro.reprov.enableAIAcaIssuers", "true")
        }

        return initOk
    }

    /** Получить файл хранилища доверенных сертификатов */
    private fun getBksTrustStore(): File {
        val trustStorePath = CSPConfig.getBksTrustStore() + File.separator +
                BKSTrustStore.STORAGE_FILE_TRUST

        val trustStoreFile = File(trustStorePath)
        if (!trustStoreFile.exists()) {
            throw Exception(
                "Trust store " + trustStorePath +
                        " doesn't exist"
            )
        } else {
            return trustStoreFile
        }
    }

    /** Проверить, есть ли сертификат в хранилище корневых сертификатов */
    private fun containsAliasInBks(alias: String): Boolean {
        val trustStoreFile = getBksTrustStore()
        val trustStoreStream = trustStoreFile.inputStream();
        val keyStore = KeyStore.getInstance(BKSTrustStore.STORAGE_TYPE)
        keyStore.load(
            trustStoreStream,
            BKSTrustStore.STORAGE_PASSWORD,
        )
        return keyStore.containsAlias(alias);
    }

    private fun listAllCertificatesInBks() {
        val trustStoreFile = getBksTrustStore()
        val trustStoreStream = trustStoreFile.inputStream();
        val keyStore = KeyStore.getInstance(BKSTrustStore.STORAGE_TYPE)
        keyStore.load(
            trustStoreStream,
            BKSTrustStore.STORAGE_PASSWORD,
        )
        val list = keyStore.aliases().toList()
        println(list)
    }

    /** Добавить сертификат в хранилище доверенных сертификатов */
    private fun addToBksTrustStore(alias: String, certificate: X509Certificate) {
        val trustStoreFile = getBksTrustStore()
        val trustStoreStream = trustStoreFile.inputStream();
        val keyStore = KeyStore.getInstance(BKSTrustStore.STORAGE_TYPE)
        keyStore.load(
            trustStoreStream,
            BKSTrustStore.STORAGE_PASSWORD,
        )
        keyStore.setCertificateEntry(alias, certificate)
        val updatedTrustStore = FileOutputStream(trustStoreFile)
        keyStore.store(
            updatedTrustStore,
            BKSTrustStore.STORAGE_PASSWORD,
        )
        trustStoreStream.close()
    }

    /** Подписать файл */
    fun signFile(
        filePathToSign: String,
        alias: String,
        password: String,
        detached: Boolean,
        disableOnlineValidation: Boolean,
        format: CAdESFormat,
        storageName: String? = null,
        tsaUrl: String? = null // URL TSA (опционально для CAdES-X Long Type 1)
    ) : JSONObject {
        val fileInputStream = FileInputStream(filePathToSign)
        val size = fileInputStream.available()
        val buffer = ByteArray(size)
        fileInputStream.read(buffer)
        fileInputStream.close()

        return sign(buffer, alias, password, detached, false, disableOnlineValidation, format, storageName, tsaUrl)
    }

    /** Подписать сообщение */
    fun signMessage(
        contentToSign: String,
        alias: String,
        password: String,
        detached: Boolean,
        signHash: Boolean,
        disableOnlineValidation: Boolean,
        format: CAdESFormat,
        storageName: String? = null,
        tsaUrl: String? = null // URL TSA (опционально для CAdES-X Long Type 1)
    ) : JSONObject {
        return sign(contentToSign.toByteArray(), alias, password, detached, signHash, disableOnlineValidation, format, storageName, tsaUrl)
    }

    /** Подписание массива байт */
    private fun sign(
        contentToSign: ByteArray,
        alias: String,
        password: String,
        detached: Boolean,
        signHash: Boolean,
        disableOnlineValidation: Boolean,
        format: CAdESFormat,
        storageName: String? = null,
        tsaUrl: String? = null // URL TSA (опционально для CAdES-X Long Type 1)
    ) : JSONObject {
        // Получаем из HDImage (или из storageName, если передан) сертификат (которым будем подписывать) с приватным ключем
        val keyStore = KeyStore.getInstance(storageName ?: JCSP.HD_STORE_NAME, JCSP.PROVIDER_NAME)
        keyStore.load(null, null)
        val protectionParam = JCPProtectionParameter(password.toCharArray())
        val privateKeyEntry = keyStore.getEntry(alias, protectionParam as ProtectionParameter) as JCPPrivateKeyEntry
        val certificateChain = privateKeyEntry.certificateChain
        val privateKey = privateKeyEntry.privateKey

        // Формируем цепочку для подписи
        val chain: MutableList<X509Certificate> = ArrayList()
        for (cert in certificateChain) {
            chain.add(cert as X509Certificate)
        }

        val cAdESSignature = CAdESSignature(detached, signHash)
        if (disableOnlineValidation) {
            cAdESSignature.setOptions((Options()).disableCertificateValidation());
        }

        // Настраиваем параметры для подписания в зависимости от формата
        val tsaSeverUrl = if (format == CAdESFormat.XLongType1 && tsaUrl != null) tsaUrl else null
        val cadesType = when (format) {
            CAdESFormat.BES -> CAdESType.CAdES_BES
            CAdESFormat.XLongType1 -> CAdESType.CAdES_X_Long_Type_1
        }

        cAdESSignature.addSigner(
            JCSP.PROVIDER_NAME, null, null, privateKey, chain,
            cadesType, tsaSeverUrl, false, null, null
        )

        val signatureStream = ByteArrayOutputStream()

        cAdESSignature.open(signatureStream)
        cAdESSignature.update(contentToSign)

        cAdESSignature.close()
        signatureStream.close()

        val enc = Encoder()
        val base64 = enc.encode(signatureStream.toByteArray())

        val response = JSONObject()
        response.put("success", true)
        response.put("signBase64", base64)
        return response
    }

    /** Установка PFX-контейнера */
    fun addPfxCertificate(path: String, password: String, newPassword: String?): JSONObject {
        val keyStore = KeyStore.getInstance(JCSP.PFX_STORE_NAME, JCSP.PROVIDER_NAME)
        val fileInputStream: InputStream = FileInputStream(path)
        keyStore.load(null,null)
        keyStore.load(fileInputStream, password.toCharArray())
        return addContainerFromStorage(keyStore, password, newPassword)
    }

    /** Получение внешнего контейнера по имени */
    fun readFromExternalStorage(name: String, password: String): JSONObject {
        val keyStore = KeyStore.getInstance(name, JCSP.PROVIDER_NAME)
        keyStore.load(null,null)
        return readContainerFromStorage(keyStore, password)
    }

    /** Получение приватного ключа сертификата по имени из внутреннего хранилища */
    fun getPrivateKeyFromInternalContainerByAlias(alias: String, password: String): JSONObject {
        val keyStore = KeyStore.getInstance(JCSP.HD_STORE_NAME, JCSP.PROVIDER_NAME)
        keyStore.load(null,null)
        return readContainerFromStorage(keyStore, password)
    }

    /** Удаление сертификата и приватного ключа из внутреннего хранилища */
    fun removeCertificateWithPrivateKeyFromStorage(alias: String): JSONObject {
        val keyStore = KeyStore.getInstance(JCSP.HD_STORE_NAME, JCSP.PROVIDER_NAME)
        keyStore.load(null,null)
        keyStore.deleteEntry(alias)

        val response = JSONObject()
        response.put("success", true)
        return response
    }

    /** Получение сертификата с приватным ключом из контейнера */
    private fun readContainerFromStorage(storage: KeyStore, password: String): JSONObject {
        try {
            // Получаем алиас сертификата с приватным ключем
            val mainCertAlias: String = findPrivateKeyAlias(storage)
            val protectionParam = PasswordProtection(password.toCharArray())
            val entry = storage.getEntry(mainCertAlias, protectionParam) as JCPPrivateKeyEntry
            val certificate = entry.certificate as X509Certificate

            val response = JSONObject()
            response.put("success", true)
            response.put("certificate", getJSONCertificate(mainCertAlias, certificate))
            return response
        } catch (e: WrongPasswordException) {
            throw CustomWrongPasswordException()
        }
    }

    /** Добавление внешнего контейнера по имени */
    fun addFromExternalStorage(name: String, password: String, newPassword: String?): JSONObject {
        val keyStore = KeyStore.getInstance(name, JCSP.PROVIDER_NAME)
        keyStore.load(null,null)
        return addContainerFromStorage(keyStore, password, newPassword)
    }

    /** Добавление контейнера в хранилище */
    private fun addContainerFromStorage(storage: KeyStore, password: String, newPassword: String?): JSONObject {
        try {
            // Получаем алиас сертификата с приватным ключем
            val mainCertAlias: String = findPrivateKeyAlias(storage)
            val protectionParam = PasswordProtection(password.toCharArray())
            val entry = storage.getEntry(mainCertAlias, protectionParam) as JCPPrivateKeyEntry
            val certificate = entry.certificate as X509Certificate
            // Загружаем цепочку в HDImage
            val hdKeyStore = KeyStore.getInstance(JCSP.HD_STORE_NAME, JCSP.PROVIDER_NAME)
            hdKeyStore.load(null, null)
            if (!hdKeyStore.containsAlias(mainCertAlias)) {
                val newProtectionParam = PasswordProtection((newPassword ?: password).toCharArray())
                hdKeyStore.setEntry(mainCertAlias, entry, newProtectionParam)
            }

            // Добавляем остальные сертификаты в хранилище доверенных
            for (cert in entry.certificateChain) {
                if (cert is X509Certificate && cert.serialNumber != certificate.serialNumber) {
                    addToBksTrustStore(cert.serialNumber.toString(), cert)
                }
            }

            // Добавляем остальные сертификаты в хранилище доверенных
            for (alias in storage.aliases().toList()) {
                if (alias != mainCertAlias && !containsAliasInBks(alias)) {
                    addToBksTrustStore(alias, storage.getCertificate(alias) as X509Certificate)
                }
            }

            val response = JSONObject()
            response.put("success", true)
            response.put("certificate", getJSONCertificate(mainCertAlias, certificate))
            return response
        } catch (e: WrongPasswordException) {
            throw CustomWrongPasswordException()
        }
    }

    /** Добавление списка сертификатов в хранилище доверенных */
    fun addCertificatesToTrustedStorage(paths: List<String>) {
        val errorCertificates = mutableListOf<String>()
        if (paths.isEmpty()) {
            return
        }

        val certificateFactory = CertificateFactory.getInstance("X.509")
        for (path in paths) {
            try {
                val certificateInputStream = FileInputStream(path)
                val certificate = certificateFactory.generateCertificate(certificateInputStream) as X509Certificate
                addToBksTrustStore(certificate.serialNumber.toString(), certificate)
            } catch (e: Exception) {
                errorCertificates.add(path)
            }
        }

        if (errorCertificates.isNotEmpty()) {
            throw SomeCertificatesAreNotAddedToTrustStoreException(errorCertificates.toTypedArray())
        }
    }

    /** Удаление сертификата из внутреннего хранилища */
    fun deleteCertificateFromInternalStorage(alias: String): JSONObject {
        val keyStore = KeyStore.getInstance(JCSP.HD_STORE_NAME, JCSP.PROVIDER_NAME)
        keyStore.load(null, null)
        keyStore.deleteEntry(alias)

        val response = JSONObject()
        response.put("success", true)
        return response
    }

    /** Получение установленных сертификатов */
    fun getInstalledCertificates(): JSONObject {
        val keyStore = KeyStore.getInstance(JCSP.HD_STORE_NAME, JCSP.PROVIDER_NAME)
        keyStore.load(null, null)
        val aliases = keyStore.aliases().toList().filterNot { it.contains("root_csp") }
        val certificatesJSON = ArrayList<JSONObject>()

        for (alias in aliases) {
            val certificate = keyStore.getCertificate(alias) as X509Certificate
            val certificateJSON = getJSONCertificate(alias, certificate)
            certificatesJSON.add(certificateJSON)
        }

        val response = JSONObject()
        response.put("success", true)
        response.put("certificates", JSONArray(certificatesJSON))
        return response
    }

    /** Получаем JSON-модель по сертификату и алиасу */
    private fun getJSONCertificate(alias: String, certificate: X509Certificate): JSONObject {
        val certificateJSON = JSONObject()

        certificateJSON.put("alias", alias)
        certificateJSON.put("owner", certificate.subjectDN.name)
        certificateJSON.put("issuer", certificate.issuerDN.name)
        certificateJSON.put("serialNumber", certificate.serialNumber.toString())
        certificateJSON.put("algorithm", certificate.sigAlgName)
        certificateJSON.put("version", certificate.version.toString())
        certificateJSON.put("oid", certificate.sigAlgOID)
        certificateJSON.put("validFrom", certificate.notBefore.toString())
        certificateJSON.put("validTo", certificate.notAfter.toString())

        return certificateJSON;
    }

    /** Поиск алиаса для сертификата с приватным ключем */
    @Throws(KeyStoreException::class, NoPrivateKeyFound::class)
    private fun findPrivateKeyAlias(keyStore: KeyStore): String {
        val aliases = keyStore.aliases()
        while (aliases.hasMoreElements()) {
            val alias = aliases.nextElement()
            if (keyStore.isKeyEntry(alias)) return alias
        }
        throw NoPrivateKeyFound()
    }
}
