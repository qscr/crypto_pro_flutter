import 'dart:io';

import 'package:crypto_pro_flutter/models/certificate.dart';

import 'crypto_pro_flutter_platform_interface.dart';

class CryptoProFlutter {
  /// Инициализация провайдера
  Future<bool> initCSP() {
    return CryptoProFlutterPlatform.instance.initCSP();
  }

  /// Добавить новый сертификат в формате Pfx
  Future<Certificate> addPfxCertificate(File file, String password) async {
    return await CryptoProFlutterPlatform.instance.addCertificate(file, password);
  }

  /// Удалить установленный сертификат
  Future<void> deleteCertificate(Certificate certificate) async {
    await CryptoProFlutterPlatform.instance.deleteCertificate(certificate);
  }

  /// Получит список установленных сертификатов
  Future<List<Certificate>> getInstalledCertificates() async {
    final list = await CryptoProFlutterPlatform.instance.getInstalledCertificates();
    return list;
  }

  /// Подписать файл
  Future<String> signFile({
    required File file,
    required Certificate certificate,
    required String password,
    bool isDetached = true,
  }) async {
    return await CryptoProFlutterPlatform.instance.signFile(
      file: file,
      certificate: certificate,
      password: password,
      isDetached: true,
    );
  }

  /// Подписать сообщение
  Future<String> signMessage({
    required String message,
    required Certificate certificate,
    required String password,
    bool isDetached = true,
    bool signHash = false,
  }) async {
    return await CryptoProFlutterPlatform.instance.signMessage(
      message: message,
      certificate: certificate,
      password: password,
      isDetached: isDetached,
      signHash: signHash,
    );
  }
}
