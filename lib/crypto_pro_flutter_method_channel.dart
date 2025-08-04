import 'dart:convert';
import 'dart:io';

import 'package:crypto_pro_flutter/models/certificate.dart';
import 'package:crypto_pro_flutter/models/license.dart';
import 'package:crypto_pro_flutter/utils/exception_handler_mixin.dart';
import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';

import 'crypto_pro_flutter_platform_interface.dart';

/// An implementation of [CryptoProFlutterPlatform] that uses method channels.
class MethodChannelCryptoProFlutter extends CryptoProFlutterPlatform
    with ExceptionHandlerMixin {
  @visibleForTesting
  final methodChannel = const MethodChannel('crypto_pro_flutter');

  @override
  Future<bool> initCSP() async {
    final result = await methodChannel.invokeMethod<bool>('initCSP');
    return result ?? false;
  }

  /// Получить информацию о лицензировании
  @override
  Future<CryptoProLicenseInfo> getLicense() async {
    String response = await methodChannel.invokeMethod('getLicense');
    Map<String, dynamic> map = json.decode(response) as Map<String, dynamic>;
    final info = CryptoProLicenseInfo.fromMap(map);
    return info;
  }

  /// Установить новую лицензию
  @override
  Future<bool> setLicense(String number) async {
    final result = await methodChannel.invokeMethod<bool>(
      'setLicense',
      {
        "number": number,
      },
    );
    return result ?? false;
  }

  @override
  Future<Certificate> addCertificate({
    required File file,
    required String password,
    String? newPassword,
  }) async {
    String response = await methodChannel.invokeMethod(
      "addPfxCertificate",
      {
        "path": file.path,
        "password": password,
        "newPassword": newPassword,
      },
    );
    Map<String, dynamic> map = json.decode(response) as Map<String, dynamic>;
    final certificate = Certificate.fromMap(map["certificate"]);
    return certificate;
  }

  @override
  Future<void> deleteCertificate(Certificate certificate) async {
    await methodChannel.invokeMethod(
      "deletePfxCertificate",
      {
        "alias": certificate.alias,
      },
    );
  }

  @override
  Future<List<Certificate>> getInstalledCertificates() async {
    String response =
        await methodChannel.invokeMethod("getInstalledCertificates");
    Map<String, dynamic> map = json.decode(response);
    final certificatesMaps =
        List<Map<String, dynamic>>.from(map['certificates'] as List);
    return certificatesMaps.map((e) => Certificate.fromMap(e)).toList();
  }

  /// Получить сертификат с приватным ключом из внутреннего хранилища приложения
  @override
  Future<Certificate> getPrivateKeyFromInternalContainerByAlias({
    required String certificateAlias,
    required String password,
  }) async {
    String response = await methodChannel.invokeMethod(
      "getPrivateKeyFromInternalContainerByAlias",
      {
        "alias": certificateAlias,
        "password": password,
      },
    );
    Map<String, dynamic> map = json.decode(response) as Map<String, dynamic>;
    final certificate = Certificate.fromMap(map["certificate"]);
    return certificate;
  }

  /// Удаление сертификата и приватного ключа из внутреннего хранилища
  @override
  Future<void> removeCertificateWithPrivateKeyFromStorage({
    required String certificateAlias,
  }) async {
    await methodChannel.invokeMethod(
      "removeCertificateWithPrivateKeyFromStorage",
      {
        "alias": certificateAlias,
      },
    );
  }

  /// Добавить внешний контейнер в хранилище
  @override
  Future<Certificate> addContainerFromExternalStorage({
    required String storageName,
    required String password,
    String? newPassword,
  }) async {
    String response = await methodChannel.invokeMethod(
      "addFromExternalStorage",
      {
        "storageName": storageName,
        "password": password,
        "newPassword": newPassword,
      },
    );
    Map<String, dynamic> map = json.decode(response) as Map<String, dynamic>;
    final certificate = Certificate.fromMap(map["certificate"]);
    return certificate;
  }

  /// Получить сертификат с приватным ключом из внешнего контейнера
  @override
  Future<Certificate> readContainerFromExternalStorage({
    required String storageName,
    required String password,
    String? newPassword,
  }) async {
    String response = await methodChannel.invokeMethod(
      "readFromExternalStorage",
      {
        "storageName": storageName,
        "password": password,
      },
    );
    Map<String, dynamic> map = json.decode(response) as Map<String, dynamic>;
    final certificate = Certificate.fromMap(map["certificate"]);
    return certificate;
  }

  @override
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
    return callWithHandler(() async {
      String response = await methodChannel.invokeMethod(
        "signFile",
        {
          "path": file.path,
          "alias": certificate.alias,
          "password": password,
          "isDetached": isDetached,
          "disableOnlineValidation": disableOnlineValidation,
          "format": format.name,
          "tsaUrl": tsaUrl,
          "storageName": storageName,
        },
      );
      Map<String, dynamic> map = json.decode(response);
      return map["signBase64"] as String;
    });
  }

  /// Добавить сертификаты в хранилище доверенных приложения
  @override
  Future<void> addCertificatesToTrustedStorage({
    required List<String> paths,
  }) async {
    await methodChannel.invokeMethod(
      "addCertificatesToTrustedStorage",
      {
        "paths": paths,
      },
    );
  }

  @override
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
    String response = await methodChannel.invokeMethod(
      "signMessage",
      {
        "message": message,
        "alias": certificate.alias,
        "password": password,
        "isDetached": isDetached,
        "signHash": signHash,
        "disableOnlineValidation": disableOnlineValidation,
        "format": format.name,
        "tsaUrl": tsaUrl,
        "storageName": storageName,
      },
    );
    Map<String, dynamic> map = json.decode(response);
    return map["signBase64"] as String;
  }
}
