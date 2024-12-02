import 'dart:io';

import 'package:crypto_pro_flutter/models/certificate.dart';

import 'crypto_pro_flutter_platform_interface.dart';

class CryptoProFlutter {
  /// Инициализация провайдера
  Future<bool> initCSP() {
    return CryptoProFlutterPlatform.instance.initCSP();
  }

  /// Добавить сертификаты в хранилище доверенных приложения
  Future<void> addCertificatesToTrustedStorage({
    required List<String> paths,
  }) async {
    return CryptoProFlutterPlatform.instance.addCertificatesToTrustedStorage(
      paths: paths,
    );
  }

  /// Добавить внешний контейнер в хранилище
  Future<Certificate> addContainerFromExternalStorage({
    required String storageName,
    required String password,
    String? newPassword,
  }) async {
    return await CryptoProFlutterPlatform.instance
        .addContainerFromExternalStorage(
      storageName: storageName,
      password: password,
      newPassword: newPassword,
    );
  }

  /// Добавить новый сертификат в формате Pfx
  Future<Certificate> addPfxCertificate({
    required File file,
    required String password,
    String? newPassword,
  }) async {
    return await CryptoProFlutterPlatform.instance.addCertificate(
      file: file,
      password: password,
      newPassword: newPassword,
    );
  }

  /// Удалить установленный сертификат
  Future<void> deleteCertificate(Certificate certificate) async {
    await CryptoProFlutterPlatform.instance.deleteCertificate(certificate);
  }

  /// Получит список установленных сертификатов
  Future<List<Certificate>> getInstalledCertificates() async {
    final list =
        await CryptoProFlutterPlatform.instance.getInstalledCertificates();
    return list;
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
  }) async {
    return await CryptoProFlutterPlatform.instance.signFile(
      file: file,
      certificate: certificate,
      password: password,
      isDetached: true,
      disableOnlineValidation: disableOnlineValidation,
      format: format,
      tsaUrl: tsaUrl,
    );
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
  }) async {
    return await CryptoProFlutterPlatform.instance.signMessage(
      message: message,
      certificate: certificate,
      password: password,
      isDetached: isDetached,
      signHash: signHash,
      disableOnlineValidation: disableOnlineValidation,
      format: format,
      tsaUrl: tsaUrl,
    );
  }
}
