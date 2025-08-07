package com.a2soft.crypto_pro_flutter.exceptions

import org.json.JSONArray
import ru.CryptoPro.AdES.exception.AdESException
import ru.CryptoPro.CAdES.exception.CAdESException

open class CryptoProFlutterBaseException(override val message: String, val code: String, override val cause: Throwable?,): Exception()

class NoPrivateKeyFound: CryptoProFlutterBaseException("Приватный ключ, связанный с сертификатом, не найден", "1", null)

class CustomWrongPasswordException: CryptoProFlutterBaseException("Указан неправильный пароль от контейнера", "2", null)

class ArgumentsParsingException: CryptoProFlutterBaseException("Обязательные аргументы не переданы или переданы с ошибкой", "3", null)

class SomeCertificatesAreNotAddedToTrustStoreException(errorCertificateNames: Array<String>): CryptoProFlutterBaseException("Не удалось добавить следующие сертификаты: " + errorCertificateNames.joinToString(", "), "4", null)

class AddSignerCertificateStatusUnknownOrRevokedException(exception: CAdESException): BaseCustomCadesException("5", exception, "Ошибка ${exception.errorCode}. Не удалось проверить сертификат на отзыв", null)

class GetCertificateFromContainerException(container: String, message: String): CryptoProFlutterBaseException("Не удалось получить сертификат из контейнера $container \n$message", "6", null)

class GetCertificatePrivateKeyException(message: String): CryptoProFlutterBaseException("Не удалось получить приватный ключ из контейнера \n$message", "7", null)

class ReadSignatureFromStreamException(message: String): CryptoProFlutterBaseException("Не удалось записать подпись из потока \n$message", "8", null)

class AddSignerUnknownException(exception: CAdESException): BaseCustomCadesException("9", exception, "Неизвестная ошибка с кодом ${exception.errorCode}", null)

open class BaseCustomCadesException(val code: String, private val exception: CAdESException, private val customMessage: String?, private val jsonChain: JSONArray?): Exception() {

    override fun toString(): String {
        val fullMap = linkedMapOf<String, Any?>() // Сохраняем порядок

        // Если есть кастомное сообщение — оно идёт первым
        if (customMessage != null) {
            fullMap["customMessage"] = customMessage
        }

        var current: Throwable? = exception
        var level = 0

        while (current != null) {
            val levelMap = mutableMapOf<String, Any?>()

            levelMap["exceptionClass"] = current::class.java.name
            levelMap["message"] = current.message
            levelMap["stackTrace"] = current.stackTrace?.joinToString("\n") { it.toString() }

            if (current is AdESException) {
                val errorCodes = current.errorCodeList.toList()
                levelMap["errorCodes"] = errorCodes
                levelMap["errorDescriptions"] = errorCodes.associateWith {
                    AdESException.getErrorCodeName(it)
                }
            }

            fullMap["exception_level_$level"] = levelMap

            current = current.cause
            level++
        }

        if (jsonChain != null) {
            fullMap["certificateChain"] = jsonChain.join("\n\n")
        }

        return fullMap.entries.joinToString("\n\n") { (key, value) ->
            when (value) {
                is Map<*, *> -> {
                    val content = value.entries.joinToString("\n") { (k, v) -> "$k: $v" }
                    "$key:\n$content"
                }
                else -> "$key: $value"
            }
        }
    }
}