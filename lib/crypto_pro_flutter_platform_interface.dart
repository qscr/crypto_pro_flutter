import 'dart:io';

import 'package:crypto_pro_flutter/models/certificate.dart';
import 'package:plugin_platform_interface/plugin_platform_interface.dart';

import 'crypto_pro_flutter_method_channel.dart';

enum CAdESFormat {
  BES,
  XLongType1,
}

abstract class CryptoProFlutterPlatform extends PlatformInterface {
  /// Constructs a CryptoProFlutterPlatform.
  CryptoProFlutterPlatform() : super(token: _token);

  static final Object _token = Object();

  static CryptoProFlutterPlatform _instance = MethodChannelCryptoProFlutter();

  /// The default instance of [CryptoProFlutterPlatform] to use.
  ///
  /// Defaults to [MethodChannelCryptoProFlutter].
  static CryptoProFlutterPlatform get instance => _instance;

  /// Platform-specific implementations should set this with their own
  /// platform-specific class that extends [CryptoProFlutterPlatform] when
  /// they register themselves.
  static set instance(CryptoProFlutterPlatform instance) {
    PlatformInterface.verifyToken(instance, _token);
    _instance = instance;
  }

  /// Инициализация провайдера
  Future<bool> initCSP() {
    throw UnimplementedError('initCSP() has not been implemented.');
  }

  /// Добавить новый сертификат в формате Pfx
  Future<Certificate> addCertificate({
    required File file,
    required String password,
    String? newPassword,
  }) {
    throw UnimplementedError('addCertificate() has not been implemented.');
  }

  /// Добавить внешний контейнер в хранилище
  Future<Certificate> addContainerFromExternalStorage({
    required String storageName,
    required String password,
    String? newPassword,
  }) {
    throw UnimplementedError('addCertificate() has not been implemented.');
  }

  /// Получить сертификат с приватным ключом из внутреннего хранилища приложения
  Future<Certificate> getPrivateKeyFromInternalContainerByAlias({
    required String certificateAlias,
    required String password,
  }) async {
    throw UnimplementedError('addCertificate() has not been implemented.');
  }

  /// Удаление сертификата и приватного ключа из внутреннего хранилища
  Future<void> removeCertificateWithPrivateKeyFromStorage({
    required String certificateAlias,
  }) async {
    throw UnimplementedError('addCertificate() has not been implemented.');
  }

  /// Получить сертификат с приватным ключом из внешнего контейнера
  Future<Certificate> readContainerFromExternalStorage({
    required String storageName,
    required String password,
  }) {
    throw UnimplementedError('addCertificate() has not been implemented.');
  }

  /// Добавить сертификаты в хранилище доверенных приложения
  Future<void> addCertificatesToTrustedStorage({
    required List<String> paths,
  }) {
    throw UnimplementedError('addCertificate() has not been implemented.');
  }

  /// Удалить сертификат
  Future<void> deleteCertificate(Certificate certificate) {
    throw UnimplementedError('signMessage() has not been implemented.');
  }

  /// Получит список установленных сертификатов
  Future<List<Certificate>> getInstalledCertificates() {
    throw UnimplementedError(
        'getInstalledCertificates() has not been implemented.');
  }

  /// Подписать файл
  Future<String> signFile({
    required File file,
    required Certificate certificate,
    required String password,
    required CAdESFormat format,
    bool isDetached = true,
    bool disableOnlineValidation = false,
    String? tsaUrl,
    String? storageName,
  }) {
    throw UnimplementedError('signFile() has not been implemented.');
  }

  /// Подписать сообщение
  Future<String> signMessage({
    required String message,
    required Certificate certificate,
    required String password,
    required CAdESFormat format,
    bool isDetached = true,
    bool signHash = false,
    bool disableOnlineValidation = false,
    String? tsaUrl,
    String? storageName,
  }) {
    throw UnimplementedError('signMessage() has not been implemented.');
  }
}
