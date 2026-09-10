# T198 S31 Terminal RAM Snapshot 001

## Question

Can the finite `bx-mantle` fixture preserve a bounded, opaque terminal
ordinary-RAM observation without adding guest or BOP semantics, and does that
observation confirm the `0x0A84` mutation seen by S29?

## Mechanic

`bx_ntvdm_finite_run_terminal_snapshot_configure_ordinary_range(address,
count)` is a private, one-shot finite-run fixture configuration. A zero length
disables it. A nonzero range is accepted only when it is within the existing
one-MiB ordinary-RAM aperture and at most 64 bytes. At the beginning of each
run, the configuration is copied locally and cleared; therefore it cannot
persist into a later run. After the CPU loop returns but before cleanup, the
mantle uses its existing checked `copy_from_ordinary_ram` primitive to copy the
requested range into the already-private terminal snapshot.

The mechanic has only physical address, byte count, and copied bytes. It does
not name or parse BOP, OpenNT, DOS, NTIO, a selector, a device, or a host
capability. No `bx-core` source changed.

## Verification

The exact fixture first confirms default disable (`0,0`), rejects an
out-of-aperture `0x100000,1` request, then configures the 16-byte range at
`0x0A80`. The existing MSVC x64 `/MT` compile/link commands were run:

```
cmd /c artifacts\build\t198-s24-terminal-snapshot-r1\compile.cmd
cmd /c artifacts\build\t198-s25-native-command-r1\link.cmd
artifacts\build\t198-s23-native-ntio-run-r1\command-r1.exe
```

The process exits zero. Its relevant exact output is:

```
observed-first-generic-ud=1 fault=0000:0a84 mode=1 vector=6
window=15:ffffffff observed-stop=1 ...
terminal=1:0000:0ad2 snapshot=1:00a80:16:ffffffff
```

The fixture also compares all 16 terminal bytes with the exact published NTIO
source bytes at image offset `0x380` and requires a difference. The four bytes
at terminal offsets `4..7` are required to be `FF`; the source image at the
same physical positions begins `14 03 FF FD`.

## Result And Boundary

S29's copied instruction window is independently confirmed by a checked
ordinary-RAM terminal snapshot. The mutation happened after source image
publication and before the accepted generic stop. The result does not identify
the writer or control-transfer predecessor, so it does not authorize a memory
repair, IVT/BDA initialization, firmware, VGA, or device change.

## Follow-Up

The next task should capture a bounded predecessor state at the first generic
fault: the copied event already contains GPRs and segment values, so the
test-only bridge can retain that complete record and report the stack/segment
state alongside this terminal snapshot. It must still not interpret or mutate
guest memory; source mapping follows only after that raw state is available.
