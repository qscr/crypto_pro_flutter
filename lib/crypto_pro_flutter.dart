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

  /// Получить сертификат с приватным ключом из внешнего контейнера
  Future<Certificate> readContainerFromExternalStorage({
    required String storageName,
    required String password,
  }) async {
    return await CryptoProFlutterPlatform.instance
        .readContainerFromExternalStorage(
      storageName: storageName,
      password: password,
    );
  }

  /// Получить сертификат с приватным ключом из внутреннего хранилища приложения
  Future<Certificate> getPrivateKeyFromInternalContainerByAlias({
    required String certificateAlias,
    required String password,
  }) async {
    return await CryptoProFlutterPlatform.instance
        .readContainerFromExternalStorage(
      storageName: certificateAlias,
      password: password,
    );
  }

  /// Добавить новый сертификат в формате Pfx в хранилище приложения
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

  /// Удалить установленный сертификат из хранилища приложения
  Future<void> deleteCertificate(Certificate certificate) async {
    await CryptoProFlutterPlatform.instance.deleteCertificate(certificate);
  }

  /// Получит список установленных сертификатов в хранилище приложения
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
    String? storageName,
  }) async {
    return await CryptoProFlutterPlatform.instance.signFile(
      file: file,
      certificate: certificate,
      password: password,
      isDetached: true,
      disableOnlineValidation: disableOnlineValidation,
      format: format,
      tsaUrl: tsaUrl,
      storageName: storageName,
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
    String? storageName,
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
      storageName: storageName,
    );
  }
}
