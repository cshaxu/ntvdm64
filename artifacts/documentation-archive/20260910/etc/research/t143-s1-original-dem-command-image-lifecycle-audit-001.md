# T143 S1 original DEM command-image lifecycle audit 001

## Original plane

The original DEM table maps:

| Service | Original handler contract |
| --- | --- |
| `50:12` `SVC_DEMOPEN` | `demOpen`: consumes the guest DS:SI pathname and returns an NT host handle through the historical CCPU/VDM ABI. |
| `50:16` `SVC_DEMREAD` | `demRead`: consumes that handle plus guest DS:DX buffer and CX length, performs a host read and writes directly through `GetVDMAddr`. |
| `50:02` `SVC_DEMCLOSE` | `demClose`: releases the same historical host handle. |
| `50:42` `SVC_DEMFASTREAD` | original `demdisp.c` assigns this table slot to `demNotYetImplemented`, not `demRead`. |

The current adapter does not import those historical host handles or pointers.
Instead its one boot-namespace provider owns the finite profile files and one
provider/session lifecycle: copied pathname open (`50:12`), checked bulk
ordinary-RAM read (`50:16`), and close (`50:02`). This is the intended
contained CLI rehost boundary, not three independent BOP fixes.

## T130 evidence limit

T130 observes `50:12`, `50:02`, and `50:42` on the path into `50:36`, but its
bounded trace has no observed `50:16` bulk command-image write and no record
of the original DOS allocation/relocation/PDB steps which would populate the
app segment `0BC1`. `50:42` is source-proven unavailable and therefore cannot
prove that the command bytes were loaded. The staged COMMAND artifact's
presence in the finite namespace likewise does not prove its guest-RAM
placement.

## Disposition

The adapter's service-plane architecture is coherent and remains unchanged.
The missing causal segment is wholly inside original NTDOS `$Exec`: how its
file lifecycle, allocation and PDB construction produce `DS:SI=0BC1:0100`.
T144 must audit this guest-side `$Exec` path from `ExecRead` through the
transfer preparation, using original source only. No BOP/provider/runtime
change is admitted.
