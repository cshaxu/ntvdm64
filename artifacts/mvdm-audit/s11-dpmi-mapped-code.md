# S11 DPMI instruction mapping: selected-CCPU counterexample

`DpmiFaultHandler` calls `DpmiEmulateInstruction` for #GP/#UD. The latter
uses `Sim32GetVDMPointer(CS:0)` and adds EIP to the resulting host pointer.
On selected CPU40, `Sim32pGetVDMPointer` resolves the selector, then
`c_GetLinAdd` translates only that base address; `c_GetPhyAdd` returns a
physical-storage pointer. The pointer plus EIP is not necessarily the
translation of guest linear base plus EIP. Original NT4's native VDM address
space instead supplies a linear process mapping.

The actual selected `dpmiint.obj`, `sim32.obj` and `ccpusas4.obj` are linked
to the CCPU monitor fixture. Case 93 maps CS base page F0000h normally but
remaps instruction linear F1000h to physical 6000h: the expected original
`8E E0` compatibility operation declines with EIP unchanged. Case 94 uses
the same selector and instruction at identity-mapped F1000h: it succeeds
and advances EIP. Case 95 places `8E` at F0FFFh and `E0` at the first byte
of a noncontiguously mapped next page: it declines. `IntelBase`, the LDT
shadow and DOSX code selector are initialized explicitly; the fixture uses
real selected CCPU page translation and DPMI function, but includes existing
`/force:multiple` test seams and does not show a real guest taking this
path. An early fixture attempt failed because its synthetic `IntelBase` was
not initialized; that setup failure is not treated as a product crash.

`tests/observation/verify-dpmi-mapped-code-known-gap.ps1` preserves these
non-passes as a diagnostic; success of that diagnostic means the known gap
was reproduced, not that the product passes. The normal suite does not count
these failing cases as accepted behavior.

A correct integration must fetch each instruction byte using the current
linear mapping, preserve original descriptor/default-size and instruction
read order, handle a page boundary, and distinguish translation failure from
a valid pointer to physical zero. `c_GetLinAdd` substitutes physical zero
for a failed translation, so merely moving the EIP addition before this
call would conceal a failure. The NT4 host exception owner and nested CCPU
fault/rollback contract are still unresolved. Do not substitute generic
#GP, assume contiguous mapping, or use an unchecked fallback. This remains
a concrete S11 blocker to unqualified mapping/exception parity; no production
code or guest was changed by this witness.

The same controls were extended through the actual `DpmiFaultHandler(13,0)`
(cases 96–98) and through `host_exint_hook(13,0)` with that handler installed
(cases 99–101). Both produce the same identity pass and remapped/split-page
non-passes. The latter proves the selected monitor's actual fallback reaches
the affected function in this controlled setup; it does not show that a
released guest reaches this configuration. The fixture links selected
objects with its existing `/force:multiple` boundaries. Outputs are retained
in [actual-chain-result](s11-dpmi-mapped-code-evidence/actual-chain-result.txt);
the normal 91-case monitor suite also passed after relinking. No runtime
implementation was changed.
