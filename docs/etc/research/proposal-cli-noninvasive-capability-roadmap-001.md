# Proposal: non-invasive CLI capability roadmap

## Purpose

This proposal turns achievable parts of the historical NTVDM user experience
into separately admissible CLI capability packages. It does not make the
modern CLI an implicit NT4 session manager. Each package is default-off,
profile-selected, independently source-mapped, and preserves this boundary:

```text
src/cli capability policy
  -> fixed-width contained host-capability seam
  -> bx-vdm source-mapped OpenNT contract
  -> bx-mantle / bx-core only for reached machine mechanics
```

No package may pass raw host handles, pointers, C++ objects or Bochs internals
across the boundary. The one-shot profile remains the default throughout.

## Common admission gate

No capability package below may begin until the first DOS profile has:

1. completed package-level BOP owner/disposition/failure mapping and
   regression for every admitted BOP family;
2. run a real declared DOS target through normal guest termination; and
3. retained a fixed-width terminal result contract where the package needs a
   CLI-visible result.

Each package also needs original OpenNT caller and failure evidence, an
explicit profile flag, default-off and negative behavior, focused source-built
regression, and bounded integration evidence. A trace may verify the completed
package; it never creates a one-service patch.

## Modern CLI lifecycle governance

The CLI can own cancellation, instruction/time budgets, resource limits,
audit logs, cleanup, and presentation of a declared terminal result. This is
modern process governance, not recovery of CSRSS, the NT4 desktop session
manager or VDD infrastructure.

The contract must distinguish guest normal completion, guest controlled stop,
budget expiry, explicit cancellation, host-stream failure and composition
failure. Ctrl+C may request bounded cancellation, but cannot inject
undocumented guest state or repurpose a host process exit code as a DOS
register result.

## Host-context projection

The CLI can collect environment variables, a working-directory choice and
selected drive roots at launch, then project only admitted values into the
guest profile. It must not let the guest query arbitrary host environment,
current directory, mounted drives, registry or filesystem after startup.

This package maps OpenNT COMMAND environment/current-directory callers and
DEM drive callers separately. It explicitly defines filtering, encoding,
drive-letter mapping, snapshots, invalid paths and denied access. `stdin`,
`stdout` and `stderr` are not context projection; they belong to the
standard-handle package below.

## Interactive COMMAND session

An opt-in `interactive-console` profile can attach a selected terminal input
source and output/error sinks, provide an explicit EOF result, and feed
successive command declarations through the OpenNT-shaped `54:01` lifecycle.
`COMMAND.COM` remains the guest command interpreter; bx-vdm routes its
source-mapped requests but never becomes a shell.

The session must run a bounded transcript and separately prove EOF,
cancellation and output-disconnect behavior. It cannot imply GUI, VDD,
KEYB installation, ambient console ownership, unrestricted host drives or a
host process broker.

## Standard handles, pipes and redirection

The CLI can provide controlled `stdin`, `stdout` and `stderr`, then
progressively recover only reached DOS/COMMAND handle, device, pipe and
redirection contracts. This is larger than answering BOP `54:06`: guest DOS
handle tables, `CON`/device semantics, redirection error behavior and possibly
redirector ownership all need source maps.

The profile must choose each endpoint and pipe explicitly. A disconnect, full
pipe or unsupported device returns the mapped DOS/COMMAND failure; no native
`HANDLE`, `FILE*`, socket or inherited host descriptor enters guest state.

## Guest DOS multi-program execution

Multiple DOS programs can run inside one VDM without nested host processes.
The normal guest path is `COMMAND.COM` or a DOS parent invoking DOS `EXEC`,
which creates a child PSP and later returns to its guest parent. The needed
work belongs to guest DOS process, PSP, memory, executable loading, file and
return semantics, not to a second `ntvdm` CLI invocation.

Admission requires a source-backed parent/child/return fixture and an explicit
guest file-view capability. It does not authorize DOS-to-Win32, DOS-to-WOW16
or DOS-to-another-VDM launch; those are retained research debt.

## Virtual-device capability profiles

Virtual devices are feasible only one profile at a time. For each device, the
proposal must name the reached OpenNT/guest caller, exact Bochs core or mantle
owner, host backend seam, allowed host resources, failure mode and negative
test. Bochs remains device-mechanics owner; bx-vdm does not learn BOP/DOS
semantics from a device implementation.

Candidate profiles might eventually cover a terminal display/keyboard path,
serial endpoint, printer sink or selected storage view. None is admitted by
this proposal merely because Bochs upstream contains a device.

## Not presently proposed as implementation

Original OpenNT shell-out and session-broker behavior for guest requests to
launch or wait for Win32, WOW16 or another VDM depends on historical NT host
composition. It is listed in `TODO.md` for source/ABI/failure research. A
future implementation requires a separately approved host-launch capability;
it must not be smuggled into interactive I/O or COMMAND BOP handling.
