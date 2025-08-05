package com.a2soft.crypto_pro_flutter

import android.content.Context
import androidx.annotation.NonNull
import com.a2soft.crypto_pro_flutter.exceptions.ArgumentsParsingException
import com.a2soft.crypto_pro_flutter.exceptions.BaseCustomCadesException
import com.a2soft.crypto_pro_flutter.exceptions.CryptoProFlutterBaseException

import io.flutter.embedding.engine.plugins.FlutterPlugin
import io.flutter.plugin.common.MethodCall
import io.flutter.plugin.common.MethodChannel
import io.flutter.plugin.common.MethodChannel.MethodCallHandler
import io.flutter.plugin.common.MethodChannel.Result
import kotlinx.coroutines.CoroutineExceptionHandler
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch

/** CryptoProFlutterPlugin */
class CryptoProFlutterPlugin: FlutterPlugin, MethodCallHandler {
  /// The MethodChannel that will the communication between Flutter and native Android
  ///
  /// This local reference serves to register the plugin with the Flutter Engine and unregister it
  /// when the Flutter Engine is detached from the Activity
  private lateinit var channel : MethodChannel
  private var context: Context? = null

  override fun onAttachedToEngine(@NonNull flutterPluginBinding: FlutterPlugin.FlutterPluginBinding) {
    channel = MethodChannel(flutterPluginBinding.binaryMessenger, "crypto_pro_flutter")
    channel.setMethodCallHandler(this)
    context = flutterPluginBinding.applicationContext
  }

  override fun onMethodCall(@NonNull call: MethodCall, @NonNull result: Result) {
    val exceptionHandler = CoroutineExceptionHandler { _, e ->
      if (e is CryptoProFlutterBaseException) {
        result.error(e.code, e.message, e.stackTraceToString())
        return@CoroutineExceptionHandler
      }

      if (e is BaseCustomCadesException) {
        result.error(e.code, e.toString(), null)
        return@CoroutineExceptionHandler
      }

      result.error(e.javaClass.name, e.message, e.stackTraceToString())
    }
    val scope = CoroutineScope(Dispatchers.IO + exceptionHandler)
    val instance = CryptoProModule.getInstance()
    if (context != null) {
      try {
        when (call.method) {
          "initCSP" -> {
            scope.launch {
              result.success(instance.initCSP(context!!))
            }
          }
          "setLicense" -> {
            val number = call.argument<String>("number")
            if (number != null) {
              scope.launch {
                result.success(instance.setLicense(number))
              }
            } else {
              throw ArgumentsParsingException()
            }
          }
          "getLicense" -> {
            scope.launch {
              result.success(instance.getLicense().toString())
            }
          }
          "addPfxCertificate" -> {
            val path = call.argument<String>("path")
            val password = call.argument<String>("password")
            val newPassword = call.argument<String?>("newPassword")
            if (path != null && password != null) {
              scope.launch {
                result.success(instance.addPfxCertificate(path, password, newPassword).toString())
              }
            } else {
              throw ArgumentsParsingException()
            }
          }
          "addFromExternalStorage" -> {
            val storageName = call.argument<String>("storageName")
            val password = call.argument<String>("password")
            val newPassword = call.argument<String?>("newPassword")
            if (storageName != null && password != null) {
              scope.launch {
                result.success(instance.addFromExternalStorage(storageName, password, newPassword).toString())
              }
            } else {
              throw ArgumentsParsingException()
            }
          }
          "readFromExternalStorage" -> {
            val storageName = call.argument<String>("storageName")
            val password = call.argument<String>("password")
            if (storageName != null && password != null) {
              scope.launch {
                result.success(instance.readFromExternalStorage(storageName, password).toString())
              }
            } else {
              throw ArgumentsParsingException()
            }
          }
          "getPrivateKeyFromInternalContainerByAlias" -> {
            val certificateAlias = call.argument<String>("alias")
            val password = call.argument<String>("password")
            if (certificateAlias != null && password != null) {
              scope.launch {
                result.success(instance.getPrivateKeyFromInternalContainerByAlias(certificateAlias, password).toString())
              }
            } else {
              throw ArgumentsParsingException()
            }
          }
          "removeCertificateWithPrivateKeyFromStorage" -> {
            val certificateAlias = call.argument<String>("alias")
            if (certificateAlias != null) {
              scope.launch {
                result.success(instance.removeCertificateWithPrivateKeyFromStorage(certificateAlias).toString())
              }
            } else {
              throw ArgumentsParsingException()
            }
          }
          "addCertificatesToTrustedStorage" -> {
            val paths = call.argument<List<String>>("paths")
            if (paths != null) {
              scope.launch {
                instance.addCertificatesToTrustedStorage(paths)
                result.success("Сертификаты успешно добавлены")
              }
            } else {
              throw ArgumentsParsingException()
            }
          }
          "getInstalledCertificates" -> {
            scope.launch {
              result.success(CryptoProModule.getInstance().getInstalledCertificates().toString())
            }
          }
          "deletePfxCertificate" -> {
            val alias = call.argument<String>("alias")
            if (alias != null) {
              scope.launch {
                result.success(CryptoProModule.getInstance().deleteCertificateFromInternalStorage(alias).toString())
              }
            } else {
              throw ArgumentsParsingException()
            }
          }
          "signFile" -> {
            val path = call.argument<String>("path")
            val alias = call.argument<String>("alias")
            val password = call.argument<String>("password")
            val isDetached = call.argument<Boolean>("isDetached")
            val disableOnlineValidation = call.argument<Boolean>("disableOnlineValidation")
            val tsaUrl = call.argument<String>("tsaUrl")
            val format = call.argument<String>("format")
            val storageName = call.argument<String>("storageName")
            val formatEnum = format?.let { CAdESFormat.valueOf(it) }
            if (path != null && password != null && alias != null && isDetached != null && disableOnlineValidation != null && formatEnum != null) {
              scope.launch {
                result.success(CryptoProModule.getInstance().signFile(path, alias, password, isDetached, disableOnlineValidation, formatEnum, storageName, tsaUrl).toString())
              }
            } else {
              throw ArgumentsParsingException()
            }
          }
          "signMessage" -> {
            val message = call.argument<String>("message")
            val alias = call.argument<String>("alias")
            val password = call.argument<String>("password")
            val isDetached = call.argument<Boolean>("isDetached")
            val signHash = call.argument<Boolean>("signHash")
            val disableOnlineValidation = call.argument<Boolean>("disableOnlineValidation")
            val tsaUrl = call.argument<String>("tsaUrl")
            val format = call.argument<String>("format")
            val storageName = call.argument<String>("storageName")
            val formatEnum = format?.let { CAdESFormat.valueOf(it) }
            if (message != null && password != null && alias != null && isDetached != null && signHash != null && disableOnlineValidation != null && formatEnum != null) {
              scope.launch {
                result.success(CryptoProModule.getInstance().signMessage(message, alias, password, isDetached, signHash, disableOnlineValidation, formatEnum, storageName, tsaUrl).toString())
              }
            } else {
              throw ArgumentsParsingException()
            }
          }
        }
      } catch (e: Exception) {
        result.error("error", e.message, e);
      }
    } else {
      result.error("nullContext", "Не удалось получить контекст", null)
    }
  }

  override fun onDetachedFromEngine(@NonNull binding: FlutterPlugin.FlutterPluginBinding) {
    channel.setMethodCallHandler(null)
  }
}
