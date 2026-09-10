# T198 S12 P3 Native Cross-Island Link Frontier 001

## Question

Can the complete i686 S12 adapter/provider object closure replace the generic
bridge in the existing MSVC/x86 CPU5 finite-run fixture and execute a bound
`UD2` transaction?

## Inputs And Procedure

The probe was extended only to accept an external fixture source and expand
its object glob. The fixture explicitly initializes/binds the adapter session,
then invokes the existing finite native runner on `UD2`. The r1 i686 closure
contains the complete S10/S12 provider set. Fresh CPU5 seed builds r2--r5
completed; r5 compiled the external MSVC/x86 fixture and reached the final
link.

## Observation

The r5 final MSVC/x86 link failed before execution. The complete MinGW object
set requires `___chkstk_ms`, `___mingw_swprintf`, `___udivdi3`, imported
`towupper`, and MinGW-decorated BCrypt symbols; it also imports UCRT helpers.
This is not the simple eight-object S7 classification island. It is an
in-process CRT/runtime boundary violation, not a missing Bochs symbol or a
provider semantic failure. The link also reports the expected unresolved
mantle mechanical-action function because the existing finite-run probe adds
that object only for its standalone action mode, not for an external composed
bridge.

The r1 attempt ended at the outer 60-second command timeout; r2 exposed a
literal PowerShell glob; r3 exposed missing adapter/CLI include roots; r4
exposed a fixture `const` mismatch. Those probe defects were corrected before
r5 reached the actual link frontier.

## Interpretation

The generic record ABI remains valid, but complete in-process provider
composition cannot use the current MinGW object set in the MSVC/x86 machine
link. Adding MinGW runtimes or forcing mixed CRT linkage would violate the
declared cross-island rule. The next route needs an explicit governing choice:

1. admit a separately source-built MSVC/x86 `bx-vdm` composition island using
   fixed records and native `bcrypt`, with no MinGW runtime in the executable;
   or
2. design a separate process ABI, which cannot synchronously operate the
   mantle action record and is therefore a materially different architecture.

The probe deliberately does not choose either route. No `bochs.exe`, product
shell, device archive, OpenNT binary or legacy adapter runtime was linked.

## Follow-up

Pause S12 before further native link edits. The owner must approve the
MSVC/x86 adapter-island exception or the distinct process-ABI redesign.
