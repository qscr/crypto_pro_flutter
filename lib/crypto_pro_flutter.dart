import 'crypto_pro_flutter_platform_interface.dart';

class CryptoProFlutter {
  Future<bool> initCSP() {
    return CryptoProFlutterPlatform.instance.initCSP();
  }
}
