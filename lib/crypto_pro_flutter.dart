import 'dart:io';

import 'package:crypto_pro_flutter/crypto_pro_api.g.dart';
import 'package:crypto_pro_flutter/utils/exception_handler_mixin.dart';

class CryptoProFlutter with ExceptionHandlerMixin {
  final CryptoProFlutterApi _api;

  CryptoProFlutter() : _api = CryptoProFlutterApi();

  CryptoProFlutter.createForTest(CryptoProFlutterApi api) : _api = api;

  /// Initializes the cryptographic provider.
  ///
  /// - **Android:**
  ///   This method **must be called before any other API method** of the plugin.
  ///   It performs initialization of the CryptoPro provider.
  ///   Failing to call this method before using other functionality may result
  ///   in unexpected errors.
  ///
  /// - **iOS:**
  ///   This method does not perform any actual initialization and simply
  ///   returns `true`.
  Future<bool> initCSP() => callWithHandler(() => _api.initCSP());

  /// Get info about Crypto Pro CSP license
  ///
  /// Returns null if license doesn't installed
  Future<CryptoProLicenseInfo?> getLicense() =>
      callWithHandler(() => _api.getLicense());

  /// Installs new license
  ///
  /// Returns operation result
  Future<bool> setLicense(String number) =>
      callWithHandler(() => _api.setLicense(number));

  /// Imports certificates into the trusted certificate store.
  ///
  /// Installs certificates that do not contain private keys
  /// into the trusted storage.
  ///
  /// Each file in [paths] must contain a valid X.509 certificate
  /// (for example, in DER or PEM format, p7b container supported too).
  ///
  /// Parameters:
  /// - [paths] - a list of absolute file paths to certificate files.
  ///
  /// Throws:
  /// - [SomeCertificatesAreNotAddedToTrustStoreException] if one or more
  ///   certificates could not be added to the trusted store.
  /// - Other platform-specific exceptions.
  Future<void> addCertificatesToTrustedStorage({
    required List<String> paths,
  }) =>
      callWithHandler(
        () => _api.addCertificatesToTrustedStorage(
          paths: paths,
        ),
      );

  /// Imports a container from external storage (Android only).
  ///
  /// Works similarly to [importPFXContainer], but imports
  /// a cryptographic container located in external storage,
  /// such as a hardware token (e.g., Rutoken), into the personal storage.
  Future<List<Certificate>> importContainerFromExternalStorage({
    required String storageName,
    required String password,
    String? newPassword,
  }) =>
      callWithHandler(
        () => _api.importContainerFromExternalStorage(
          storageName: storageName,
          password: password,
          newPassword: newPassword,
        ),
      );

  /// Reads certificates with private keys from an external container
  /// (Android only).
  ///
  /// This method accesses an external cryptographic container
  /// (e.g., a hardware token such as Rutoken) using [storageName]
  /// and [password], and returns the certificates that contain
  /// associated private keys.
  ///
  /// - The certificates are NOT imported into the application's
  ///   personal storage.
  /// - No private key containers are copied.
  /// - The method only reads and returns metadata about the certificates.
  ///
  /// Parameters:
  /// - [storageName] - name or identifier of the external container.
  /// - [password] - password used to access the external container.
  ///
  /// Returns:
  /// A list of [Certificate] objects representing certificates
  /// with private keys found in the external container.
  ///
  /// Throws:
  /// - [WrongPasswordException] if the container password is invalid.
  /// - Other platform-specific exceptions.
  Future<List<Certificate>> readContainerFromExternalStorage({
    required String storageName,
    required String password,
  }) =>
      callWithHandler(
        () => _api.readContainerFromExternalStorage(
          storageName: storageName,
          password: password,
        ),
      );

  /// Returns a certificate with its associated private key
  /// from the personal storage.
  ///
  /// This method retrieves a certificate identified by [certificateAlias]
  /// from the personal container and validates access
  /// to its private key using the provided [password].
  ///
  /// Parameters:
  /// - [certificateAlias] - alias of the certificate in the personal store.
  /// - [password] - password used to open the private key container.
  ///
  /// Returns:
  /// A [Certificate] object representing the certificate with an accessible
  /// private key.
  ///
  /// Throws:
  /// - [WrongPasswordException] if the private key password is invalid.
  /// - [NoPrivateKeyFoundException] if the certificate does not contain
  ///   a private key.
  /// - Other platform-specific exceptions.
  Future<Certificate> getCertificateWithPrivateKeyFromPersonalContainer({
    required String certificateAlias,
    required String password,
  }) =>
      callWithHandler(
        () => _api.getCertificateWithPrivateKeyFromPersonalContainer(
          certificateAlias: certificateAlias,
          password: password,
        ),
      );

  /// Imports a PFX container into the application certificate storages.
  ///
  /// This method performs a full import of certificates contained in the
  /// specified PFX file:
  ///
  /// - Certificates that do NOT contain private keys are installed into
  ///   the application's trusted certificate store
  /// - Certificates that DO contain private keys are installed into the
  ///   personal store. The corresponding private key container
  ///   is also copied into the internal storage.
  ///
  /// The private key container is protected with:
  /// - the original PFX password if [newPassword] is not provided, or
  /// - the [newPassword] value if specified.
  ///
  /// The method may return multiple certificates if the PFX file contains
  /// several entries with private keys.
  ///
  /// Parameters:
  /// - [file] - Absolute path to the PFX (PKCS#12) file.
  /// - [password] - password used to open the PFX file.
  /// - [newPassword] - optional new password that will be set for the imported
  ///   private key container. If not provided, the original PFX password
  ///   will be used.
  ///
  /// Returns:
  /// A list of [Certificate] objects representing certificates that contain
  /// private keys and were successfully imported into the personal store.
  ///
  /// Throws:
  /// - [WrongPasswordException] if the PFX password is invalid.
  /// - Other platform-specific exceptions.
  Future<List<Certificate>> importPFXContainer({
    required File file,
    required String password,
    String? newPassword,
  }) =>
      callWithHandler(
        () => _api.importPFXContainer(
          filePath: file.path,
          password: password,
          newPassword: newPassword,
        ),
      );

  /// Deletes a certificate and its associated private key
  /// from the personal storage.
  ///
  /// This method removes:
  /// - The certificate with [certificateAlias].
  /// - The corresponding private key container from the internal storage.
  ///
  /// Parameters:
  /// - [certificateAlias] - alias of the certificate
  Future<void> deleteCertificateWithPKeyFromPersonalStorage(
    String certificateAlias,
  ) =>
      callWithHandler(
        () =>
            _api.deleteCertificateWithPKeyFromPersonalStorage(certificateAlias),
      );

  /// Returns a list of certificates with private keys
  /// installed in the personal storage.
  ///
  /// This method retrieves only certificates that have an associated
  /// private key and are stored in the personal container.
  ///
  /// Returns:
  /// A list of [Certificate] objects representing installed certificates
  /// with private keys.
  Future<List<Certificate>> getInstalledCertificatesWithPrivateKeys() =>
      callWithHandler(
        () => _api.getInstalledCertificatesWithPrivateKeys(),
      );

  /// Signs a file using the specified certificate.
  ///
  /// The file is signed using the certificate identified by [certificateAlias]
  /// and protected with [password].
  ///
  /// Parameters:
  /// - [file] - file to be signed.
  /// - [certificateAlias] - alias of the certificate in the storage.
  /// - [password] - password for accessing the private key container.
  /// - [format] - CAdES signature format.
  /// - [signHash] - if `true`, the input file is treated as a precomputed hash
  ///   instead of raw content.
  /// - [isDetached] - if `true`, creates a detached signature.
  /// - [disableOnlineValidation] - if `true`, disables online certificate
  ///   status validation during signing. Ignored on iOS.
  /// - [tsaUrl] - optional Time Stamping Authority (TSA) URL for creating
  ///   XLongType1 signature.
  /// - [storageName] - specifies the name of an external container (e.g., hardware token)
  ///   to use for signing instead of the internal personal storage.
  ///   Ignored on iOS.
  ///
  /// Returns:
  /// A [SuccessSign] object containing the Base64-encoded signature.
  ///
  /// Throws:
  /// - [WrongPasswordException] if the private key password is invalid.
  /// - [NoPrivateKeyFoundException] if the certificate has no accessible private key.
  /// - Other platform-specific exceptions.
  Future<SuccessSign> signFile({
    required File file,
    required String certificateAlias,
    required String password,
    required CAdESFormat format,
    bool signHash = false,
    bool isDetached = true,
    bool disableOnlineValidation = false,
    String? tsaUrl,
    String? storageName,
  }) =>
      callWithHandler(
        () => _api.signFile(
          filePath: file.path,
          certificateAlias: certificateAlias,
          password: password,
          format: format,
          signHash: signHash,
          isDetached: isDetached,
          disableOnlineValidation: disableOnlineValidation,
          tsaUrl: tsaUrl,
          storageName: storageName,
        ),
      );

  /// Signs a string message using the specified certificate.
  ///
  /// The message is signed using the certificate identified by [certificateAlias]
  /// and protected with [password].
  ///
  /// Parameters:
  /// - [message] - message to be signed.
  /// - [certificateAlias] - alias of the certificate in the storage.
  /// - [password] - password for accessing the private key container.
  /// - [format] - CAdES signature format.
  /// - [signHash] - if `true`, the input file is treated as a precomputed hash
  ///   instead of raw content.
  /// - [isDetached] - if `true`, creates a detached signature.
  /// - [disableOnlineValidation] - if `true`, disables online certificate
  ///   status validation during signing. Ignored on iOS.
  /// - [tsaUrl] - optional Time Stamping Authority (TSA) URL for creating
  ///   XLongType1 signature.
  /// - [storageName] - specifies the name of an external container (e.g., hardware token)
  ///   to use for signing instead of the internal personal storage.
  ///   Ignored on iOS.
  ///
  /// Returns:
  /// A [SuccessSign] object containing the Base64-encoded signature.
  ///
  /// Throws:
  /// - [WrongPasswordException] if the private key password is invalid.
  /// - [NoPrivateKeyFoundException] if the certificate has no accessible private key.
  /// - Other platform-specific exceptions.
  Future<SuccessSign> signMessage({
    required String message,
    required String certificateAlias,
    required String password,
    required CAdESFormat format,
    bool isDetached = true,
    bool signHash = false,
    bool disableOnlineValidation = false,
    String? tsaUrl,
    String? storageName,
  }) =>
      callWithHandler(
        () => _api.signMessage(
          message: message,
          certificateAlias: certificateAlias,
          password: password,
          format: format,
          isDetached: isDetached,
          signHash: signHash,
          disableOnlineValidation: disableOnlineValidation,
          tsaUrl: tsaUrl,
          storageName: storageName,
        ),
      );
}
