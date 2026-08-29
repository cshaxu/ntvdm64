# M0 T310 S8 P4 — CCPU instruction-delta width closure

## Original contract

`c_main.c` keeps a private host decode cursor (`p`) and an instruction-start
cursor (`p_start`) while it decodes one guest instruction.  `SETUP_HOST_IP`
selects a code page; `DIFF_INST_BYTE` therefore produces the byte span inside
that selected private host page.  The result advances the original 32-bit
Intel `EIP`; it is not a host pointer, guest pointer, handle or mapping ID.

## x86/x64 divergence and repair

The historical macros assigned that pointer difference to `int`, or added it
directly to `IU32 EIP`.  MSVC x64 consequently formed a native-width
intermediate and emitted `__int64` narrowing diagnostics.  The mirror now
names the existing bounded byte span as `CCPU_INSTRUCTION_DELTA`, an `IU32`,
before the three original EIP-update forms use it.  The original instruction
order, `USE16` mask, EIP wrap, page transition and code fetch source remain
unchanged.  It is registered as `MVDM-HOST-DIV-125`.

No mapping-manager operation is appropriate here: both inputs remain private
SoftPC pointers and only their bounded difference enters fixed-width Intel
state.

## Formal focused proof

Outside the sandbox, the formal MSVC `/MT` Ninja graphs were forced to rebuild
`obj/ccpu/c_main.obj` for both `x64` and `x86`:

```text
ninja -C build/M0-T310/S8/p1-machine-source/x64 obj/ccpu/c_main.obj
ninja -C build/M0-T310/S8/p1-machine-source/x86 obj/ccpu/c_main.obj
```

Both rebuilds succeeded and neither emitted a `c_main.c` native-width
`__int64` C4244 diagnostic.  Unrelated fixed-width Intel byte/word conversion
warnings remain visible for their separate owner-contract disposition.
