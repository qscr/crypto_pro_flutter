
import 'crypto_pro_flutter_platform_interface.dart';

class CryptoProFlutter {
  Future<String?> getPlatformVersion() {
    return CryptoProFlutterPlatform.instance.getPlatformVersion();
  }
}
