# M0 T310 S8 P4: X86GFX Console Video-Hole Disposition

## Original source contract

`softpc.new/host/src/nt_sec.c` implements the historical full-screen hardware
transition:

- `LoseRegenMemory` releases the host process virtual address `0xA0000`.
- `RegainRegenMemory` reserves/commits the same host address.
- Under `X86GFX`, `GetROMsMapped` passes actual ICA and event pointers to
  `NtVdmControl(VdmInitialize)` for the NT4 Console Server/kernel VDM
  composition.

The selected CPU_40/C_VID profile does not define `X86GFX`; its startup
`GetROMsMapped` call is correspondingly compiled out. The remaining source
warnings are retained evidence because this translation unit is kept as an
original selected mirror input.

## Disposition

This is an explicit **original-unavailable Console Server full-screen hardware
video-hole boundary**, not a guest physical address or a session mapping-manager
identity. Mapping `0xA0000` to SoftPC/guest RAM would change the original
Console Server ownership and fabricate success. No adapter, header declaration
or source-body modification is admitted for this path.

The selected windowed/stream C-video controller path remains independent and
continues through its own P4 ABI closure. A future full-screen product package
would require a separately approved public-host display composition rather than
reviving kernel VDM or using this fixed host virtual address.

## Verification

Formal x64 and x86 Ninja compilation of `obj/host/nt_sec.obj` succeeds. The
three recorded diagnostics remain visible, matching this intentional
unavailable disposition.
