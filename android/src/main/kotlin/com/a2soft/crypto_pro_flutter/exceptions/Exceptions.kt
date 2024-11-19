package com.a2soft.crypto_pro_flutter.exceptions

class NoPrivateKeyFound : Exception("Приватный ключ, связанный с сертификатом, не найден")
class CustomWrongPasswordException: Exception("Указан неправильный пароль от контейнера")
