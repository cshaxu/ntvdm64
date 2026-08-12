# Non-MASM386 Toolchain Removal

Status: complete, 2026-08-07.

The research workspace no longer contains JWasm, UASM, OpenWatcom, or an
active script that invokes any of them. The removal included their local tool
trees, compatibility overlays, WLink-only linker helpers, obsolete probe
scripts, and all guest-image stages built through those paths.

The retained guest build boundary is intentionally narrow:

- manifested OpenNT source input under `src/opennt`;
- the locally source-built MASM386 closure;
- DOSBox plus the recovered historical `LINK.EXE`;
- local `reloc`, `stripz`, and `stripdd` format helpers with their focused
  tests;
- host-only BUILDMSG compatibility support, documented separately.

`KEYBOARD.SYS` and `SETVER.EXE` are absent rather than represented by legacy
compatibility images. Reintroducing either requires a new manifested pristine
source closure and a MASM386-first build record; no deleted toolchain may be
restored as a fallback.
