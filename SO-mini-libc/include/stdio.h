/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef __STDIO_H__
#define __STDIO_H__	1

#ifdef __cplusplus
extern "C" {
#endif

/* Add puts() declaration. */
#include <internal/types.h>
#include <string.h>

int puts(const char *str);

#ifdef __cplusplus
}
#endif

#endif
