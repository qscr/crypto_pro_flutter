import 'package:flutter/services.dart';

class NoPrivateKeyFoundException extends PlatformException {
  NoPrivateKeyFoundException({
    required super.code,
    super.details,
    super.message,
    super.stacktrace,
  });
}

class WrongPasswordException extends PlatformException {
  WrongPasswordException({
    required super.code,
    super.details,
    super.message,
    super.stacktrace,
  });
}

class ArgumentsParsingException extends PlatformException {
  ArgumentsParsingException({
    required super.code,
    super.details,
    super.message,
    super.stacktrace,
  });
}

class SomeCertificatesAreNotAddedToTrustStoreException
    extends PlatformException {
  SomeCertificatesAreNotAddedToTrustStoreException({
    required super.code,
    super.details,
    super.message,
    super.stacktrace,
  });
}

class CertificateStatusUnknownOrRevokedException extends PlatformException {
  CertificateStatusUnknownOrRevokedException({
    required super.code,
    super.details,
    super.message,
    super.stacktrace,
  });
}

class GetCertificateFromContainerException extends PlatformException {
  GetCertificateFromContainerException({
    required super.code,
    super.details,
    super.message,
    super.stacktrace,
  });
}

class GetCertificatePrivateKeyException extends PlatformException {
  GetCertificatePrivateKeyException({
    required super.code,
    super.details,
    super.message,
    super.stacktrace,
  });
}

class ReadSignatureFromStreamException extends PlatformException {
  ReadSignatureFromStreamException({
    required super.code,
    super.details,
    super.message,
    super.stacktrace,
  });
}
