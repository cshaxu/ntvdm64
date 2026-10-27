/* Compile-only check against the selected original declarations and types.
 * No substitute provider, mocked typedef or function-pointer cast is used. */
#include <insignia.h>
#include <host_def.h>
#include <xt.h>
#include <cbw.h>
#include <cwde.h>
#include <cwd.h>
#include <cdq.h>
#include <lahf.h>
#include <sahf.h>
#include <stc.h>
#include <clc.h>
#include <cmc.h>
#include <std.h>
#include <cld.h>
#include <add.h>
#include <adc.h>
#include <and.h>
#include <cmp.h>
#include <dec.h>
#include <inc.h>
#include <neg.h>
#include <or.h>
#include <sbb.h>
#include <sub.h>
#include <test.h>
#include <xor.h>

typedef char ccpu_u32_is_four_bytes[sizeof(IU32) == 4 ? 1 : -1];
typedef char ccpu_size_is_one_byte[sizeof(IUM8) == 1 ? 1 : -1];
typedef char ccpu_pointer_is_four_bytes[sizeof(void *) == 4 ? 1 : -1];

typedef void (__cdecl *zero_argument)(void);
typedef void (__cdecl *binary_write)(IU32 *, IU32, IUM8);
typedef void (__cdecl *binary_read)(IU32, IU32, IUM8);
typedef void (__cdecl *unary_write)(IU32 *, IUM8);

zero_argument const ccpu_leaf_zero[] = {
    CBW, CWDE, CWD, CDQ, LAHF, SAHF, STC, CLC, CMC, STD, CLD
};
binary_write const ccpu_leaf_binary_write[] = {
    ADD, ADC, AND, OR, SBB, SUB, XOR
};
binary_read const ccpu_leaf_binary_read[] = { CMP, TEST };
unary_write const ccpu_leaf_unary_write[] = { DEC, INC, NEG };

#ifdef CPU_LEAF_ABI_NEGATIVE
/* Deliberately incompatible first argument must be diagnosed, not cast away. */
binary_read const ccpu_leaf_wrong_add = ADD;
#endif
