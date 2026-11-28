# S15 P63 N022 state drawing migration

## Source and scope

Start2ec49dd06/runtime71. Fixed XP SP1 commit
 a2f6d7c93aa4f11efc51147566d5961c59a8ef89, original path
windows/core/ntuser/client/mngrayc.c, blob
a0107f672ad707b0c02448c26e71ce8af3d7405a. Prior OpenNT blob
4e8c72791e03e7eeb105d5b3fc9e139df70f42bf.

XP restores RTL layout and text alignment, relative viewport offsets,
ANSI conversion count and actual callback result. The obsolete ClientDrawState
wrapper is replaced by the XP DrawStateW entry. Production and fixtures bind
that local symbol explicitly rather than USER32's imported implementation.
Final x86 object symbols define _DrawStateW@40 and _DrawStateA@40 and reference
_wow_edit_native_SetLayoutWidth@12; no __imp_DrawStateW reference exists.
The existing native-controls provider resolves installed GDI32 SetLayoutWidth;
both System32 and SysWOW64 export it. XP xform.c lines70 onward forwards the
same HDC/LONG/DWORD contract to NtGdiSetLayout. Do not substitute SetLayout,
which lacks the width input. No copied GDI implementation or new state machine.

## Residual adaptation evidence

Two existing composition guards keep Button's atlas distinct from scoped
DrawState resources; the same original helper serves both compiled contexts.
Two original #if0 branches remain excluded. #line mappings are verified.
Retain the existing __finally release of the scoped critical section: the
real-GDI fixture raises from the callback, and the outer scope destroys the
lock/resources on unwinding. Returning with the lock owned violates that
lifetime. Retain bitmap-selection failure cleanup: the existing provider only
transfers bitmap ownership after successful SelectObject, so an unselected
new bitmap must be deleted by its allocating call, not leaked by a false
successful resize. No second retry or fallback path.

The scoped context exposes borrowed original oembmi/BitCount/hbrGray inputs;
no guest or window ownership is moved. BltColor flags become XP UINT.
The callback also checks actual viewport14,16 from existing3,4 plus offset11,12.
The existing draw-state test is updated from the OpenNT TRUE return expectation
to XP's callback result. Nested scopes and exception/borrowed-bitmap checks
remain required. Original guest binaries are unchanged.

## Census and verification

File1point/1185lines against OpenNT becomes9points/20lines against XP,
with two excluded original arms. LF recovery exposes separate small adapters;
no whole-file formatting diff remains. Global1329/788/799/530/8079/66627,
delta0/0/0/0/+8/-1165;1301XP/28OpenNT. Final verified results.
Cached full build, runtime72 dual13-image26hash publication, controls40/40,
real CCPU window-borrow, SOL Options and genuine Win16 controls pass.
[Receipt](n022-mngrayc-result.json), [census](n022-mngrayc-census.json).
Initial build found the missing USER_AWCONV_COUNTSTRINGSZ declaration.
Reused existing wow_char_conversion_abi.h; no mirror change or duplicate
constant. First attempted textual edit missed the compat-prefixed include;
second build therefore repeated the same diagnostic. The third build uses the
read-back-verified include. Both failure logs are retained; publication stopped
before staging.
N02234/39 delivered; P61 Full24/24 is current checkpoint.
