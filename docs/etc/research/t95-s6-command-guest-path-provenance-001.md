# T95 S6 COMMAND guest-path provenance 001

## Question

Can the existing BYOB-verified `COMMAND.COM` artifact, or the historical
COMMAND BOPs, supply the guest pathname required by the `SHELL=` line returned
through `54:0C SVC_GETCONFIGSYS`?

## Inputs and procedure

This is a read-only source correlation of:

- `base/mvdm/dos/command/cmdconf.c:462-542`;
- `base/mvdm/dos/v86/cmd/command/init.asm:1121-1128`;
- `base/mvdm/dos/command/cmdmisc.c:635-650` and `cmdenv.c:309-321`;
- `base/mvdm/dos/command/cmdexec.c:249-257`; and
- the current source-built startup descriptor and observation records, which
  classify NTDOS and COMMAND as identity-only inputs.

No executable, build, host file, guest memory, Bochs source, or adapter source
was changed or invoked.

## Observations

1. The historical `ExpandConfigFiles(TRUE)` deliberately writes a `SHELL=`
   command that names `%SystemRoot%\\System32\\command.com`; its operand is
   therefore a file location later consumed by the DOS guest, not a host
   callback or a BOP target.
2. On permanent COMMAND startup, `init.asm` calls `54:02
   SVC_CMDCOMSPEC` with `DS:DX` pointing to the already established resident
   guest `ComSpec` string.
3. The historical `cmdComSpec` owner reads that guest string with
   `GetVDMAddr`, prefixes it with `COMSPEC=`, and retains it only for the
   historical host-side environment.  It does not load, map, locate, or
   validate a COMMAND executable.
4. The same retained COMSPEC text is later copied into the host-side command
   environment and SCS record.  These are downstream mirrors of the guest
   path, not a source of guest bytes.
5. Current ntdos64 BYOB admission validates the byte identity of
   `COMMAND.COM`, but its startup descriptor and source-built observations
   explicitly state that COMMAND is not placed or started.  The BYOB root is
   intentionally not a DOS namespace root.

## Interpretation

The dependency direction is:

```text
S7 guest artifact placement -> guest-visible COMMAND pathname
  -> CONFIG.SYS SHELL= -> DOS executes COMMAND
  -> COMMAND 54:02 reports COMSPEC to host compatibility code
```

`54:02` cannot be used to bootstrap the first arrow.  Treating it as a
COMMAND loader would reverse the original ownership direction and turn an
ordinary guest file lookup into an adapter BOP special case.

Consequently, an S7 first profile needs a separately declared guest artifact
placement/namespace contract that makes the selected, identity-verified
COMMAND payload reachable at a fixed guest path.  That contract must be
separate from `--byob-root`, from host-drive exposure, and from Bochs.  Only
after it exists can a CONFIG materializer state a source-compatible `SHELL=`
result or a deliberately new, labelled ntdos64 profile policy.

## Follow-up

Keep `54:02`, `54:0C`, `54:0D`, and the Open/Seek/Read/Close family listener
only.  The guest-side default execution path and minimum placement contract
are now recorded in
`t95-s6-guest-artifact-placement-contract-001.md`; implementation remains an
S7 prerequisite, not another BOP handler.
