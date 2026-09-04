# Original Image Disposition and COMMAND Execution Recovery

## Corrected decision

Source review of original `msproc.asm::$EXEC` proves that an already-running
guest `COMMAND.COM` executes recognized DOS COM/MZ targets inside the current
VDM; that direct DOS `EXEC` route does not create another `ntvdm.exe`. This
does not cover the distinct original later `BOP 54:08` `COMSPEC /c` route
selected by normal NT COMMAND prompt processing. For that host-process route,
the modern adapter replaces only unavailable NT4 automatic VDM spawn for one
resolved DOS/Win16 image with a same-architecture product child.

## Purpose

Recover the original execution boundary without replacing guest DOS `EXEC`.
At direct product entry, classify exactly one concrete target before any VDM
exists: DOS opens the ordinary BaseVDM/PermCom route, native PE starts through
public Windows process creation, and Win16 is held at the WOW bootstrap gate.
Inside an existing guest `COMMAND.COM`, retain the original DOS format check,
`cmdCheckBinary`, `cmdExec32`, parent PSP and DOS `CON` paths.

This is the admitted final S6 scope of T391, not a replacement COMMAND
implementation or a new shell parser.

## Source Basis

- `mvdm-guest/dos/v86/doskrnl/dos/msproc.asm`: `$EXEC` exchanges and resets
  `SCS_Is_Dos_Binary`, calls `get_binary_type`, and directly executes known
  DOS images. Only type `0ffh` reaches `SVC_CMDCHECKBINARY` when original
  `SCS_DOSONLY` policy permits it.
- `mvdm-host/dos/command/cmdmisc.c`: the first PermCom request uses
  `ASKING_FOR_FIRST_COMMAND` while `IsFirstCall` is true. After the original
  `GetNextVDMCommand` returns its initial record, that same source body clears
  `IsFirstCall` *before* guest execution of the record's image begins.
- `mvdm-host/dos/command/cmdexec.c`: original `cmdExec` has two source-owned
  forms: its `AH==0` direct `cmdExec32` form, used after the first PermCom's
  PE -> `/z` -> BOP transition, and its `AH!=0` `COMSPEC /c <raw tail>` form.
  Original `cmdCreateProcess` creates, waits for and returns the child result.
- `mvdm-host/dos/command/cmdexec.c::cmdCheckBinary`: `SCS_DOS_BINARY` returns
  to DOS with carry clear. `SCS_WOW_BINARY` retains original WOW compatibility
  checks; native formats construct `COMMAND.COM /z <target>` for the original
  host execution path.
- `mvdm-guest/dos/v86/cmd/command/tcode.asm::check_command` and
  `tdata.asm::NT_INTRNL_CMND`: before `Do16BitPrompt` can issue
  `SVC_CMDEXEC`/`54:08`, the original second-shell code retains `EXIT`,
  `PROMPT`, `SET`, `PATH`, `CD`, `CHDIR`, and drive-letter changes in the
  guest. They resume normal guest COMMAND internal-command handling and never
  enter the host-process adapter. `VER` and every command absent from that
  narrow table retain the source-selected later `54:08` route.

The second guest shell's `ver` printing the host Windows version is expected:
it is a host `cmd.exe` built-in on the original `COMSPEC /c` branch, not proof
that the guest shell or DOS `CON` was replaced.

## Ownership And Contract

```text
first PermCom GetNextVDMCommand (IsFirstCall=1)
  -> initial record delivered -> IsFirstCall=0
  -> PE -> /z -> BOP -> original cmdExec
later COMMAND Do16BitPrompt / DOS CON -> SVC_CMDEXEC -> original cmdExec
  -> one child-launch disposition adapter
     -> native Win32: unchanged original direct or COMSPEC /c form
      -> DOS:          matching-architecture child product, DOS profile
      -> Win16:        matching-architecture child product, Win16 profile
  -> original cmdExec32 worker wait / re-entry
  -> original cmdReturnExitCode / BOP 54:0B
  -> original guest parent prompt
```

The adapter is therefore reached only after the guest's source-owned narrow
internal-command gate. It must not reclassify, emulate or redirect `EXIT`,
`PROMPT`, `SET`, `PATH`, `CD`, `CHDIR`, or a drive change. In particular,
`SET` changes the guest shell environment and `CD` changes the guest DOS
current-directory state; neither is a host `cmd.exe` operation in this path.

The new bounded adapter belongs to the COMMAND owner subfamily below
`adapter-mvdm-host-out`; it is not an app command parser, a new app launcher,
guest-media change, SoftPC change or generic BaseVDM broker. The child invokes
the existing product executable with its ordinary positional target; `app`
therefore follows its pre-existing initial BaseVDM record path unchanged.

Parent and child are separate processes. Each owns independent SoftPC, guest
RAM and session mapping-manager instances. The process command line contains
only the current product path, the resolved image pathname and copied raw
arguments. It may not contain a guest pointer, a mapping token, a native
HANDLE or retained parent-memory address.

The product executable is named by host architecture: `ntvdm32.exe` for a
Win32/x86 final link and `ntvdm64.exe` for a Win64/x64 final link. Each is
published at both `build/output` and the package root (`O:\ntvdm64` for the
current local profile). A disposable formal build may call its linked file
`original-softpc-process.exe`, but no task-specific filename is published as a
product executable and no architecture may stand in for the other.
`tools/build/StageProductExecutable.mjs` reads the PE machine field before it
copies a formal result into either published slot, and creates the package
root's `logs` directory for diagnostics rather than permitting records beside
the executables.

## Disposition

1. **Guest internal command:** preserve the preceding original
   `NT_INTRNL_CMND` gate and its guest COMMAND processing. It is outside the
   image-disposition adapter.
2. **Native Win32 or host-shell command:** retain the exact original direct or
   `COMSPEC /c <tail>` form selected by COMMAND, then its `cmdExec32` worker
   route. This includes host built-ins such as `ver`, plus compound shell
   syntax.
3. **DOS:** launch the exact current product executable, obtained through
   `GetModuleFileNameA`, followed by the normal positional DOS target and its
   copied arguments. The child follows the same existing pure-DOS profile and
   returns its ordinary process exit code to the unchanged parent worker.
4. **Win16:** use the same ordinary product invocation and a Win16 profile. Before the
   later DOSX/WOW bootstrap is complete, return an explicit recorded
   unavailable result; never fall through to host `cmd.exe` or its unsupported
   16-bit dialog.
5. **Compound or unresolved:** do not guess at quoting, expansion, pipes,
   redirection, batch syntax or a non-resolvable first image. Preserve the
   native COMSPEC branch unless a simple legacy image is positively resolved.

Classification first retains the reached `GetBinaryType` contract. The single
`mvdm_image_classify_command_line` helper is shared by direct app entry and
the `54:08` child-launch adapter, so DOS COM/MZ, Win16 NE and native PE have
one resolved-file result at both product boundaries. A bounded adapter-header
fallback may distinguish those formats only when public Win32 cannot. It
operates on a resolved host file and copied command text, never on guest
pointers and never as a guest parser.

## Work Sequence

1. Map the original `IsFirstCall` / `GetNextVDMCommand` transition, the
   first-PermCom PE -> `/z` -> BOP route, the later `/c` route, their
   error/CF outcomes and copied child process command before writing a route.
2. Test simple COM/MZ, NE and PE classification plus built-in/compound
   non-interception.
3. Launch one selected DOS workload from a second guest `COMMAND.COM`; prove
   parent worker wait, original `54:0B` result and return to parent prompt.
4. Exercise Win16's explicit unavailable result. The later DOSX/DPMI/WOW
   package changes only the Win16 child profile and proves one NE workload.
5. Re-run the original native `cmd.exe /c exit 37` proof unchanged.

## Completion

- Native Win32 remains on the untouched original direct-or-COMSPEC worker
  branch selected by the source body.
- A simple guest DOS command launches a separate product child, not a host
  unsupported-16-bit path, and returns through original `54:0B`.
- Win16 is either bootstrap-proved or explicitly unavailable; it never becomes
  a native host launch.
- `COMMAND.COM` remains unmodified; no app parser, guest-memory injection or
  cross-process pointer/HANDLE transfer exists.
- Formal CPU40/x86 build, focused route tests, fixed-container observation,
  `git diff --check` and mirror divergence registration pass.
