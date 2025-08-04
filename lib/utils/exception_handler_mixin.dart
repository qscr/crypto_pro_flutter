import 'dart:async';

import 'package:crypto_pro_flutter/models/exceptions.dart';
import 'package:flutter/services.dart';

mixin ExceptionHandlerMixin {
  Future<T> callWithHandler<T>(FutureOr<T> Function() func) async {
    try {
      return await func();
    } on PlatformException catch (e, stack) {
      switch (e.code) {
        case "1":
          Error.throwWithStackTrace(
            NoPrivateKeyFoundException(
              code: e.code,
              message: e.message,
              stacktrace: e.stacktrace,
              details: e.details,
            ),
            stack,
          );
        case "2":
          Error.throwWithStackTrace(
            WrongPasswordException(
              code: e.code,
              message: e.message,
              stacktrace: e.stacktrace,
              details: e.details,
            ),
            stack,
          );
        case "3":
          Error.throwWithStackTrace(
            ArgumentsParsingException(
              code: e.code,
              message: e.message,
              stacktrace: e.stacktrace,
              details: e.details,
            ),
            stack,
          );
        case "4":
          Error.throwWithStackTrace(
            SomeCertificatesAreNotAddedToTrustStoreException(
              code: e.code,
              message: e.message,
              stacktrace: e.stacktrace,
              details: e.details,
            ),
            stack,
          );
        case "5":
          Error.throwWithStackTrace(
            CertificateStatusUnknownOrRevokedException(
              code: e.code,
              message: e.message,
              stacktrace: e.stacktrace,
              details: e.details,
            ),
            stack,
          );
        case "6":
          Error.throwWithStackTrace(
            GetCertificateFromContainerException(
              code: e.code,
              message: e.message,
              stacktrace: e.stacktrace,
              details: e.details,
            ),
            stack,
          );
        case "7":
          Error.throwWithStackTrace(
            GetCertificatePrivateKeyException(
              code: e.code,
              message: e.message,
              stacktrace: e.stacktrace,
              details: e.details,
            ),
            stack,
          );
        case "8":
          Error.throwWithStackTrace(
            ReadSignatureFromStreamException(
              code: e.code,
              message: e.message,
              stacktrace: e.stacktrace,
              details: e.details,
            ),
            stack,
          );
        case "8":
          Error.throwWithStackTrace(
            ReadSignatureFromStreamException(
              code: e.code,
              message: e.message,
              stacktrace: e.stacktrace,
              details: e.details,
            ),
            stack,
          );
        case "9":
          Error.throwWithStackTrace(
            CertificateChainException(
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
