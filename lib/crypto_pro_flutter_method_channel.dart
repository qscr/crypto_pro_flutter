import 'dart:convert';
import 'dart:io';

import 'package:crypto_pro_flutter/models/certificate.dart';
import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';

import 'crypto_pro_flutter_platform_interface.dart';

/// An implementation of [CryptoProFlutterPlatform] that uses method channels.
class MethodChannelCryptoProFlutter extends CryptoProFlutterPlatform {
  @visibleForTesting
  final methodChannel = const MethodChannel('crypto_pro_flutter');

  @override
  Future<bool> initCSP() async {
    final result = await methodChannel.invokeMethod<bool>('initCSP');
    return result ?? false;
  }

  @override
  Future<Certificate> addCertificate(File file, String password) async {
    try {
      String response = await methodChannel.invokeMethod(
        "addPfxCertificate",
        {
          "path": file.path,
          "password": password,
        },
      );
      Map<String, dynamic> map = json.decode(response) as Map<String, dynamic>;
      final certificate = Certificate.fromMap(map["certificate"]);
      return certificate;
    } catch (exception) {
      throw Exception();
    }
  }

  @override
  Future<void> deleteCertificate(Certificate certificate) async {
    try {
      await methodChannel.invokeMethod(
        "deletePfxCertificate",
        {
          "alias": certificate.alias,
        },
      );
    } catch (exception) {
      throw Exception();
    }
  }

  @override
  Future<List<Certificate>> getInstalledCertificates() async {
    try {
      String response = await methodChannel.invokeMethod("getInstalledCertificates");
      Map<String, dynamic> map = json.decode(response);
      final certificatesMaps = List<Map<String, dynamic>>.from(map['certificates'] as List);
      return certificatesMaps.map((e) => Certificate.fromMap(e)).toList();
    } catch (exception) {
      throw Exception();
    }
  }

  @override
  Future<String> signFile({
    required File file,
    required Certificate certificate,
    required String password,
    bool isDetached = true,
    bool disableOnlineValidation = false,
  }) async {
    String response = await methodChannel.invokeMethod(
      "signFile",
      {
        "path": file.path,
        "alias": certificate.alias,
        "password": password,
        "isDetached": isDetached,
        "disableOnlineValidation": disableOnlineValidation,
      },
    );
    Map<String, dynamic> map = json.decode(response);
    if (map["success"] == false) {
      throw Exception(map["message"]);
    }
    return map["signBase64"] as String;
  }

  @override
  Future<String> signMessage({
    required String message,
    required Certificate certificate,
    required String password,
    bool isDetached = true,
    bool signHash = false,
    bool disableOnlineValidation = false,
  }) async {
    try {
      String response = await methodChannel.invokeMethod(
        "signMessage",
        {
          "message": message,
          "alias": certificate.alias,
          "password": password,
          "isDetached": isDetached,
          "signHash": signHash,
          "disableOnlineValidation": disableOnlineValidation,
        },
      );
      Map<String, dynamic> map = json.decode(response);
      return map["signBase64"] as String;
    } catch (exception) {
      throw Exception();
    }
  }
}
