import 'dart:convert';

import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';

import 'crypto_pro_flutter_platform_interface.dart';

/// An implementation of [CryptoProFlutterPlatform] that uses method channels.
class MethodChannelCryptoProFlutter extends CryptoProFlutterPlatform {
  /// The method channel used to interact with the native platform.
  @visibleForTesting
  final methodChannel = const MethodChannel('crypto_pro_flutter');

  @override
  Future<bool> initCSP() async {
    final result = await methodChannel.invokeMethod<bool>('initCSP');
    return result ?? false;
  }

  Future<Certificate> addCertificate(File file, String password) async {
    try {
      String response = await methodChannel
          .invokeMethod("addPfxCertificate", {"path": file.path, "password": password});
      Map map = json.decode(response) as Map;
      if (map["success"] as bool) {
        Certificate certificate = Certificate.fromBase64(map);
        Directory directory = await getApplicationDocumentsDirectory();
        String filePath = "${directory.path}/certificates/${certificate.uuid}.pfx";
        File(filePath);
        await file.copy(filePath);
        file.delete();
        return certificate;
      }
      throw ApiResponseException(map["message"] as String, map["exception"].toString());
    } catch (exception) {
      if (exception is ApiResponseException) rethrow;
      if (exception is PlatformException)
        throw ApiResponseException(exception.message, exception.details.toString());
      throw ApiResponseException(
          "Не удалось добавить сертификат в хранилище", exception.toString());
    }
  }
}
