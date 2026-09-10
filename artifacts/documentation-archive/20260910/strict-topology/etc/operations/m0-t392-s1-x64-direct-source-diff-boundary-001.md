# M0 T392 S1: x64 direct-source-diff boundary

## Decision

The x64 recovery must not use a build-time or release-time source-rewrite
script.  The former bulk C-VID GDP rewrite helper has been removed.  The
selected sources compiled by the formal graph must be the checked-in original
path files, with each required portability correction visible in its own
source diff.

## Reproduced baseline

The clean T392 x64 graph links `original-softpc-process.exe`, but all four
Console COMMAND cases time out before the existing configuration-complete
observer:

| case | result |
| --- | --- |
| `command/c ver` | timeout (12) |
| `command.com /c ver` | timeout (12) |
| `command.com /c command.com /c ver` | timeout (12) |
| interactive `command.com` then `exit` | timeout (12) |

This is not an app-entry or ROM failure.  The selected C-VID `gdpvar.h`
continues to describe a packed 32-bit layout.  For example, it assigns an
eight-byte host pointer field to `Gdp + 8` and the next field to `Gdp + 12`.
On x64 the two fields overlap.  More importantly, the generated rules bypass
the `GLOBAL_*` macros entirely: they dereference expressions such as
`*((IUH *)(r1 + 8))`.  The existing native-width GDP slot owner can therefore
not repair them by itself.

## Ownership boundary

`mvdm_gdp_state.[ch]` remains external host compatibility: it owns only
private native-width storage and never changes guest state.  The original-path
rules under `src/mvdm-host/softpc.new/base/cvidc/` retain the selected rule
algorithm, but must directly express each access through the existing GDP
access boundary wherever `r1` denotes the packed GDP base.  This is an ABI
representation correction, not a C-VID, BIOS, VGA, timing, BOP, or COMMAND
behavior change.

## Admitted source batch

The required direct-diff batch is exactly these 34 selected C-VID rule inputs:

- `sevid000.c` through `sevid030.c`
- `sinit011.c`, `sinit012.c`, and `sinit013.c`

Every changed file must carry a short local `DIVERGENCE(MVDM-HOST-DIV-...)`
reason.  No script may generate, patch, copy, or select an alternate C/H
version of these files during a build.  A read-only audit may enumerate or
compare the checked-in diffs, but is not a compiler input.

## Proof required before publication

1. Clean x86 and x64 formal graphs compile these same checked-in rule files.
2. The x64 runtime reaches the configuration-complete observer rather than
   timing out during early SoftPC initialization.
3. The four existing clean Console COMMAND cases pass unchanged.
4. The S8 resource contract remains unchanged: only `bios1.rom`, `bios4.rom`,
   and `v7vga.rom` are embedded; `profile.spc`, `cmos.ram`, and `bios2.rom`
   remain excluded.

Until all four conditions pass, `ntvdm64.exe` is not published to
`O:\\ntvdm64`.
