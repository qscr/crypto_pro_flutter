import 'dart:io';

import 'package:crypto_pro_flutter/crypto_pro_api.g.dart';
import 'package:crypto_pro_flutter/crypto_pro_flutter.dart';
import 'package:mocktail/mocktail.dart';
import 'package:test/test.dart';

class MockCryptoProFlutterApi extends Mock implements CryptoProFlutterApi {}

void main() {
  late MockCryptoProFlutterApi api;
  late CryptoProFlutter plugin;

  setUpAll(() {
    registerFallbackValue(CAdESFormat.BES);
  });

  setUp(() {
    api = MockCryptoProFlutterApi();
    plugin = CryptoProFlutter.createForTest(api);
  });

  group('CryptoProFlutter unit tests', () {
    test('initCSP forwards call and returns value', () async {
      when(() => api.initCSP()).thenAnswer((_) async => true);

      final res = await plugin.initCSP();

      expect(res, isTrue);
      verify(() => api.initCSP()).called(1);
      verifyNoMoreInteractions(api);
    });

    test('getLicense forwards call and returns value', () async {
      final installDate = DateTime(2024, 1, 1).toIso8601String();
      final license = CryptoProLicenseInfo(
        expiredThroughDays: 10,
        licenseInstallDate: installDate,
        existingLicenseStatus: 0,
        serialNumber: 'ABC',
      );
      when(() => api.getLicense()).thenAnswer((_) async => license);

      final res = await plugin.getLicense();

      expect(res?.serialNumber, 'ABC');
      expect(res?.existingLicenseStatus, 0);
      expect(res?.licenseInstallDate, installDate);
      expect(res?.expiredThroughDays, 10);
      verify(() => api.getLicense()).called(1);
      verifyNoMoreInteractions(api);
    });

    test('setLicense forwards number', () async {
      when(() => api.setLicense('123')).thenAnswer((_) async => true);

      final res = await plugin.setLicense('123');

      expect(res, isTrue);
      verify(() => api.setLicense('123')).called(1);
      verifyNoMoreInteractions(api);
    });

    test('addCertificatesToTrustedStorage forwards paths', () async {
      when(() =>
              api.addCertificatesToTrustedStorage(paths: any(named: 'paths')))
          .thenAnswer((_) async {});

      await plugin.addCertificatesToTrustedStorage(paths: ['a.cer', 'b.crt']);

      verify(() =>
              api.addCertificatesToTrustedStorage(paths: ['a.cer', 'b.crt']))
          .called(1);
      verifyNoMoreInteractions(api);
    });

    test('addPfxCertificate forwards file path + passwords', () async {
      final cert = Certificate(alias: 'ALIAS');
      when(() => api.importPFXContainer(
            filePath: any(named: 'filePath'),
            password: any(named: 'password'),
            newPassword: any(named: 'newPassword'),
          )).thenAnswer((_) async => [cert]);

      final tmp = File('test/fixtures/test.pfx');
      final res = await plugin.importPFXContainer(
        file: tmp,
        password: 'old',
        newPassword: 'new',
      );

      expect(res.first.alias, 'ALIAS');
      verify(() => api.importPFXContainer(
            filePath: tmp.path,
            password: 'old',
            newPassword: 'new',
          )).called(1);
      verifyNoMoreInteractions(api);
    });

    test('getInstalledCertificates forwards call', () async {
      when(() => api.getInstalledCertificatesWithPrivateKeys())
          .thenAnswer((_) async => [
                Certificate(alias: 'A'),
                Certificate(alias: 'B'),
              ]);

      final res = await plugin.getInstalledCertificatesWithPrivateKeys();

      expect(res.map((e) => e.alias).toList(), ['A', 'B']);
      verify(() => api.getInstalledCertificatesWithPrivateKeys()).called(1);
      verifyNoMoreInteractions(api);
    });

    test('deleteCertificate forwards alias', () async {
      when(() => api.deleteCertificateWithPKeyFromPersonalStorage('ABC'))
          .thenAnswer((_) async {});

      await plugin.deleteCertificateWithPKeyFromPersonalStorage('ABC');

      verify(() => api.deleteCertificateWithPKeyFromPersonalStorage('ABC'))
          .called(1);
      verifyNoMoreInteractions(api);
    });

    test('signFile forwards args correctly', () async {
      final cert = Certificate(alias: 'ALIAS');
      final file = File('test/fixtures/data.txt');

      final expected = SuccessSign(base64: 'BASE64SIG');
      when(() => api.signFile(
            filePath: any(named: 'filePath'),
            certificateAlias: any(named: 'certificateAlias'),
            password: any(named: 'password'),
            format: any(named: 'format'),
            signHash: any(named: 'signHash'),
            isDetached: any(named: 'isDetached'),
            disableOnlineValidation: any(named: 'disableOnlineValidation'),
            tsaUrl: any(named: 'tsaUrl'),
            storageName: any(named: 'storageName'),
          )).thenAnswer((_) async => expected);

      final res = await plugin.signFile(
        file: file,
        certificateAlias: cert.alias ?? '',
        password: '1234',
        format: CAdESFormat.BES,
        signHash: true,
        isDetached: false,
        disableOnlineValidation: true,
        tsaUrl: 'https://tsa.test',
        storageName: 'MY',
      );

      expect(res.base64, 'BASE64SIG');

      verify(() => api.signFile(
            filePath: file.path,
            certificateAlias: 'ALIAS',
            password: '1234',
            format: CAdESFormat.BES,
            signHash: true,
            isDetached: false,
            disableOnlineValidation: true,
            tsaUrl: 'https://tsa.test',
            storageName: 'MY',
          )).called(1);
      verifyNoMoreInteractions(api);
    });

    test('signMessage forwards args correctly', () async {
      final cert = Certificate(alias: 'ALIAS');

      final expected = SuccessSign(base64: 'SIG');
      when(() => api.signMessage(
            message: any(named: 'message'),
            certificateAlias: any(named: 'certificateAlias'),
            password: any(named: 'password'),
            format: any(named: 'format'),
            signHash: any(named: 'signHash'),
            isDetached: any(named: 'isDetached'),
            disableOnlineValidation: any(named: 'disableOnlineValidation'),
            tsaUrl: any(named: 'tsaUrl'),
            storageName: any(named: 'storageName'),
          )).thenAnswer((_) async => expected);

      final res = await plugin.signMessage(
        message: 'hello',
        certificateAlias: cert.alias ?? '',
        password: '1234',
        format: CAdESFormat.BES,
        isDetached: true,
        signHash: false,
      );

      expect(res.base64, 'SIG');

      verify(() => api.signMessage(
            message: 'hello',
            certificateAlias: 'ALIAS',
            password: '1234',
            format: CAdESFormat.BES,
            signHash: false,
            isDetached: true,
            disableOnlineValidation: false,
            tsaUrl: null,
            storageName: null,
          )).called(1);
      verifyNoMoreInteractions(api);
    });
  });
}
