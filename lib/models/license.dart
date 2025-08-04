class CryptoProLicenseInfo {
  CryptoProLicenseInfo({
    required this.serialNumber,
    required this.expiredThroughDays,
    required this.licenseInstallDate,
    required this.existingLicenseStatus,
  });

  late final String? serialNumber;
  late final int? expiredThroughDays;
  late final String? licenseInstallDate;
  late final int? existingLicenseStatus;

  CryptoProLicenseInfo.fromMap(Map<String, dynamic> json) {
    serialNumber = json["serialNumber"];
    expiredThroughDays = json["expiredThroughDays"];
    licenseInstallDate = json["licenseInstallDate"];
    existingLicenseStatus = json["existingLicenseStatus"];
  }
}
