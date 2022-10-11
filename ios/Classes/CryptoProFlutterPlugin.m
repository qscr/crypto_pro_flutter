#import "CryptoProFlutterPlugin.h"
#if __has_include(<crypto_pro_flutter/crypto_pro_flutter-Swift.h>)
#import <crypto_pro_flutter/crypto_pro_flutter-Swift.h>
#else
// Support project import fallback if the generated compatibility header
// is not copied when this plugin is created as a library.
// https://forums.swift.org/t/swift-static-libraries-dont-copy-generated-objective-c-header/19816
#import "crypto_pro_flutter-Swift.h"
#endif

@implementation CryptoProFlutterPlugin
+ (void)registerWithRegistrar:(NSObject<FlutterPluginRegistrar>*)registrar {
  [SwiftCryptoProFlutterPlugin registerWithRegistrar:registrar];
}
@end
