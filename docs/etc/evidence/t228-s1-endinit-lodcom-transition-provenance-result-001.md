# T228 S1 P26 — EndInit-to-LodCom Transition Provenance Result

## Result

The fresh explicit-provenance CPU5 graph completed the valid hash-locked
Direct/Readonly observation pair.  Both modes reached the retained
one-million-tick terminal with exit `4` and byte-identical stdout and stderr.
The final retained generic CS transition is:

```text
previous: 0C41:00007ED7  SS:SP 0C41:591C  BP 000A
current:  6972:0000645B  SS:SP 0C41:5918  BP 000A
predecessor bytes: FF 1E 8C 91 EB 1A 33 C9 80 FE FF 74 06 26 8A
successor bytes:   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
stack bytes:       DB 7E 41 0C 67 7E 01 00 69 99
```

The predecessor window exactly matches immutable source-built `COMMAND.COM`
file offset `0x7DD7` (the established COM image mapping is segment offset
minus `0x100`).  The paired result therefore proves that the predecessor is
not late corruption.  It also proves that the observed successor window was
already zero at the retained generic transition.

The earlier `0C41 -> 4444` state remains historical diagnostic context, not a
predicate for this run.  This fresh current composition instead reached the
`0C41 -> 6972` transfer above; P26 corrects that distinction rather than
retrofitting the observation to an old address.

## Source Disposition

The predecessor starts with an ordinary indirect near-call encoding
(`FF 1E 8C 91`), but the fixed-width capture intentionally does not read the
referenced runtime word.  The exact-hash recovered `command.map` does improve
the locality: file offset `0x7DD7` is linked `TRANGROUP 0249:5947`, between
public `Tsysgetmsg` at `0249:589E` and `$M_GET_MSG_ADDRESS` at `0249:597C`.
`TPRINTF.ASM` and its `MSG_SERVICES` expansion identify that bounded region as
original guest message-retrieval code, not the `LodCom` entry or a host BOP
dispatcher.  The map is not an instruction listing, however, so it cannot bind
the captured indirect call to one source statement or reveal its runtime
word.  P26 therefore does not claim an individual service, `Patch_stub` slot,
nor BOP.

Original source nevertheless defines the only admissible owner domain:

- `rdata.asm` `EndInit` sets `TrnMvFlg`, allocates the largest block, computes
  `TrnSeg`, reverse-copies the transient, then deallocates the temporary
  block; its comments say resident COMMAND will allocate it again and expect
  the transient intact.
- `stub.asm` declares `LodCom_Entry` as a patched far entry and routes
  `LodCom_Trap` through it after `CheckA20` and saved segments.
- `command2.asm` `LodCom1` receives/reuses the transient allocation, performs
  the source-defined copy/reload paths, restores the resident state, and
  reaches `TJmp: jmp Trans`.

Consequently the zero successor remains an explicit **whole original COMMAND
transient allocation/move/reload plus NTDOS MCB lifecycle unknown**.  It is
not grounds for a Bochs CPU patch, a BOP/provider leaf implementation, an
adapter workaround, or a mutation-profile change.

## Build, Inputs, And Parity

- Graph: `build/M0-T228-S1/p26-endinit-lodcom-provenance-20260818a`, MSVC x64
  `/MT`, CPU5 projection, with only the existing history and explicit
  provenance definitions.
- CLI SHA-256:
  `fc4cb7c12166314866ed4e443d35f0cc5c5a2105c96ba93168c9e2d50da5f887`.
- Immutable input manifest SHA-256:
  `c888239b9de415f5c5f38b8d8dcacf5a30c102ab1465189c1ffc1a97fb1b9b6a`.
- Child environment was cleared, then populated only with `SystemRoot`,
  `ComSpec`, the two retained startup-source paths, and `OPAQUE_A` through
  `OPAQUE_D`, each 1,000 characters.
- Both runs used the exact existing CLI observers
  `--observe-budget-terminal-cs-transitions` and
  `--observe-budget-terminal-provenance`, with the same one-million-tick
  budget and no BOP, interrupt, or first-fault observer.
- Durable disposable observation:
  `build/M0-T228-S1/p26-provenance-observation-20260818b/observation.json`.

| Mode | Exit | stdout SHA-256 | stderr SHA-256 |
| --- | ---: | --- | --- |
| Direct | 4 | `dd3b17a4e5d801c04ff9c66780d5e534fa8301dab5ec191766f7a3eef6198ed1` | `52de23b1a4fbd12fe3fd463888431e2b5a4b852502bad2dfff6a4b4b50025860` |
| Readonly | 4 | `dd3b17a4e5d801c04ff9c66780d5e534fa8301dab5ec191766f7a3eef6198ed1` | `52de23b1a4fbd12fe3fd463888431e2b5a4b852502bad2dfff6a4b4b50025860` |

## Limitation

The evidence records only fixed existing diagnostics.  It neither follows the
indirect pointer nor establishes the writer of the zero successor.  Any repair
must first compare the complete original COMMAND transient and NTDOS allocator
owner package; trace data remains integration evidence, not a BOP work queue.