package com.a2soft.crypto_pro_flutter

import android.content.Context
import android.util.Base64
import com.a2soft.crypto_pro_flutter.exceptions.NoPrivateKeyFound
import org.json.JSONException
import org.json.JSONObject
import ru.CryptoPro.AdES.AdESConfig
import ru.CryptoPro.JCSP.CSPConfig
import ru.CryptoPro.JCSP.JCSP
import ru.CryptoPro.reprov.RevCheck
import java.io.FileInputStream
import java.io.InputStream
import java.security.KeyStore
import java.security.KeyStoreException
import java.security.Security
import java.security.cert.X509Certificate

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

    /** Установка PFX-сертификата в хранилище */
    fun addPfxCertificate(path: String, password: String): JSONObject {
        try {
            val keyStore = KeyStore.getInstance(JCSP.PFX_STORE_NAME, JCSP.PROVIDER_NAME)
            val fileInputStream: InputStream = FileInputStream(path)
            keyStore.load(fileInputStream, password.toCharArray())
            val alias: String = findPrivateKeyAlias(keyStore)
            val certificate = keyStore.getCertificate(alias) as X509Certificate


            val response = JSONObject()
            response.put("success", true)
            response.put("certificate", Base64.encodeToString(certificate.encoded, Base64.NO_WRAP))
            response.put("alias", alias)
            return response
        } catch (e: NoPrivateKeyFound) {
            return getErrorResponse("Не найден приватный ключ, связанный с сертификатом", e)
        } catch (e: Exception) {
            return getErrorResponse("Произошла непредвиденная ошибка", e)
        }
    }

    /** Поиск приватного ключа сертификата в хранилище */
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