# M68 Guest COMMAND CMDINFO and Bootstrap ABI Ledger

Status: source-derived guest/host command ABI record, 2026-08-08.

## Sources

- `src/opennt/base/mvdm/inc/cmdsvc.h`
- `src/opennt/base/mvdm/dos/command/cmd.h`
- `src/opennt/base/mvdm/dos/v86/cmd/command/init.asm`
- `src/opennt/base/mvdm/dos/v86/cmd/command/rdata.asm`
- `src/opennt/base/mvdm/dos/v86/cmd/command/tcode.asm`

## Fixed Guest Record

`cmdsvc.h` declares `CMDINFO` under `#pragma pack(2)`. Its 36-byte guest ABI
is, in order:

```text
00 EnvSeg          02 EnvSize          04 CurDrive         06 NumDrives
08 CmdLineSeg      0A CmdLineOff       0C CmdLineSize      0E ReturnCode
10 bStdHandles     12 pRdrInfo (DWORD) 16 CodePage         18 fTSRExit
1A fBatStatus      1C ExecPathSeg      1E ExecPathOff     20 ExecPathSize
22 ExecExtType
```

The record is the command broker's protocol, not generic core state. It must
be modeled as a version-locked guest profile structure and accessed through
checked field/span operations. A future backend must not cast it into a host C
pointer or retain such a pointer after the handler returns.

`SCSINFO`, registered through `SVC_CMDSETINFO`, is a separate guest record:
64-byte COMSPEC buffer, 128-byte command-tail buffer, a parameter block, and
the one-byte `SCS_ToSync` marker. Its fields explain the host's historical
pointer caching but do not justify that caching in a new runner.

## Observed First COMMAND Order

For the permanent COMMAND instance, `init.asm` calls `SVC_CMDCOMSPEC` with
DS:DX pointing to the resident COMSPEC string. The returned AL controls the
historical pause behavior.

During environment replacement, `rdata.asm` allocates an ES environment
segment and invokes `SVC_GETINITENVIRONMENT` with ES:0 and BX capacity in
paragraphs. A returned BX of zero selects the old environment; a returned BX
larger than the allocation causes reallocation and one retry. Thus the service
has a real capacity-negotiation contract, rather than an unconstrained guest
write.

At the command prompt, `tcode.asm` fills its transient CMDINFO fields:

- command line buffer location and executable-path buffer location;
- redirector completion pointer; and
- environment pointer/size fields.

It then calls `SVC_CMDGETNEXTCMD` with DS:DX pointing at that CMDINFO. CF set
means the environment buffer was too small; AX carries the required byte size
and guest code reallocates before retrying. CF clear proceeds to command
execution.

## Profile Versus Product Fields

| Needed by a contained first CLI profile | Historical broker/product-only until proven otherwise |
| --- | --- |
| COMSPEC value, bounded OEM environment, command line buffer, executable path, current drive, return code, environment capacity result | `pRdrInfo`, `bStdHandles` raw-handle convention, `fTSRExit`, `fBatStatus`, PIF/session event policy, BaseSrv request/re-entry machinery |

The table is a staging distinction, not permission to omit a field from the
guest record. The complete version-locked record must still exist exactly as
the guest expects; the adapter may explicitly reject or defer fields whose
historical product meaning is outside the first profile.

## Next Boundary

The records M60-M68 now establish the principal bootstrap, file, namespace,
error, command, and exit service categories. The next work should synthesize
these facts into one versioned, non-implementation profile contract and map
each required operation to either a currently available backend capability or
one of M55's externally governed capability gaps. No guest/host dispatcher was
implemented.
