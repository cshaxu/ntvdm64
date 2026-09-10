# T180 S1 post-target COMMAND lifecycle map 001

## Question and method

This read-only map reconciles original `tcode.asm`, `cmddisp.c` and
`cmdmisc.c` with the current declared-target provider. It distinguishes the
post-target `54:01` request from normal child-return `54:11`; it does not add
a host command queue or alter runtime behavior.

## Original structure

`cmddisp.c` maps service `01h` to `cmdGetNextCmd`. COMMAND's transient code
prepares a `CMDINFO` buffer at DS:DX and issues `CMDSVC SVC_CMDGETNEXTCMD`.
This occurs at its command-acquisition/re-entry path; it is not a normal DOS
child exit report. The historical `cmdGetNextCmd` then:

1. reads the guest `CMDINFO` and builds a `VDMINFO` request;
2. handles first/repeat-call, error and DOS-binary state;
3. copies redirection completion state and synchronizes current directories;
4. conditionally merges/reallocates the environment;
5. invokes `GetNextVDMCommand` (and may repeat for a larger environment);
6. writes path, command line, environment, drive and PIF-derived information
   into guest buffers.

On historical failure it presents an environment error and terminates the VDM;
there is no source-proven simple `54:01` "no next command, resume normally"
branch equivalent to the contained `54:11` CF-clear return contract.

## Current declared-target boundary

The current `bx_ntvdm_cmd_get_next_state_v1` intentionally accepts exact
real-mode `C4 C4 54 01` only while `delivered == 0`. It gathers the COMMAND
buffer, writes the immutable BYOB target launch envelope, resumes at +4 with
CF clear, and commits `delivered = 1`. Its paired `54:11` path is separately
gated by a real guest return and is not reached in T179.

The next `54:01` is therefore declined by design. That is correct protective
behavior: synthesizing another target, treating it as `54:11`, or making an
ambient `GetNextVDMCommand` queue would change the product contract.

## Dependency and owner table

| Original concern | Historical dependency | CLI disposition |
| --- | --- | --- |
| Command selection | `GetNextVDMCommand`, VDM state/repeat protocol | Unavailable outside an explicitly declared finite sequence. |
| Environment | allocation/reallocation, host environment and VDM merge | Not admitted to the immutable first-profile namespace. |
| Directories/redirection | current-directory synchronization, guest pointer and redirection-copy processing | Not admitted; no host filesystem or handle inheritance. |
| Console/event lifecycle | event-thread block/resume, console title/sleep, `TerminateVDM` | Historical host composition; not adapter behavior. |
| Target launch bytes | `CMDINFO` guest buffer ABI | Already contained for exactly one declared target. |
| Normal child status | `54:11` DX/CF contract | Existing contained implementation, but runtime-unreached. |

## Disposition

The next finite question is the post-target **second `54:01`** request. It
belongs to a future whole COMMAND no-next/finite-sequence composition, not a
Bochs machine handler, DEM provider or CLI result ABI. S2 may align this
source map with T179's exact trace and identify the first observed decline
edge; it cannot implement a response.
