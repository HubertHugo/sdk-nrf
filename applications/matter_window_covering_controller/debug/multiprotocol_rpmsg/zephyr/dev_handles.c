#include <zephyr/device.h>
#include <zephyr/toolchain.h>

/* 1 : /soc/clock@41005000:
 */
extern const Z_DECL_ALIGN(device_handle_t) __attribute__((__section__(".__device_handles_pass2"))) __devicehdl_dts_ord_32[3];
const Z_DECL_ALIGN(device_handle_t) __attribute__((__section__(".__device_handles_pass2")))
__devicehdl_dts_ord_32[] = { DEVICE_HANDLE_SEP, DEVICE_HANDLE_SEP, DEVICE_HANDLE_ENDS };

/* 2 : /soc/gpio@418c0800:
 */
extern const Z_DECL_ALIGN(device_handle_t) __attribute__((__section__(".__device_handles_pass2"))) __devicehdl_dts_ord_7[3];
const Z_DECL_ALIGN(device_handle_t) __attribute__((__section__(".__device_handles_pass2")))
__devicehdl_dts_ord_7[] = { DEVICE_HANDLE_SEP, DEVICE_HANDLE_SEP, DEVICE_HANDLE_ENDS };

/* 3 : /soc/gpio@418c0500:
 */
extern const Z_DECL_ALIGN(device_handle_t) __attribute__((__section__(".__device_handles_pass2"))) __devicehdl_dts_ord_6[3];
const Z_DECL_ALIGN(device_handle_t) __attribute__((__section__(".__device_handles_pass2")))
__devicehdl_dts_ord_6[] = { DEVICE_HANDLE_SEP, DEVICE_HANDLE_SEP, DEVICE_HANDLE_ENDS };

/* 4 : /soc/random@41009000:
 */
extern const Z_DECL_ALIGN(device_handle_t) __attribute__((__section__(".__device_handles_pass2"))) __devicehdl_dts_ord_41[3];
const Z_DECL_ALIGN(device_handle_t) __attribute__((__section__(".__device_handles_pass2")))
__devicehdl_dts_ord_41[] = { DEVICE_HANDLE_SEP, DEVICE_HANDLE_SEP, DEVICE_HANDLE_ENDS };

/* 5 : /ipc/ipc0:
 * Direct Dependencies:
 *    - /soc/mbox@41012000
 */
extern const Z_DECL_ALIGN(device_handle_t) __attribute__((__section__(".__device_handles_pass2"))) __devicehdl_dts_ord_20[4];
const Z_DECL_ALIGN(device_handle_t) __attribute__((__section__(".__device_handles_pass2")))
__devicehdl_dts_ord_20[] = { 6, DEVICE_HANDLE_SEP, DEVICE_HANDLE_SEP, DEVICE_HANDLE_ENDS };

/* 6 : /soc/mbox@41012000:
 * Supported:
 *    - /ipc/ipc0
 */
extern const Z_DECL_ALIGN(device_handle_t) __attribute__((__section__(".__device_handles_pass2"))) __devicehdl_dts_ord_19[4];
const Z_DECL_ALIGN(device_handle_t) __attribute__((__section__(".__device_handles_pass2")))
__devicehdl_dts_ord_19[] = { DEVICE_HANDLE_SEP, DEVICE_HANDLE_SEP, 5, DEVICE_HANDLE_ENDS };

/* 7 : /soc/temp@41010000:
 */
extern const Z_DECL_ALIGN(device_handle_t) __attribute__((__section__(".__device_handles_pass2"))) __devicehdl_dts_ord_49[3];
const Z_DECL_ALIGN(device_handle_t) __attribute__((__section__(".__device_handles_pass2")))
__devicehdl_dts_ord_49[] = { DEVICE_HANDLE_SEP, DEVICE_HANDLE_SEP, DEVICE_HANDLE_ENDS };
