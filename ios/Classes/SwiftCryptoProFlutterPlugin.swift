import Flutter
import UIKit

public class SwiftCryptoProFlutterPlugin: NSObject, FlutterPlugin {
  public static func register(with registrar: FlutterPluginRegistrar) {
    let channel = FlutterMethodChannel(name: "crypto_pro_flutter", binaryMessenger: registrar.messenger())
    let instance = SwiftCryptoProFlutterPlugin()
    registrar.addMethodCallDelegate(instance, channel: channel)
  }

  public func handle(_ call: FlutterMethodCall, result: @escaping FlutterResult) {
      let args = call.arguments as? Dictionary<String, Any>;
      
      if (call.method == "initCSP") {
          let response = initCSP();
          result(response);
      }
      
      if (call.method == "addPfxCertificate") {
          let path = args?["path"] as? String;
          let password = args?["password"] as? String;
          let response = addCertificate(path, password);
          result(response);
      }
      
      if (call.method == "getInstalledCertificates") {
          result(getInstalledCertificate())
      }
      
      if (call.method == "signMessage") {
          let _message = args?["message"] as? String;
          let _alias = args?["alias"] as? String;
          let _password = args?["password"] as? String;
          let isDetached = (args?["isDetached"]) as? Bool;
          let signHash = (args?["signHash"]) as? Bool;
          result(signMessage(_message, _alias, _password, isDetached ?? true, signHash ?? true))
      }
      
      result(FlutterMethodNotImplemented);
  }
}
