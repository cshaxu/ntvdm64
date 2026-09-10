# M0 T377 S1 — DOS device workload matrix and owner admission

## Question

Which smallest immutable original DOS workload matrix can cover the next
keyboard/console, file/seek/IOCTL, timer/IRQ and video owner paths without an
app loader, generated guest input, or trace-selected repair?

## Candidate disposition

- `MEM.EXE` is selected for noninteractive console plus DOS memory/IOCTL
  inquiry.  The selected original artifact is
  `src/mvdm-guest/dos/v86/cmd/mem/mem.exe`, 40,462 bytes, SHA-256
  `c4582b1e0738f3aaf89e17de0f940caa7e4b3d77a21fccd6ff91b7e075bdf6eb`.
  Its checked `build/output/dos/mem.exe` counterpart is byte-identical.
  `mem.c` begins by obtaining the PSP through DOS INT 21h and its original
  headers/data path declare DOS IOCTL use.  It is not accepted as evidence of
  keyboard or video behavior.
- `KB16.COM US` is selected for the original keyboard install and its original
  file/seek chain.  `KB16.COM` is 14,710 bytes, SHA-256
  `9c432ea71e0e11bfb2edf9166c862826115ec4cdf8439f986a5ed71d9a3af3b9`,
  byte-identical with `build/output/dos/KB16.COM`.  Its guest source opens and
  seeks the original `KEYBOARD.SYS`; that immutable companion is
  `src/mvdm-guest/dos/v86/dev/keyboard/KEYBOARD.SYS`, 40,665 bytes, SHA-256
  `96fcac606683cf262dbc030713a9263731ee83f19a10207e5dd632e5ee06c2ad`.
  This selects the complete original keyboard-controller/vector owner path,
  not fabricated keystrokes.
- `GRAPHICS.COM` is selected for original video BIOS/controller state and
  resident vector installation.  It is 19,694 bytes, SHA-256
  `989b38806ec1e1bdd30da3dc6b41cc85bdc81dda74f870fcc7df399876eb2756`,
  byte-identical with `build/output/dos/GRAPHICS.COM`; its original
  `graphics.pro` companion is 21,232 bytes, SHA-256
  `b734f750a473f83d33aa773fe3d6c7bcdd209d97339f37b03b8c5f3f8eed890b`.
  `grinst.asm` calls the original multiplex service and installs original
  INT 5/10/2F vector behavior; `grctrl.asm` queries video BIOS state through
  INT 10h.  An actual Print-Screen rendering event requires real later user
  input and is not silently simulated in this package.
- `EDLIN.EXE` and `DEBUG.EXE` are rejected for this matrix because their normal
  contracts are interactive and would require injecting console/keyboard input.
  No synthetic input source is admitted.

## Immediate owner map

The selected guest programs remain load-only original code.  Their host-side
mechanics are already represented by original source families in the formal
CPU40/x86 manifest, but not yet runtime-proved as one matrix:

- keyboard: `mvdm-host/softpc.new/base/keymouse/{keyba.c,keybd_io.c,ppi.c}`;
- video: `mvdm-host/softpc.new/base/video/*`, including `video.c`,
  `video_io.c`, `vga_*`, `ega_*`, `cga.c`, and cvidc access sources;
- timer/IRQ: original `mvdm-host/softpc.new/host/src/nt_timer.c` together with
  original SoftPC timer/ICA interfaces;
- file/seek/IOCTL: original DEM `demfile.c`, `demhndl.c`, `demioctl.c`,
  session host-resource IDs and bounded guest-memory leases.

The S2/S3 source-recovery ladder must revalidate each source body and its
same-shaped `adapter-mvdm-host-out/softpc` or Win32 binding before enabling a
runtime conclusion.  Existing formal membership is not runtime proof.

## S1 conclusion

The matrix is frozen as `MEM.EXE`, `KB16.COM US` plus `KEYBOARD.SYS`, and
`GRAPHICS.COM` plus `graphics.pro`.  S2 owns only console/keyboard and
file/seek/IOCTL binding closure.  S3 owns timer/IRQ and video owner closure.
No stage update or product run occurred in S1.

