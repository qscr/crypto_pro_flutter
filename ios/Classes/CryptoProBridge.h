#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface CryptoProBridge : NSObject
- (NSDictionary * _Nullable)getLicenseInfo:(NSError **)error;
- (NSDictionary * _Nullable)setLicense:(NSString *)number error:(NSError **)error;
- (NSMutableArray<NSDictionary *> * _Nullable)importPFXContainer:(NSString *)filePath
         password:(NSString *)password
         newPassword:(NSString * _Nullable)newPassword
         error:(NSError **)error;
- (BOOL)addCertificatesToTrustedStore:(NSArray<NSString *> *)certificatesPaths error:(NSError **)error;
- (BOOL)deleteCertificateFromPersonalStore:(NSString *)certificateAlias error:(NSError **)error;
- (NSMutableArray<NSDictionary *> * _Nullable)getInstalledCertificatesWithPrivateKey:(NSError **)error;
- (NSString * _Nullable) sign:(NSString *)certificateAlias
                contentToSign:(NSString * _Nullable)contentToSign
            contentToSignPath:(NSString * _Nullable)contentToSignPath
                     detached:(BOOL)detached
                     signHash:(BOOL)signHash
      disableOnlineValidation:(BOOL)disableOnlineValidation
                  cadesFormat:(NSInteger)cadesFormat
                  storageName:(NSString * _Nullable)storageName
                     password:(NSString *)password
                       tsaUrl:(NSString * _Nullable)tsaUrl
                        error:(NSError **)error;
@end

NS_ASSUME_NONNULL_END
