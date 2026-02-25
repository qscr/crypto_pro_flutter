import 'dart:async';

import 'package:crypto_pro_flutter/models/exceptions.dart';
import 'package:flutter/services.dart';

mixin ExceptionHandlerMixin {
  Future<T> callWithHandler<T>(FutureOr<T> Function() func) async {
    try {
      return await func();
    } on PlatformException catch (e, stack) {
      switch (e.code) {
        case "NoPrivateKeyFound":
          Error.throwWithStackTrace(
            NoPrivateKeyFoundException(
              code: e.code,
              message: e.message,
              stacktrace: e.stacktrace,
              details: e.details,
            ),
            stack,
          );
        case "CustomWrongPasswordException":
          Error.throwWithStackTrace(
            WrongPasswordException(
              code: e.code,
              message: e.message,
              stacktrace: e.stacktrace,
              details: e.details,
            ),
            stack,
          );
        case "ArgumentsParsingException":
          Error.throwWithStackTrace(
            ArgumentsParsingException(
              code: e.code,
              message: e.message,
              stacktrace: e.stacktrace,
              details: e.details,
            ),
            stack,
          );
        case "SomeCertificatesAreNotAddedToTrustStoreException":
          Error.throwWithStackTrace(
            SomeCertificatesAreNotAddedToTrustStoreException(
              code: e.code,
              message: e.message,
              stacktrace: e.stacktrace,
              details: e.details,
            ),
            stack,
          );
        case "AddSignerCertificateStatusUnknownOrRevokedException":
          Error.throwWithStackTrace(
            CertificateStatusUnknownOrRevokedException(
              code: e.code,
              message: e.message,
              stacktrace: e.stacktrace,
              details: e.details,
            ),
            stack,
          );
        case "GetCertificateFromContainerException":
          Error.throwWithStackTrace(
            GetCertificateFromContainerException(
              code: e.code,
              message: e.message,
              stacktrace: e.stacktrace,
              details: e.details,
            ),
            stack,
          );
        case "GetCertificatePrivateKeyException":
          Error.throwWithStackTrace(
            GetCertificatePrivateKeyException(
              code: e.code,
              message: e.message,
              stacktrace: e.stacktrace,
              details: e.details,
            ),
            stack,
          );
        case "ReadSignatureFromStreamException":
          Error.throwWithStackTrace(
            ReadSignatureFromStreamException(
              code: e.code,
              message: e.message,
              stacktrace: e.stacktrace,
              details: e.details,
            ),
            stack,
          );
        case "AddSignerUnknownException":
          Error.throwWithStackTrace(
            AddSignerUnknownException(
              code: e.code,
              message: e.message,
              stacktrace: e.stacktrace,
              details: e.details,
            ),
            stack,
          );
        default:
          rethrow;
      }
    }
  }
}
