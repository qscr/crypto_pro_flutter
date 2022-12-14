package com.a2soft.crypto_pro_flutter

import android.content.Context
import com.a2soft.crypto_pro_flutter.exceptions.NoPrivateKeyFound
import org.json.JSONArray
import org.json.JSONException
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
import ru.CryptoPro.JCSP.support.BKSTrustStore
import ru.CryptoPro.reprov.RevCheck
import ru.CryptoPro.ssl.util.cpSSLConfig
import java.io.*
import java.security.KeyStore
import java.security.KeyStoreException
import java.security.PrivateKey
import java.security.Security
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
            System.setProperty("com.sun.security.crl.timeout", "10")
            System.setProperty("ru.CryptoPro.crl.read_timeout", "10")


            // Задание провайдера по умолчанию для CAdES.
            AdESConfig.setDefaultProvider(JCSP.PROVIDER_NAME)
            // https://cryptopro.ru/forum2/default.aspx?g=posts&t=20779
            cpSSLConfig.setDefaultSSLProvider(JCSP.PROVIDER_NAME)

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
            System.setProperty("ru.CryptoPro.reprov.enableCRLDP", "true")
            System.setProperty("com.sun.security.enableCRLDP", "true")
            System.setProperty("com.ibm.security.enableCRLDP", "true")
            System.setProperty("ru.CryptoPro.reprov.enableAIAcaIssuers", "true")
            System.setProperty("com.sun.security.enableAIAcaIssuers", "true")
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
    fun signFile(filePathToSign: String, alias: String, password: String, detached: Boolean, disableOnlineValidation: Boolean) : JSONObject {
        try {
            val fileInputStream = FileInputStream(filePathToSign)
            val size = fileInputStream.available()
            val buffer = ByteArray(size)
            fileInputStream.read(buffer)
            fileInputStream.close()

            return sign(buffer, alias, password, detached, false, disableOnlineValidation)
        } catch (e: NoPrivateKeyFound) {
            return getErrorResponse("Не найден приватный ключ, связанный с сертификатом", e)
        } catch (e: Exception) {
            return getErrorResponse("Произошла непредвиденная ошибка", e)
        }
    }

    /** Подписать сообщение */
    fun signMessage(contentToSign: String, alias: String, password: String, detached: Boolean, signHash: Boolean, disableOnlineValidation: Boolean) : JSONObject {
        return sign(contentToSign.toByteArray(), alias, password, detached, signHash, disableOnlineValidation)
    }

    /** Подписание массива байт */
    private fun sign(contentToSign: ByteArray, alias: String, password: String, detached: Boolean, signHash: Boolean, disableOnlineValidation: Boolean) : JSONObject {
        try {
            // Получаем из HDImage сертификат (которым будем подписывать) с приватным ключем
            val keyStore = KeyStore.getInstance(JCSP.HD_STORE_NAME, JCSP.PROVIDER_NAME)
            keyStore.load(null, null)
            val certificate = keyStore.getCertificate(alias) as X509Certificate
            val privateKey = keyStore.getKey(alias, password.toCharArray()) as PrivateKey

            // Формируем цепочку для подписи
            val chain: MutableList<X509Certificate> = ArrayList()
            chain.add(certificate)

            val cAdESSignature = CAdESSignature(detached, signHash)
            if (disableOnlineValidation) {
                cAdESSignature.setOptions((Options()).disableCertificateValidation());
            }

            var exception: Exception? = null;


            val gfgThread = Thread {
                try {
                    cAdESSignature.addSigner(
                        JCSP.PROVIDER_NAME, null, null, privateKey, chain,
                        CAdESType.CAdES_BES, null, false, null, null, null,
                        true
                    )
                } catch (e: Exception) {
                    exception = e
                }
            }

            gfgThread.start()
            gfgThread.join();

            if (exception != null) {
                return getErrorResponse(exception.toString(), exception!!)
            }

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
        } catch (e: Exception) {
            return getErrorResponse("Произошла непредвиденная ошибка", e)
        }
    }


    /** Установка PFX-сертификата */
    fun addPfxCertificate(path: String, password: String, context: Context): JSONObject {
        try {
            // Загружаем pfx-файл в Pfx-KeyStore
            val keyStore = KeyStore.getInstance(JCSP.PFX_STORE_NAME, JCSP.PROVIDER_NAME)
            val fileInputStream: InputStream = FileInputStream(path)
            keyStore.load(null,null)
            keyStore.load(fileInputStream, password.toCharArray())
            // Получаем алиас сертификата с приватным ключем
            val mainCertAlias: String = findPrivateKeyAlias(keyStore)
            val certificate = keyStore.getCertificate(mainCertAlias) as X509Certificate
            val privateKey = keyStore.getKey(mainCertAlias, password.toCharArray()) as PrivateKey
            val chain = keyStore.getCertificateChain(mainCertAlias)
            // Загружаем цепочку в HDImage
            val hdKeyStore = KeyStore.getInstance(JCSP.HD_STORE_NAME, JCSP.PROVIDER_NAME)
            hdKeyStore.load(null, null)
            if (!hdKeyStore.containsAlias(mainCertAlias)) {
                val keyEntry = JCPPrivateKeyEntry(privateKey, chain, true)
                val parameter = JCPProtectionParameter(null)
                hdKeyStore.setEntry(mainCertAlias, keyEntry as KeyStore.Entry, parameter as KeyStore.PasswordProtection)
            }

            // Добавляем остальные сертификаты из pfx-файла в хранилище доверенных
            for (alias in keyStore.aliases().toList()) {
                if (alias != mainCertAlias && !containsAliasInBks(alias)) {
                    addToBksTrustStore(alias, keyStore.getCertificate(alias) as X509Certificate)
                }
            }



            val response = JSONObject()
            response.put("success", true)
            response.put("certificate", getJSONCertificate(mainCertAlias, certificate))
            return response
        } catch (e: NoPrivateKeyFound) {
            return getErrorResponse("Не найден приватный ключ, связанный с сертификатом", e)
        } catch (e: Exception) {
            return getErrorResponse("Произошла непредвиденная ошибка", e)
        }
    }

    /** Удаление PFX-сертификата */
    fun deletePfxCertificate(alias: String): JSONObject {
        try {
            val keyStore = KeyStore.getInstance(JCSP.HD_STORE_NAME, JCSP.PROVIDER_NAME)
            keyStore.load(null, null)
            keyStore.deleteEntry(alias)

            val response = JSONObject()
            response.put("success", true)
            return response
        } catch (e: NoPrivateKeyFound) {
            return getErrorResponse("Не найден приватный ключ, связанный с сертификатом", e)
        } catch (e: Exception) {
            return getErrorResponse("Произошла непредвиденная ошибка", e)
        }
    }

    /** Получение установленных сертификатов */
    fun getInstalledCertificates(): JSONObject {
        try {
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
        } catch (e: Exception) {
            return getErrorResponse("Произошла непредвиденная ошибка", e)
        }
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

    @Throws(JSONException::class)
    private fun getErrorResponse(message: String, e: Exception): JSONObject {
        val response = JSONObject()
        response.put("success", false)
        response.put("message", message)
        response.put("exception", e)
        return response
    }
}