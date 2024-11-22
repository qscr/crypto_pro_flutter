package com.a2soft.crypto_pro_flutter

import android.content.Context
import androidx.annotation.NonNull

import io.flutter.embedding.engine.plugins.FlutterPlugin
import io.flutter.plugin.common.MethodCall
import io.flutter.plugin.common.MethodChannel
import io.flutter.plugin.common.MethodChannel.MethodCallHandler
import io.flutter.plugin.common.MethodChannel.Result
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
    if (context != null) {
      try {
        when (call.method) {
          "initCSP" -> {
            result.success(CryptoProModule.getInstance().initCSP(context!!))
          }
          "addPfxCertificate" -> {
            val path = call.argument<String>("path")
            val password = call.argument<String>("password")
            if (path != null && password != null) {
              result.success(CryptoProModule.getInstance().addPfxCertificate(path, password, context!!).toString())
            } else {
              throw NullPointerException()
            }
          }
          "getInstalledCertificates" -> {
            result.success(CryptoProModule.getInstance().getInstalledCertificates().toString())
          }
          "deletePfxCertificate" -> {
            val alias = call.argument<String>("alias")
            if (alias != null) {
              result.success(CryptoProModule.getInstance().deletePfxCertificate(alias).toString())
            } else {
              throw NullPointerException()
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
            val formatEnum = format?.let { CAdESFormat.valueOf(it) }
            if (path != null && password != null && alias != null && isDetached != null && disableOnlineValidation != null && formatEnum != null) {
              CoroutineScope(Dispatchers.IO).launch {
                try {
                  result.success(CryptoProModule.getInstance().signFile(path, alias, password, isDetached, disableOnlineValidation, formatEnum, tsaUrl).toString())
                } catch (e: Exception) {
                // Обрабатываем ошибки
                result.error("error", e.message, e)
                }
              }
            } else {
              throw NullPointerException()
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
            val formatEnum = format?.let { CAdESFormat.valueOf(it) }
            if (message != null && password != null && alias != null && isDetached != null && signHash != null && disableOnlineValidation != null && formatEnum != null) {
              CoroutineScope(Dispatchers.IO).launch {
                try {
                  result.success(CryptoProModule.getInstance().signMessage(message, alias, password, isDetached, signHash, disableOnlineValidation, formatEnum, tsaUrl).toString())
                } catch (e: Exception) {
                  // Обрабатываем ошибки
                  result.error("error", e.message, e)
                }
              }
            } else {
              throw NullPointerException()
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
