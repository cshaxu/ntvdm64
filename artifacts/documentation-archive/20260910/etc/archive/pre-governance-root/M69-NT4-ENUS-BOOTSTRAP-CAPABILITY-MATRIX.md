# M69 NT4 EN-US Bootstrap Capability Matrix

Status: superseded as an execution decision by
`SOFTPC-RECONSTRUCTION-GOVERNANCE.md`, 2026-08-08. It remains a historical
capability-inventory record from M60-M68 and the M55/M58 backend audit.

> Governance correction: references below to a "neutral backend" or a
> `ntvdm64/core` admission are no longer an execution route for `ntdos64`.
> The active route repairs the historical SoftPC/CCPU monitor in this
> repository; the matrix is retained only for later neutral-requirement
> extraction.

## Scope

This matrix defines an executable-admission path for one fixed BYOB profile.
It is not an implementation plan for a local SVC dispatcher and does not add
DOS names, paths, selectors, file formats, or handles to `ntvdm64/core`.

## Capability Matrix

| Profile operation | Historical evidence | Neutral backend need | Current state | Owner after admission |
| --- | --- | --- | --- | --- |
| Pre-entry NTIO placement and real-mode entry | M61 | entry plan, ROM/A20/memory topology, checked preload | foundation exists | wrapper profile |
| NTDOS publication from `DI:0000` | M61 | paused ordinary-RAM all-or-none transaction; bounded stop/fault | governed gap | wrapper profile uses backend transaction |
| Observe/dispatch profile marker sequence | M60 | configured decode observation with copied context and finite outcomes | governed gap; current `#UD` only is insufficient | backend mechanism plus wrapper dispatch policy |
| DEM file open/read/write | M62 | checked directional buffer read/write/publication, opaque session state | foundation partly exists; publication/transaction discipline needs the governed path | wrapper/mantle capability adapter |
| Drive/current directory | M63 | checked structure/string read/write | foundation partly exists | wrapper/mantle session namespace |
| DOS error, VHE, retry | M64 | copied register patch, validated guest packet spans, finite resume/stop | copied-context discipline partly exists; registration lifetime is profile logic | wrapper/mantle error coordinator |
| Normal DOS process exit | M66 | finite stop/result and session cleanup | lifecycle foundation exists | wrapper/mantle session policy |
| Host 32/64-bit program handoff | M66 | host process/stream capabilities; finite wait/cancel/result | no generic external ABI yet | wrapper/mantle CLI policy |
| COMMAND COMSPEC/environment/CMDINFO | M67-M68 | checked multi-span copies, copied register results | entry/memory foundations partly exist; multi-span atomic publication is a governed gap | wrapper/mantle profile ABI |
| Debug services and fatal exit | M65 | optional trace sink and finite stop | no first-profile need | explicitly deferred/rejected |
| Pipe/redirector/WOW/DPMI/XMS | M60-M67 | separate async, redirector, protected-mode capability evidence | outside first profile | explicitly deferred |

## Hard Boundary

Only two missing backend facilities block the first historical-image prefix
trace itself:

1. **Configured decode observation** for a frozen, bounded instruction byte
   pattern, with copied context and finite `unhandled`/`handled-resume`/
   `stop`/`fault` outcome. It cannot permit mode, segment, mapping, device,
   IRQ, or host-object mutation.
2. **Paused ordinary-RAM transaction** that validates every normalized span
   before writing any byte, then commits all or none and invalidates on resume,
   reset, stop, fault, destruction, or context change.

M55 requires each to have an independent neutral consumer, core-owned tests,
and a narrow copied-value C ABI before this project uses them. No third core
extension is justified by M60-M68. File objects, DOS tokens, namespace rules,
VHE state, command policy, host launch, stream routing, and redaction remain
outside core.

## Executable Admission Sequence

1. A core owner admits the two neutral capabilities under M55, with an
   independent consumer and tests.
2. A versioned, opaque external C ABI exposes only copied machine state,
   checked spans, transactions, frozen decode registrations, and finite result
   values.
3. This workspace builds owned, redacted fixtures for NTIO pre-entry and the
   NTDOS publication transaction. Fixtures stop on the first unmet original
   dependency and never synthesize a DOS service result.
4. A wrapper profile binds the BYOB validator, immutable selected files,
   contained drive map, opaque file/stream/process tokens, and M60-M68 ABI
   adapters to that external ABI.
5. Only then is a bounded `NTIO -> NTDOS -> COMMAND.COM /C` test admissible.
   Its acceptance is a command result plus a redacted trace; no ambient path,
   registry, process injection, system-file write, or persistent guest bytes
   are allowed.

## Superseded Result

The former future split was recorded as:

```text
governed software backend
  -> neutral copied-value ABI
  -> version-locked NT4 profile adapter
  -> DEM/COMMAND service policy and capabilities
  -> bounded BYOB CLI command result
```

This conclusion is superseded. The historical CCPU/SoftPC chain is now the
software-monitor reconstruction target for x64 user mode; only the kernel V86
route remains excluded. M57-M58 remain source evidence, not controlling
execution authority.
