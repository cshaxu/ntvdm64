# T412 S1 original BaseSrv compilation boundary

## Scope and reproduction

`node tools/audit/Verify-BrokerOriginalCompilation.mjs` compiles the complete
selected `src/opennt-host/base/win32/server/srvvdm.c`, without changing its
function bodies. It uses the existing formal x86 CCPU40 graph's compiler flags,
MSVC 2022, and a test-only declaration boundary. All generated files remain in
`build/M0-T412/S1/base-owner`. No probe executable is published.

The tracked `tests/broker/source_compile_probe.h` is NOT an admitted product
ABI or a CSR implementation. It supplies original-shaped NT declarations,
replaces only the NT4 TEB request-thread accessor with an unresolved external,
and leaves the excluded original RTL inline unresolved. The unused DFS include
from the broad `basesrv.h` umbrella is empty only in this probe. Production
recovery must select a finite declaration boundary, not install that empty
header or copy an entire CSR hierarchy into the runtime.

## Observed result

Compilation succeeds with `/we4013`: no implicit function declaration is
accepted. The output is an x86 COFF object (machine 014c). The driver confirms
the owner source bytes are unchanged, records their SHA-256 and graph path,
and saves the complete object symbol table in `symbols.txt`. The 44 reported
UNDEF records include COFF common data, CRT/compiler helpers and external API
references; they are NOT 44 missing functions or 44 required implementations.

The direct attempt using only the existing product declarations failed on
PORT_MESSAGE, then private CSR umbrella declarations, then CsrClientThread in
the selected support TEB. Supplying the retained original LPC/CSR declarations
and a finite request accessor removes those syntax failures without editing
the original command-selection, queue, duplication or cleanup bodies.

## Link and integration disposition

- CRT and native NT event, token, loader and object APIs need correctly typed
  link bindings, not replacement algorithms. The original RTL LUID inline
  needs its original carrier restored; its external declaration here is only
  a compile probe and is not claimed to name an exported OS function.
- CsrLockProcessByClientId, CsrUnlockProcess and the request-thread accessor
  require an authenticated, lifetime-pinned process/request context. A pointer
  into a fabricated modern TEB is not acceptable.
- Original server heap/critical-section initialization must be selected in
  the broker composition. COFF common symbols alone do not prove this.
- UserNotifyProcessCreate and WOWEXEC registration/posting retain the distinct
  dispositions in the [owner audit](m0-t412-s1-broker-owner-audit.md). No success
  stub was supplied to make this compile test pass.
- BaseSrvDupStandardHandles and BaseSrvCreatePairWaitHandles perform target
  duplication at the original call site. A future OS-attachment binding must
  preserve that timing, target failure and rollback; postponing duplication
  until GetNextVDMCommand would change the policy. Attachment feasibility is
  [separately tested](m0-t412-s1-rpc-resource-feasibility.md), not integrated here.

This is a full-original-translation-unit compilation result, not a link,
original lifecycle test, three-program build, S1 closure or T412 completion.
The product binaries and their existing acceptance limits remain unchanged.
