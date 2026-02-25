import 'package:pigeon/pigeon.dart';

@ConfigurePigeon(
  PigeonOptions(
    dartOut: 'lib/crypto_pro_api.g.dart',
    kotlinOut:
        'android/src/main/kotlin/com/a2soft/crypto_pro_flutter/CryptoProApi.g.kt',
    swiftOut: 'ios/Classes/CryptoProApi.g.swift',
    dartPackageName: 'crypto_pro_flutter',
  ),
)
@HostApi()
abstract class CryptoProFlutterApi {
  bool initCSP();

  CryptoProLicenseInfo? getLicense();

  bool setLicense(String number);

  @async
  List<Certificate> importPFXContainer({
    required String filePath,
    required String password,
    String? newPassword,
  });

  @async
  List<Certificate> importContainerFromExternalStorage({
    required String storageName,
    required String password,
    String? newPassword,
  });

  @async
  Certificate getCertificateWithPrivateKeyFromPersonalContainer({
    required String certificateAlias,
    required String password,
  });

  @async
  List<Certificate> readContainerFromExternalStorage({
    required String storageName,
    required String password,
  });

  @async
  void addCertificatesToTrustedStorage({
    required List<String> paths,
  });

  @async
  void deleteCertificateWithPKeyFromPersonalStorage(String certificateAlias);

  @async
  List<Certificate> getInstalledCertificatesWithPrivateKeys();

  @async
  SuccessSign signFile({
    required String filePath,
    required String certificateAlias,
    required String password,
    required CAdESFormat format,
    bool signHash = false,
    bool isDetached = true,
    bool disableOnlineValidation = false,
    String? tsaUrl,
    String? storageName,
  });

  @async
  SuccessSign signMessage({
    required String message,
    required String certificateAlias,
    required String password,
    required CAdESFormat format,
    bool isDetached = true,
    bool signHash = false,
    bool disableOnlineValidation = false,
    String? tsaUrl,
    String? storageName,
  });
}

class CryptoProLicenseInfo {
  CryptoProLicenseInfo({
    required this.serialNumber,
    required this.expiredThroughDays,
    required this.licenseInstallDate,
    required this.existingLicenseStatus,
  });

  final String? serialNumber;
  final int? expiredThroughDays;
  final String? licenseInstallDate;
  final int? existingLicenseStatus;
}

class Certificate {
  final String? alias;

  final String? owner;

  final String? issuer;

  final String? serialNumber;

  final String? algorithm;

  final int? version;

  final String? oid;

  final String? validFrom;

  final String? validTo;

  Certificate({
    required this.alias,
    required this.algorithm,
    required this.issuer,
    required this.oid,
    required this.owner,
    required this.serialNumber,
    required this.validFrom,
    required this.validTo,
    required this.version,
  });
}

class SuccessSign {
  SuccessSign(this.base64);

  final String base64;
}

enum CAdESFormat {
  // ignore: constant_identifier_names
  BES,
  // ignore: constant_identifier_names
  XLongType1,
}
