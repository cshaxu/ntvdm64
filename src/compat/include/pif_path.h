#ifndef COMPAT_PIF_PATH_H
#define COMPAT_PIF_PATH_H
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* In-place, bounded OEM-to-ANSI boundary for original PIF configuration
 * file consumers. Returns zero if conversion cannot fit; opens no file. */
int mvdm_softpc_config_path_to_ansi(char *path, uint32_t capacity);

#ifdef __cplusplus
}
#endif
#endif
