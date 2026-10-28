# S10 shard 3: selected WOW keyboard thunk provider and buffer boundaries

Read-only audit of selected pending `wow32/wkbman.c`, SHA-256 `D5D728A2E7527814343BD2470BCE98CA9028796FF7E404C7FE7B57ABBCFE4367`, identical to both original inventory editions. This extends `keyboard-abi-role-review.md`; no guest rebuild, input injection or runtime test.

`wow16/drivers/keyboard/keyboard.asm` emits Win16 thunks; `wkbdtbl2.h:18-20,166-175` registers 11 corresponding `WKB32*` functions. The current cached WOW32 map lines 574-584 attributes those 11 functions to `wkbman.obj`. These source/map links establish a selected path, not execution or released binary identity. Native USER owns host keyboard mapping, layout and code-page operations; the original WOW thunk maps guest pointers, converts arguments and flushes outputs. IRQ1/controller state belongs to a different path.

Entry families: `ToAscii` maps keyboard state and a four-byte result, then invokes host `ToAscii`; `OemKeyScan`, `VkKeyScan`, `GetKeyboardType`, `MapVirtualKey` and `GetKBCodePage` pass scalar inputs, with `MapVirtualKey` copying the high diacritic marker into the 16-bit return. `AnsiToOem`/`OemToAnsi`, their Buff variants and `GetKeyNameText` map source/destination and flush. `FREE*` are no-ops in non-DEBUG original macros; pointer validity and consistency belong to `GetPModeVDMPointer` and `Sim32FlushVDMPointer`, not to the names of those macros.

Specific test boundaries: `ToAscii` obtains 256-byte keyboard state with `GETPSZPTR` even though its contents are binary; macro expansion in `wow32.h:557-561` currently maps an optional pointer rather than performing string parsing, but its debug logging uses string formatting. `AnsiToOem` and `OemToAnsi` flush `strlen(input)`; Buff versions flush the requested length or 0xffff if zero. The actual extent, terminator, invalid selector, page-edge and zero-length behavior depend on pointer/flush provider and host API contracts. No defect is asserted solely from these original patterns; validate actual provider, exact buffer contents and return semantics before claiming recovery. Also separately test dead keys, active layouts/code pages, task focus and teardown.

The observed cached DLL hash and installed DLL hash differ as documented in `wow-dib-provider-lifecycle-review.md`; cached-map attribution alone does not establish installed-image identity. Do not report whole-file restored percentage from dispatch registration.

## Released-code binding follow-up

The independent [image observer](../shard-0/wow-keyboard-image-observer.py)
compares cached and released executable sections and binds all 11 mapped
original keyboard thunk bodies to release code with relocation words masked;
the [snapshot](../shard-0/wow-keyboard-image-observations.json) records the
object, map and image identities. This supports a **partial** file role, not
whole-file functionality: relocation targets, table dispatch, actual guest
buffers, layout/dead-key behavior, errors and teardown remain unproved.

Replay:

```powershell
Get-FileHash src/mvdm/wow32/wkbman.c -Algorithm SHA256
rg -n 'WKB32' src/mvdm/wow32/wkbdtbl2.h src/mvdm/wow32/wkbman.c
rg -n 'GETPSZPTR|GETMISCPTR|FLUSHVDMPTR|FREEPSZPTR|GetPModeVDMPointer' src/mvdm/wow32/wow32.h
rg -n 'wkbman.obj' build/.ninja/msvc-x86-release/wow32/wow32.dll.map
```
