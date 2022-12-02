//
//  CryptoProModule.h
//  crypto_pro_flutter
//
//  Created by Maxim Petrov on 25.11.2022.
//

#import <Foundation/Foundation.h>
#import "stdlib.h"
#import "stdio.h"

#ifdef __cplusplus
extern "C" {
#endif
bool initCSP(void);
NSString* addCertificate(NSString* path, NSString* password);
NSString* getInstalledCertificate();
NSString* signMessage(NSString* message, NSString *alias, NSString* password, bool isDetached, bool signHash);
NSString* digest(NSString* alias, NSString* password, NSString* message);
NSString* sign(NSString* alias, NSString* password, NSString* digest);

#ifdef __cplusplus
}
#endif

