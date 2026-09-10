# T95 S7 CLI Contained-Target Contract

## Decision

The first runnable CLI target is **profile-pinned**, not an arbitrary host
file passed into the adapter. A target must be a separately declared,
identity-checked BYOB component under the selected BYOB root. The public CLI
continues to accept `target [args...]`, but the runner accepts it for the DOS
engine only when its resolved local file is the selected component itself;
the engine receives only profile/root plus a normalized launch declaration.

This is a first-profile containment rule, not a claim that arbitrary host
paths will never be supported. A later capability may offer an explicit,
separately governed import/staging workflow. It may not silently turn the
BYOB root or an admitted drive into an unrestricted execution namespace.

## Why This Is The Minimal Boundary

The present runner passes an absolute host target path after `--`, while
adapter installation reads only profile/root/drive policy. Passing that path
to adapter code would make the host pathname an implicit adapter input.

An inherited pipe or Windows handle is not an acceptable shortcut: it would
introduce a new cross-process/cross-toolchain ownership protocol and conflicts
with the rule that host handles do not cross island boundaries. Linking the
GCC `byob_image` implementation into MSVC Bochs is likewise prohibited.

The existing flow already provides the correct mechanism:

```text
CLI target pathname
  -> runner compares it with one selected BYOB component
  -> engine receives profile/root plus immutable normalized launch metadata
  -> GCC adapter re-verifies and loads that component with byob_image_load_exact
  -> adapter-owned read-only namespace exposes selected bytes at a canonical DOS path
  -> CMDGETNEXTCMD receives a copied launch envelope only
```

No host path, handle, pointer, Bochs setting or guest-memory API crosses the
CLI/adapter or adapter/Bochs contract.

## Proposed v3 Profile Fields

The existing v1/v2 schemas remain unchanged. A new exact v3 first-profile
schema has the same mandatory `ntio`, `ntdos` and `command` components plus
one mandatory `target` component and one `guest_target_placement` declaration.

The target descriptor retains the standard component identity tuple:
`file_name`, exact byte count and SHA-256. Its filename is restricted to the
canonical selected guest basename `TARGET.COM` or `TARGET.EXE`; the extension
selects OpenNT's original `COM_EXTENTION=8` or `EXE_EXTENTION=4`. The placement
must use the same explicitly selected DOS drive as `guest_command_placement`
and the matching canonical path `\\TARGET.COM` or `\\TARGET.EXE`. No directory,
relative host name, PIF/BAT, wildcard or caller-selected DOS path is admitted.

The CLI tail is a versioned ASCII/OEM-safe byte string, has a 126-byte maximum
so its length byte plus CR/LF fit OpenNT's 128-byte UCOMBUF, and is copied
before engine start. Any non-ASCII input, embedded NUL, overlong tail,
unmatched target identity, absent target declaration, or target outside the
selected root fails before the engine starts.

## BOP 54:01 Consequence

With the v3 component loaded into the existing adapter-owned read-only
namespace, the already proven `CMDINFO`/generic-gather/multi-write route may
deliver it exactly once. It can write the path, UCOMBUF and source-shaped
response fields only after every requested guest span preflights. Environment
and later host-drive/console policy remain separate capabilities.

## Non-Goals

This contract does not implement file import, broad host-drive execution,
PE/NE/WOW dispatch, DOS wildcard search, environment synthesis, standard
handles, BOP routing, Bochs changes, or a guest runtime trace.
