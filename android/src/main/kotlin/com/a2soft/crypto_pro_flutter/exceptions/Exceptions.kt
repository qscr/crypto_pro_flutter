package com.a2soft.crypto_pro_flutter.exceptions

class NoPrivateKeyFound: Exception("Приватный ключ, связанный с сертификатом, не найден")
class CustomWrongPasswordException: Exception("Указан неправильный пароль от контейнера")
class ArgumentsParsingException: Exception("Обязательные аругменты не переданы или переданы с ошибкой")
class SomeCertificatesAreNotAddedToTrustStoreException(errorCertificateNames: Array<String>): Exception("Не удалось добавить следующие сертификаты: " + errorCertificateNames.joinToString(", "))
