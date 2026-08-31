# M0 T318 S2 P51 — Original SCS pre-NTIO owner cohort

## Cohort

The fixed-container startup scan now covers the complete original `scs_init`
precondition set, rather than letting the next observation nominate one of its
callees as a trace-selected patch:

```
InitialiseDosEmulation
  -> SetupInstallableVDD
  -> scs_init
     -> GetNextVDMCommand(NULL)
     -> CMDInit
     -> DemInit
     -> XMSInit
     -> DBGInit
  -> original NTIO file load and CPU entry
```

## Source owners and outcomes

- `GetNextVDMCommand(NULL)` has the existing source-shaped Base VDM client
  binding in `adapter-mvdm-host-out/basesrv/source/base_vdm_client.c`.  It
  preserves the original request/result structure and retry outcome while the
  local Base VDM dispatcher replaces only private CSR capture/transport.  The
  S2 admitted `--ordinary-child` declaration is already bound before
  `scs_init`; this is not a new COMMAND route.
- `mvdm-host/dos/command/cmd.c::CMDInit` is original and only establishes the
  command home drive from `GetSystemDirectory`.  It is a directly usable public
  Win32 operation and cannot terminate this path.
- `mvdm-host/dos/dem/dem.c::DemInit` remains original except registered
  `MVDM-HOST-DIV-154`: the immutable session DOS-media root initializes the
  original caller buffer.  It retains original `SetErrorMode`, argument scan
  and failure direction.  P2 established this media-root binding.
- `mvdm-host/xms.486/xms.c::XMSInit` is the original allocator/control flow
  with registered `MVDM-HOST-DIV-132`: both host widths select its original
  callback sequence through the session guest-memory backend rather than the
  historical host-pointer branch.  P49 verifies its selected guest HIMEM/UMB
  cohort and formal graph membership.
- The full original NT4 debugger package needs private VDM debug transport.
  The reached `DBGInit(argc, argv)` has the deliberately narrow same-shaped
  `adapter-mvdm-host-out/debugger/source/dbg_init.c` binding: it observes the
  public NTDLL debug-port result and preserves successful non-debug startup.
  It does not claim private debugger dispatch or alter CPU/guest behavior.
- `SetupInstallableVDD` is still original `nt_msscs.c` registry/query/load
  control flow.  Registered `MVDM-HOST-DIV-148` gives exactly one modern
  disposition: absence of the retired global installable-VDD registry key
  selects an empty VDD set.  Other registry/load failures retain the original
  dialog/error direction.  No VDD is fabricated.

## Formal composition

The selected formal x86 graph includes the original COMMAND, DEM, XMS and host
archives, together with the selected Base VDM, debugger, VDD-registry and XMS
mapping adapter objects.  These are original owner bindings, not a substitute
guest loader, BOP handler, or session-owned execution state machine.

## Result

There is no remaining unclassified early terminal branch in the original SCS
pre-NTIO cohort for the declared ordinary-child profile.  The next owner
boundary is therefore not another SCS leaf: it is the original NTIO image
load/entry transaction and the first reached guest initialization marker.

The current product is unchanged by this static conclusion, so P50's fixed
container is intentionally not rerun.  The next observation requires a newly
linked product arising from a source-shaped repair within that next owner
cohort; it must otherwise retain the same r33 media, firmware, arguments,
console ownership and eight-second watchdog.
