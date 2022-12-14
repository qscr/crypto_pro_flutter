import 'dart:io';

import 'package:crypto_pro_flutter/models/certificate.dart';
import 'package:plugin_platform_interface/plugin_platform_interface.dart';

import 'crypto_pro_flutter_method_channel.dart';

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
  Future<Certificate> addCertificate(File file, String password) {
    throw UnimplementedError('addCertificate() has not been implemented.');
  }

  /// Удалить сертификат
  Future<void> deleteCertificate(Certificate certificate) {
    throw UnimplementedError('signMessage() has not been implemented.');
  }

  /// Получит список установленных сертификатов
  Future<List<Certificate>> getInstalledCertificates() {
    throw UnimplementedError('getInstalledCertificates() has not been implemented.');
  }

  /// Подписать файл
  Future<String> signFile({
    required File file,
    required Certificate certificate,
    required String password,
    bool isDetached = true,
    bool disableOnlineValidation = false,
  }) {
    throw UnimplementedError('signFile() has not been implemented.');
  }

  /// Подписать сообщение
  Future<String> signMessage({
    required String message,
    required Certificate certificate,
    required String password,
    bool isDetached = true,
    bool signHash = false,
    bool disableOnlineValidation = false,
  }) {
    throw UnimplementedError('signMessage() has not been implemented.');
  }
}
