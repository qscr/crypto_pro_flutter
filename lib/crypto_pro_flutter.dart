import 'dart:io';

import 'crypto_pro_flutter_platform_interface.dart';

class CryptoProFlutter {
  Future<bool> initCSP() {
    return CryptoProFlutterPlatform.instance.initCSP();
  }

  Future<String> addPfxCertificate(File file, String password) async {
    return await CryptoProFlutterPlatform.instance.addCertificate(file, password);
  }
}
