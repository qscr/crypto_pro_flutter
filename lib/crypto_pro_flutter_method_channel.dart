import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';

import 'crypto_pro_flutter_platform_interface.dart';

/// An implementation of [CryptoProFlutterPlatform] that uses method channels.
class MethodChannelCryptoProFlutter extends CryptoProFlutterPlatform {
  /// The method channel used to interact with the native platform.
  @visibleForTesting
  final methodChannel = const MethodChannel('crypto_pro_flutter');

  @override
  Future<String?> getPlatformVersion() async {
    final version = await methodChannel.invokeMethod<String>('getPlatformVersion');
    return version;
  }
}
