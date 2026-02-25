#import <Foundation/Foundation.h>

OBJC_EXPORT
@interface CSPBundle : NSObject

@property NSBundle *bundle;

+ (instancetype)sharedInstance;

@end
