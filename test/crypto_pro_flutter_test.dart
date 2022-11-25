import 'package:crypto_pro_flutter/crypto_pro_flutter_platform_interface.dart';

// class MockCryptoProFlutterPlatform
//     with MockPlatformInterfaceMixin
//     implements CryptoProFlutterPlatform {
//   @override
//   Future<bool> initCSP() {
//     return Future(() => true);
//   }

//   @override
//   Future<String> addCertificate(File file, String password) {
//     // TODO: implement addCertificate
//     throw UnimplementedError();
//   }
// }

void main() {
  final CryptoProFlutterPlatform initialPlatform = CryptoProFlutterPlatform.instance;

  // test('$MethodChannelCryptoProFlutter is the default instance', () {
  //   expect(initialPlatform, isInstanceOf<MethodChannelCryptoProFlutter>());
  // });

  // test('initCSP', () async {
  //   CryptoProFlutter cryptoProFlutterPlugin = CryptoProFlutter();
  //   MockCryptoProFlutterPlatform fakePlatform = MockCryptoProFlutterPlatform();
  //   CryptoProFlutterPlatform.instance = fakePlatform;

  //   expect(await cryptoProFlutterPlugin.initCSP(), true);
  // });
}
