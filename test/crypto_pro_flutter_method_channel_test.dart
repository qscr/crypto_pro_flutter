import 'package:flutter/services.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:crypto_pro_flutter/crypto_pro_flutter_method_channel.dart';

void main() {
  MethodChannelCryptoProFlutter platform = MethodChannelCryptoProFlutter();
  const MethodChannel channel = MethodChannel('crypto_pro_flutter');

  TestWidgetsFlutterBinding.ensureInitialized();

  setUp(() {
    channel.setMockMethodCallHandler((MethodCall methodCall) async {
      return true;
    });
  });

  tearDown(() {
    channel.setMockMethodCallHandler(null);
  });

  test('initCSP', () async {
    expect(await platform.initCSP(), true);
  });
}
