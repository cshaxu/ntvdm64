# T225 S33 bootstrap continuity observation admission map

## Question and result

Can the current repository distinguish original guest reach of `sysinit`,
`DosInit`, and initial device-chain work without adding a general instruction
trace, copying arbitrary guest memory, or changing execution?

**Yes.** The already-bound, default-off BOP sequence observer is sufficient.
The retained S19 Direct transcript has 33 fixed-width C4 C4 records with no
overflow; source correlation identifies three ordered, original guest
checkpoints. No new observer, adapter route, Bochs change, or native run is
admitted by S33.

## Existing observation ABI

`src/bx-vdm/bx_ntvdm_bop_sequence_observation_v1.h` records at most 128
already-reached C4 C4 instruction boundaries. Each record contains only
`CS:EIP`, selector, optional service byte, and typed disposition. It holds no
register state, guest memory, provider identity, or semantic outcome.
`bx_ntvdm_vdm_generic_ud_bridge_v1.c:68-80` copies an event only after the
normal generic-UD route has accepted or declined it; it does not decide that
route. The CLI exposes it only by the explicit default-off
`--observe-bop-sequence` option (`src/cli/ntdos64_native_cli.c:149-153,
191-224`).

This is a suitable mechanical evidence seam because the source correlation is
performed after collection. Neither bx-core nor bx-mantle recognizes OpenNT,
DOS, a selector, or a service name.

## Retained native checkpoint correlation

The retained source-built S19 Direct transcript is local reviewed evidence at
`build/M0-T225-S19/native-package-validation-r1/native-run/direct.stdout.log`;
its reviewed result and identity are recorded in
`docs/etc/evidence/t225-s19-dem-native-package-validation-result-001.md`.
The Readonly transcript is byte-identical. Its relevant ordered records are:

| Record | Retained `CS:EIP` / BOP | Original source correlation | What it proves | What it does not prove |
| --- | --- | --- | --- | --- |
| 0 | `0070:0475`, `50:11` | `bios/msinit.asm:247-252` | Original NTIO requested NTDOS loading and current narrow seam resumed it. | Any later guest stage. |
| 1 | `010D:0516`, `12` | `bios/sysinit1.asm:818-855`; retained static map `docs/research/ccpu-bios-closure/S5-SYSINIT-ENTRY-STATIC-TRACE.md` | Execution passed the original post-50:11 far jump into `sysinit`/`goinit`. | That `DosInit` returned. |
| 3 | `9346:7439`, `50:0F` | `dos/msinit.asm:232-292`; `inc/dossvc.h:107` defines `SVC_DEMGETDRIVES=0Fh` | Original `DosInit` was entered far enough to consume `BiosComBlock` and request the drive count. | Exact temporary DOSDATA/SS values or device-chain completion. |
| 7 | `9346:7588`, `50:46` | `dos/msinit.asm:440-524`; `inc/dossvc.h:162` defines `SVC_DEMGETDPBLIST=46h` | `charinit` has returned for the console and device-chain loop, reached `continit`, and requested the DPB list. | Exact dynamic `DS:SI` at every `DEVIOCALL2`, strategy/interrupt return, or the first non-UD fault's instruction identity. |

The observation therefore resolves the broad continuity ambiguity left by the
isolated S30 first-fault record: original guest execution demonstrably reaches
all three named lifecycle stages in the same source-built, bounded native
composition. It does not transform the 33-event transcript into BOP
implementation authority; every record was already a package-level validation
fact.

## Reconciliation with S32

S32 correctly establishes that the first-fault record alone cannot establish
`sysinit`, `DosInit`, or `charinit` reach. Its wording that no dynamic proof
was then assigned to those stages is now superseded by the S19 transcript
correlation above. The lifecycle ownership and all prohibitions on synthetic
DOSDATA/device state remain unchanged.

## Four-rung observation ledger

| Recovery rung | Disposition |
| --- | --- |
| Original source reuse | Retained OpenNT sources and source-built maps provide the stage semantics and static address correlation. They are not a runtime observer. |
| Smallest adapter/shim | Retain the existing generic-UD/BOP sequence copier: fixed-width, default-off, route-observing only. It is already composable and sufficient. |
| External-code intrusion | Rejected. No CPU, decoder, memory, exception, or mantle change is necessary to establish these checkpoints. |
| Newly authored behavior | Rejected. A general PC/instruction trace, guest-memory snapshot, or synthetic breakpoint would exceed the needed evidence and risk changing execution. |

## Next boundary

No new continuity observer is required. The remaining narrow question is the
first non-UD `DS:FFFF` access **after** the now-proven original bootstrap
stages. The next package may perform a source/build address attribution of the
recorded `0C41:2357` state against the source-built NTDOS map and the existing
first-fault register record. It must not add a trace, modify guest state, or
repair machine behavior until that attribution is complete.