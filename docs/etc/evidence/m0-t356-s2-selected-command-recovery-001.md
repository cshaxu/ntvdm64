# M0 T356 S2 — Selected original DOS command recovery disposition

## Selected immutable workload

The selected first workload is the package's existing
`mvdm/system32/COMMAND.COM` with the declared command text `EXIT`.  App's
unchanged `launch_declaration.c` publishes the original Base-VDM shape:

```
application = <selected mvdm root>\system32\COMMAND.COM
command     = /C EXIT\r\n
environment = COMSPEC=<application>\0PATH=<selected mvdm root>\0\0
```

The staged runtime package already contains `COMMAND.COM`, `NTIO.SYS` and
`NTDOS.SYS`; no new guest payload is copied, generated or modified.  This is a
local DOS `.COM` workload selected from existing product media, not an ambient
host command or a replacement executable.

## Four-rung recovery review

1. **Original source reuse — selected.** Guest COMMAND
   `tcode.asm:557-623` consumes `SVC_CMDGETNEXTCMD`; guest `tmisc1.asm:389-510`
   describes the Base-VDM supplied fully qualified application/type and enters
   ordinary `INT 21h` execution; NTDOS `msproc.asm:$Exec` owns child and parent
   state.  The selected guest source files are exact OpenNT mirrors.
2. **Same-shaped binding — selected only where necessary.** Existing Base VDM
   local/broker code supplies the original `VDMINFO` request/copy/capacity/wait
   contract without CSR transport.  Existing session guest-location and
   CPU40 outer-execution bindings are the only current host mechanics used by
   the source bodies.  They retain named source interfaces and do not encode
   COMMAND or NTDOS behavior.
3. **External-code intrusion — not needed.** No guest mirror, CPU40 source or
   imported OpenNT body changes are required to select the first command.
4. **New behavior — not permitted or needed.** No new command parser,
   executable loader, PSP implementation, parent-return logic, host-child
   result or BOP handler is introduced.

## Critical source distinction

`tmisc1.asm:390-410` proves the intended Base-VDM behavior: a filled VDMINFO
provides the qualified application path and executable type, which guest
COMMAND uses once without PATH search.  Therefore the `COMMAND.COM /C EXIT`
record is a genuine guest COMMAND/NTDOS execution request.

By contrast, host `cmdExec` (`cmdexec.c:618-680`) is explicitly the
non-DOS-binary route.  S2 did not call, alter or use it as an execution
surrogate.

## Result and handoff

No selected-cohort source binding is absent at this static layer.  S2 closes
as a no-new-shim recovery disposition: existing original sources and existing
same-shaped composition own all immediate edges.  This is code/source closure
only.  S3 must validate the source-shaped declaration/local broker behavior
and produce a fresh formal CPU40/x86 link; only S4 may make the one fixed
integration observation.
