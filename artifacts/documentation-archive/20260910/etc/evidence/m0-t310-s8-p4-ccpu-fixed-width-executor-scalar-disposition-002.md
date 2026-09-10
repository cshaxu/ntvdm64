# CCPU fixed-width executor scalar disposition

## Question

Do the remaining selected CCPU `C4244` records represent x86/x64 host-width
compatibility defects, or original fixed-width machine-value transitions?

## Inputs

- The current x86/x64 compiler classification ledger and its
  `SPC-CCPU-EXECUTOR-DISPATCH` rows.
- The selected original CCPU executor sources under
  `src/mvdm-host/softpc.new/base/ccpu386/`.
- The `IU8`, `IU16`, `IS16`, `IU32`, `IS32`, `ISM32`, and `IUH` type contracts
  from the selected SoftPC headers.

## Procedure

The S8 source reading grouped every current C4244 record by exact source and
destination type, then reread each ledger location with its destination field
and the operation that produced it.  It found only these original value forms:

- Intel dword/signed-dword operands written into byte, word, or signed-word
  register views.
- Arithmetic and shift results narrowed to their architecturally defined
  byte/word result fields.
- FPU tag-word OR/XOR expressions written into `IU16` tag temporaries.  The
  historical tag fields use `IUH` as a carrier, but the masks and destination
  are the original 16-bit FPU tag value; no host address or object identity is
  present.

The audit explicitly excludes native pointer/HANDLE values, guest-memory
lease results, host allocation sizes, callbacks, and cross-component IDs.  A
future source row outside the exact CCPU root/type matrix remains on the
normal x86/x64 worklist.

## Observation

The matrix covers the current C4244 messages from `IU32`, `IS32`, `ISM32`,
`IUH`, or MSVC's 32-bit `unsigned long` to `IU8`, `IU16`, `IS16`, or `IUM8`.
The
conversions occur on both x86 and x64 with the same fixed machine widths.
They neither widen a host pointer nor truncate an x64-only representation.

The same full source pass also identified the remaining CCPU `C4018` and
`C4146` rows as selector privilege checks, count loops, and unsigned Intel
arithmetic. Those values remain fixed machine scalars on both hosts. The four
`fpu.c` format diagnostics are original diagnostic-string defects, not width
transitions. `ThreadSimBuf`'s `jmp_buf` alignment warning is a private
same-process native layout: it must retain natural alignment, but it is not a
public structure or mapping boundary.

## Disposition

Classify this exact matrix as
`not-x86-x64-fixed-width-ccpu-scalar-contract`.  The compiler output remains
fully recorded and visible.  No warning is suppressed, no cast is added, no
mapping-manager entry is created, and no adapter/overlay replaces the original
executor logic.  Any functional concern is a SoftPC instruction-semantics
matter and remains subject to the CCPU execution and later machine-profile
tests, not an x86/x64 ABI repair.

## Follow-up

The unclassified signedness, format, alignment, K&R, and genuine native-width
rows remain in S8's worklist.  The next cold x86/x64 selected-source build
must regenerate the ledger before this cluster can close.
