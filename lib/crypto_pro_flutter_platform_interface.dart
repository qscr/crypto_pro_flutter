import 'dart:io';

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

  Future<bool> initCSP() {
    throw UnimplementedError('platformVersion() has not been implemented.');
  }

  Future<String> addCertificate(File file, String password) {
    throw UnimplementedError('addCertificate() has not been implemented.');
  }
}
