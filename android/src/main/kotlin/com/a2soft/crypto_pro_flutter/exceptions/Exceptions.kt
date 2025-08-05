package com.a2soft.crypto_pro_flutter.exceptions

open class CryptoProFlutterBaseException(override val message: String, val code: String, override val cause: Throwable?,): Exception()

class NoPrivateKeyFound: CryptoProFlutterBaseException("Приватный ключ, связанный с сертификатом, не найден", "1", null)

class CustomWrongPasswordException: CryptoProFlutterBaseException("Указан неправильный пароль от контейнера", "2", null)

class ArgumentsParsingException: CryptoProFlutterBaseException("Обязательные аргументы не переданы или переданы с ошибкой", "3", null)

class SomeCertificatesAreNotAddedToTrustStoreException(errorCertificateNames: Array<String>): CryptoProFlutterBaseException("Не удалось добавить следующие сертификаты: " + errorCertificateNames.joinToString(", "), "4", null)

class AddSignerCertificateStatusUnknownOrRevokedException(cause: Throwable, errorCode: Int): CryptoProFlutterBaseException("Ошибка $errorCode. Не удалось проверить сертификат на отзыв", "5", cause)

class GetCertificateFromContainerException(container: String, message: String): CryptoProFlutterBaseException("Не удалось получить сертификат из контейнера $container \n$message", "6", null)

class GetCertificatePrivateKeyException(message: String): CryptoProFlutterBaseException("Не удалось получить приватный ключ из контейнера \n$message", "7", null)

class ReadSignatureFromStreamException(message: String): CryptoProFlutterBaseException("Не удалось записать подпись из потока \n$message", "8", null)

class AddSignerUnknownException(cause: Throwable, errorCode: Int): CryptoProFlutterBaseException("Неизвестная ошибка с кодом $errorCode", "9", cause)