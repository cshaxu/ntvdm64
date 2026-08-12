# OpenNT REDIR Build Record

## Scope

This record covers the OpenNT guest redirector TSR declared by
`base/mvdm/dos/v86/redir/makefile` as `redir.exe`. It is a source-closure
result for a guest component, not a claim that the resulting program can run
without a corresponding host redirector service.

## Inputs and Stage

```text
OpenNT input revision: 5c5b979ec08c17d3ca2eb70e8aad62d26515d01c
stage:                 toolchain-probe/redir-stage-v1
build profile:         original retail DEBUG=0 and CALL_DOS definitions
message input:         usa/redirmsg.inc copied to redirmsg.inc unchanged
```

The original source checkout is unmodified. The stage copies the REDIR source,
the V86 include tree, and the MVDM common include tree.

## Stage-Only Compatibility Change

JWasm treats `INVOKE` as a built-in keyword, whereas the historical shared
`dosmac.inc` declares a lower-case `invoke` macro. The stage renames only that
legacy macro and its active internal call sites to `ntinvoke`, and uses the
same `EXTERNDEF` forward-declaration spelling already verified in the NTDOS
v19 stage. REDIR's own nine source modules contain no active use of that macro.
No OpenNT input file was changed.

## Assembly and Link Result

The original makefile object order was preserved:

```text
redir.obj resident.obj namepipe.obj mailslot.obj netapis.obj
int2a.obj int5c.obj neterror.obj msgapi.obj
```

All nine objects assembled with zero errors. JWasm emitted its known legacy
macro warning from `dosmac.inc`; `resident.asm` also emitted an alignment
warning. The Open Watcom DOS compatibility linker completed with
`option nocaseexact`.

| Artifact | Result |
| --- | --- |
| `redir.exe` | 2,906 bytes |
| SHA-256 | `BD59A99F42CF2E770E76A5E8475D65674FFAFDE89173D80EC6224A948CCBC100` |
| MZ signature | `4D 5A` |
| header paragraphs | 5 |
| relocation entries | 12 |
| declared entry | `0097:0000` |

## Host-Service Boundary

`rdrsvc.inc` defines each guest `SVC selector` as `BOP BOP_REDIR` followed by
the selector byte. `bop.h` fixes `BOP_REDIR` at `0x57`; `rdrsvc.h` declares a
contiguous selector set `0x00..0x31`.

The guest actually invokes services for these families:

| Family | Selectors used by the guest |
| --- | --- |
| Named pipes | `0x02..0x08`, `0x20`, `0x21`, `0x23` |
| Mailslots | `0x09..0x0e` |
| Environment and legacy mappings | `0x0f`, `0x1b..0x1f`, `0x2b..0x2d` |
| Remote/local network APIs | `0x10..0x1a`, `0x2e` |
| NetBIOS and DLC | `0x25..0x27`, `0x30` |

The asynchronous interrupt path is material: `int5c.asm` uses the virtual
network interrupt `0x76`, requires register/flag-preserving return behavior
between a BOP and its guest post routine, and shares a `VDM_REDIR_DOS_WINDOW`
with the host side. These calls cannot be represented by a synchronous DOS
file capability alone.

## Consequence

`REDIR.EXE` is now a reproducible compatibility-build candidate. It must be
treated as an optional wrapper feature behind an explicit REDIR service
provider. A minimal NTDOS bootstrap may omit it; loading or exercising it
requires a separately versioned BOP 0x57 dispatch contract, checked guest
memory marshalling, safe asynchronous interrupt injection, and explicit
unsupported-service behavior. No runtime test, host provider, or release
payload claim is made by this record.
