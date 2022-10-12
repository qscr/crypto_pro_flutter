import 'dart:convert';
import 'dart:io';

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

  @override
  Future<String> addCertificate(File file, String password) async {
    try {
      String response = await methodChannel.invokeMethod(
        "addPfxCertificate",
        {
          "path": file.path,
          "password": password,
        },
      );
      Map map = json.decode(response) as Map;
      return "true";
    } catch (exception) {
      throw Exception();
    }
  }
}
