import 'dart:io';

import 'package:crypto_pro_flutter/crypto_pro_api.g.dart';
import 'package:crypto_pro_flutter/crypto_pro_flutter.dart';
import 'package:crypto_pro_flutter/models/exceptions.dart';
import 'package:flutter/services.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:integration_test/integration_test.dart';
import 'package:path_provider/path_provider.dart';

Future<File> assetToTempFile(String assetPath, {String? fileName}) async {
  final bytes = await rootBundle.load(assetPath);
  final dir = await getTemporaryDirectory();
  final out = File('${dir.path}/${fileName ?? assetPath.split('/').last}');
  await out.writeAsBytes(bytes.buffer.asUint8List(), flush: true);
  return out;
}

void main() {
  const pfxPassword = "123123";

  IntegrationTestWidgetsFlutterBinding.ensureInitialized();

  late CryptoProFlutter crypto;

  setUpAll(() async {
    crypto = CryptoProFlutter();
  });

  tearDown(() async {
    final certificates = await crypto.getInstalledCertificatesWithPrivateKeys();
    for (var certificate in certificates) {
      if (certificate.alias != null) {
        await crypto
            .deleteCertificateWithPKeyFromPersonalStorage(certificate.alias!);
      }
    }
  });

  group('CryptoProFlutter integration', () {
    testWidgets('initCSP works', (tester) async {
      final ok = await crypto.initCSP();
      expect(ok, isTrue);
    });

    testWidgets('getLicense completes without errors', (tester) async {
      await crypto.getLicense();
    });

    testWidgets('setLicense works', (tester) async {
      const serialNumber = "50502-Y0000-01D8L-5PQLK-TFGQW";
      await crypto.setLicense(serialNumber);
      final lic = await crypto.getLicense();

      expect(
        lic?.serialNumber?.replaceAll("-", ""),
        equals(serialNumber.replaceAll("-", "")),
      );
    });

    testWidgets('addCertificatesToTrustedStorage completes without errors',
        (tester) async {
      final root = await assetToTempFile('assets/cert.cer');
      final chain = await assetToTempFile('assets/certs.p7b');

      await crypto
          .addCertificatesToTrustedStorage(paths: [root.path, chain.path]);
    });

    testWidgets('addPfxCertificate + getInstalledCertificates contains it',
        (tester) async {
      final pfx = await assetToTempFile('assets/container.pfx');

      final certs = await crypto.importPFXContainer(
        file: pfx,
        password: pfxPassword,
        newPassword: null,
      );

      expect(certs.firstOrNull, isNotNull);

      final certAlias = certs.first.alias;

      final installed = await crypto.getInstalledCertificatesWithPrivateKeys();
      expect(installed.any((c) => c.alias == certAlias), isTrue);
    });

    testWidgets('signFile returns base64 signature', (tester) async {
      final pfx = await assetToTempFile('assets/container.pfx');
      final dataFile = await assetToTempFile('assets/sample.doc');

      final certs = await crypto.importPFXContainer(
        file: pfx,
        password: pfxPassword,
      );

      expect(certs.firstOrNull?.alias, isNotNull);

      final sign = await crypto.signFile(
        file: dataFile,
        certificateAlias: certs.first.alias!,
        password: pfxPassword,
        format: CAdESFormat.BES,
        signHash: false,
        isDetached: true,
        disableOnlineValidation: false,
      );

      expect(sign.base64, isNotEmpty);
    });

    testWidgets('signMessage returns base64 signature', (tester) async {
      final pfx = await assetToTempFile('assets/container.pfx');

      final certs = await crypto.importPFXContainer(
        file: pfx,
        password: pfxPassword,
      );

      expect(certs.firstOrNull?.alias, isNotNull);

      final sign = await crypto.signMessage(
        message: "Test",
        certificateAlias: certs.first.alias!,
        password: pfxPassword,
        format: CAdESFormat.BES,
        signHash: false,
        isDetached: true,
        disableOnlineValidation: false,
      );

      expect(sign.base64, isNotEmpty);
    });

    testWidgets(
        'wrongPassword exception successfully handled through PFX import',
        (tester) async {
      final pfx = await assetToTempFile('assets/container.pfx');

      expect(
        () => crypto.importPFXContainer(
          file: pfx,
          password: "${pfxPassword}1",
        ),
        throwsA(isA<WrongPasswordException>()),
      );
    });

    testWidgets('wrongPassword exception successfully handled through signing',
        (tester) async {
      final pfx = await assetToTempFile('assets/container.pfx');

      final certs = await crypto.importPFXContainer(
        file: pfx,
        password: pfxPassword,
        newPassword: "${pfxPassword}1",
      );

      expect(certs.firstOrNull?.alias, isNotNull);

      expect(
        () => crypto.signMessage(
          message: "Test",
          certificateAlias: certs.first.alias!,
          password: pfxPassword,
          format: CAdESFormat.BES,
          signHash: false,
          isDetached: true,
          disableOnlineValidation: false,
        ),
        throwsA(isA<WrongPasswordException>()),
      );
    });
  });
}
