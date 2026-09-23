# T422 S1 WOW32 unavailable-boundary disposition

## Question

Which historical dependencies may be reused through a finite standalone
binding, and which are explicit stopping boundaries rather than permission to
invent a replacement subsystem?

## Evidence base

- The selected 77-body manifest and ten original dispatch tables.
- Original non-MVDM USER source already admitted as finite `opennt-host`
  subsets, including desktop/client/cleanup/task/class/resource algorithms.
- [Dynamic host-surface audit](m0-t422-s1-dynamic-host-surface-audit.md),
  which resolves 143 selected dynamic exports on this x86 host.
- [Direct-data matrix](m0-t422-s1-direct-data-owner-matrix.md) and
  [bootstrap/teardown order](m0-t422-s1-wow-bootstrap-teardown-order.md).
- The project [source policy](../operations/policy/source-policy.md).

## Disposition matrix

| Dependency family | Original role | S1 disposition | Permitted receiving mechanism | Explicit prohibition / receiver |
| --- | --- | --- | --- | --- |
| Original WOW32 bodies | Win16 thunk, conversion and dispatch policy | Reuse selected body as-is; all 77 are already source-selected. | Original `src/mvdm/wow32` body plus finite ABI binding only at an unavailable edge. | No duplicated algorithm in a target-local facade; S2--S7 own runtime closure. |
| Original USER client/kernel/RTL slices | Layout, class, cleanup, resource and selected client algorithms | Reuse only the function-scoped already admitted source slices or further individually admitted original files. | `src/opennt-host` at original path, with a bounded worker/DLL binding. | No recursive `ntuser` server import; no CSRSS/Win32k server runtime. S2--S7 name each added file/range. |
| Guest-visible USER view | NT4 mapped desktop/client view and handle records | Required finite representation, source-shaped layouts/order. | One worker-local CCPU-visible backing producer paired with native object operations. | No host pointer/HWND/TEB in guest memory; no copied host desktop graph; S2. |
| USER server scheduling / queues | USER locks, task queues, callback transitions | Recover selected original task algorithms, bind one worker-local runtime to public modern queue wait where contract permits. | Existing single runtime/session binding and original taskman/queue slices. | No second scheduler, TLS owner, SMS list or USER server; S2. |
| NT kernel VDM / CPU30 monitor | Ring-0 trap/context/process mechanisms | Not composable for CPU40 standalone. | Existing CCPU40 worker, current finite guest-memory/page-domain ABI only. | No V86 monitor, trap frame, kernel thread/object or CPU30 revival; S2/S6 must not reintroduce it. |
| CSR/Base transport | Historical service/private transport | Not a WOW provider dependency to recreate. | Existing broker protocol where a selected product operation already has a finite record. | No CSR transport/client recreation inside WOW32; S7 CORE only consumes established worker lifetime. |
| Modern dynamic DLLs | Winsock, spool, multimedia and OLE dynamic imports | Static x86 availability proven: WSOCK32 49 ordinal, WINSPOOL 15 name, WINMM 78 name, OLETHK32 1 name. | Atomic load-and-resolve table with original call signatures and explicit failure before dispatch. | Debug-only assertion/success behavior is not allowed as a product result; S7 tests load failure/callback/teardown. |
| Native Registry | Original Shell and configuration readers/writers | Use shared worker-local layered/shadow Registry provider. | `NTVDM.REG` override then read-only host snapshot; mutable WOW Shell tree only through same provider, never system mutation. | No direct system Registry writes and no WOW-only duplicate parser; S7. |
| Real device/legacy providers | COMM, print, sound/multimedia, OLE or Winsock services | Preserve selected original provider-call/error semantics where public host facility exists. | Public host API or dynamically loaded original-named export behind one finite binding. | Do not fabricate a success provider for absent hardware/service. S7 records explicit negative/mock coverage where host capability is absent. |
| Immutable guest | USER.EXE/PMODE32 and WRITE/WINMINE/SOL | Read-only test subject and ABI consumer. | Normal guest runtime writes only. | No binary patch, recompile, hot patch or media replacement. Proven guest defects are TODO/non-pass only. |

## S1 result

No currently observed WOW bootstrap fault is classified as an original guest
defect: the proven `GetDesktopWindow` failure is an absent host-side client
view producer.  Therefore it is assigned to S2, not waived under the
immutable-guest policy.

The static host export result is also not a runtime pass.  S7 must turn each
dynamic table into an all-or-nothing load transaction, test an unavailable
library/export path without calling a null function pointer, and prove normal
callback/cleanup where the original family requires it.

For every future import the four-rung decision is fixed: first reuse the
directly composable original file/range; then retain it with the smallest
named adapter; then use the narrow registered exceptional carrier where the
source has no usable x86 boundary; author new behavior only after the prior
rungs are evidenced unavailable.  “Modern Windows has a similar API” is not
an equivalence conclusion.

## Follow-up

S1 A03 must attach function-level file/range, binding and deletion criteria to
each existing target-local operation.  S2--S7 may not bypass this matrix by
adding new generic compatibility layers.  A future original-guest limitation
is entered in indexed evidence and TODO with its reproducer, then remains a
non-pass; no additional owner approval is required by the standing policy.
