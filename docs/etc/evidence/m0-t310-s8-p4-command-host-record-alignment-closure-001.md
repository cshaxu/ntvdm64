# M0 T310 S8 P4 — original COMMAND host-record alignment closure

## Scope

This closure covers every x64 C4366 reported by the selected original
`dos/command/cmdredir.c` owner package.

## Source contract

`SCSINFO` and `STD_HANDLES` are packed guest-facing records.  They carry
fixed offsets consumed through the original VDM service ABI.  The following
`PIPE_INPUT`, `PIPE_OUTPUT`, `REDIRCOMPLETE_INFO`, and `VDMENVBLK` records are
different: they are allocated and consumed entirely by COMMAND host code and
contain native pointers, Win32 handles, thread state, and a
`CRITICAL_SECTION`.

The original header left `#pragma pack(1)` active across both categories.
That did not fault in its x86 product layout, but on x64 it produces an
unaligned address for every synchronization API call into a pipe record.

## Resolution

`MVDM-HOST-DIV-119` closes the packing scope after the two guest records.
The guest formats, member order, BOP/register identity mapping, pipe
algorithm, and original cleanup ordering are unchanged.  The host-private
records regain natural compiler alignment, which is required by their native
Win32 fields.

No mapping-manager change is involved: the existing COMMAND redirection
identity bridge remains the sole owner of values crossing the guest ABI.

## Verification

Forced formal Ninja `/MT` object rebuilds completed on both supported host
architectures:

- x64: `build/M0-T310/S8/p1-machine-source/x64/command-redirection-abi.log`
- x86: `build/M0-T310/S8/p1-machine-source/x86/command-redirection-abi.log`

The x64 log contains none of the former C4366 records.  The two pre-existing
K&R C4131 source-style diagnostics remain visible on both targets and are
not treated as x86/x64 transport repairs.
