# M0 T310 S16 P2 — BIOS/firmware formal x86 assembly

## Build

The fresh selected Win32/x86 `/MT` CCPU40 graph was generated at
`build/M0-T310/S16/original-softpc-ccpu40-s16p2` with:

```text
New-T310OriginalSoftpcNinja.ps1 -Architecture x86 \
  -BuildRoot build/M0-T310/S16/original-softpc-ccpu40-s16p2 \
  -NodeExecutable O:\\.nvm\\versions\\node\\v22.22.1\\bin\\node.exe
ninja -C build/M0-T310/S16/original-softpc-ccpu40-s16p2 \
  original-softpc-candidate
ninja -C build/M0-T310/S16/original-softpc-ccpu40-s16p2 \
  original-softpc-forced-closure.dll
```

The candidate completed all `369/369` requested edges. It contains the
complete original `base/bios` manifest archive, original `base/system` ROM and
CMOS bodies, original host resource bodies, immutable firmware inputs and the
existing app/session-to-firmware-root binding.

## Forced-link disposition

The forced-closure log has no unresolved `host_find_file`,
`host_read_resource`, `read_rom` or CMOS resource form. It therefore confirms
that the selected original resource lookup and ROM-input chain is not being
silently replaced by a standalone BIOS shim.

The log deliberately still reports later-owner forms, including controller,
video, storage, Base VDM, BOP and debugger edges. `/force:unresolved` is an
ownership audit only and is not executable evidence. No S16 BIOS/provider
semantics were fabricated to make that audit quiet.

## Result

S16 now has a direct selected-source x86 assembly baseline. The next packet
step is to classify the original BIOS reset and ROM/CMOS callable contracts,
including the immutable-media failure direction and the later controller
owners they invoke.
