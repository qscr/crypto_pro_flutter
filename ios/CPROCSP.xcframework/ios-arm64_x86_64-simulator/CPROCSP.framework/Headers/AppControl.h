//
//  AppControl.h
//  CPROCSP
//
//  Created by Ivan Yakovenko on 04.04.2024.
//

#ifndef AppControl_h
#define AppControl_h

#include <stdint.h>

typedef struct
{
    uint32_t size;
    uint8_t checksum[32];
} check_info;

#ifdef __cplusplus
extern "C" {
#endif

uint32_t register_checksum(const char *framework_name, const check_info *checksum_info);

#ifdef __cplusplus
}
#endif

#endif /* AppControl_h */
