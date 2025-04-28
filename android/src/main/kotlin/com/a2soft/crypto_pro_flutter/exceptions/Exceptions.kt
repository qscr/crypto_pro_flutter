package com.a2soft.crypto_pro_flutter.exceptions

open class CryptoProFlutterBaseException(override val message: String, val code: String): Exception()

class NoPrivateKeyFound: CryptoProFlutterBaseException("Приватный ключ, связанный с сертификатом, не найден", "1")
class CustomWrongPasswordException: CryptoProFlutterBaseException("Указан неправильный пароль от контейнера", "2")
class ArgumentsParsingException: CryptoProFlutterBaseException("Обязательные аргументы не переданы или переданы с ошибкой", "3")
class SomeCertificatesAreNotAddedToTrustStoreException(errorCertificateNames: Array<String>): CryptoProFlutterBaseException("Не удалось добавить следующие сертификаты: " + errorCertificateNames.joinToString(", "), "4")
class CertificateStatusUnknownOrRevokedException: CryptoProFlutterBaseException("Не удалось проверить сертификат на отзыв", "5")

class GetCertificateFromContainerException(container: String, message: String): CryptoProFlutterBaseException("Не удалось получить сертификат из контейнера $container \n$message", "6")

class GetCertificatePrivateKeyException(message: String): CryptoProFlutterBaseException("Не удалось получить приватный ключ из контейнера \n$message", "7")

class ReadSignatureFromStreamException(message: String): CryptoProFlutterBaseException("Не удалось записать подпись из потока \n$message", "8")

