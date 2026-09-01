# M0 T343 S3 P3 — Copied-record formal closure

## Selected cohort result

S3 is closed for the selected DOS BaseVDM cohort.

- `broker/base_vdm_record.*` owns the versioned, copied, fixed-width record,
  same-record pending/second-empty-call result, peek/copy/consume ordering and
  disconnect.
- `adapter-mvdm-host-out/basesrv/base_vdm_broker.*` is the sole bridge from
  the app's selected launch declaration to the existing source-shaped local
  BaseVDM record.
- The original `GetNextVDMCommand(VDMINFO *)` client spelling remains the
  caller-visible boundary; neither it nor MVDM source receives a broker,
  pointer, handle, guest address or mapping-manager value.

## Verification

Both x86 and x64 formal CPU40 graphs passed:

- `broker-base-vdm-record-test.exe` — registration, pending, second empty
  request, copied delivery, DOS/WOW rejection, cross-record rejection and
  close-once disconnect.
- `basesrv-base-vdm-broker-test.exe` — selected adapter publication, failed
  local-destination delivery without consumption, later same-record delivery,
  copied command bytes and teardown.
- `original-softpc-candidate` — all selected broker, adapter and app objects
  compile into the paired candidate libraries.

The full `original-softpc-forced-closure.dll` audit now executes rather than
passing a literal redirection token to `link.exe`. It includes `broker.lib` on
both architectures. Its x86 `/FORCE` diagnostic has exactly two unrelated
original SoftPC display-path imports: `nt_sec.c::NtAllocateVirtualMemory` and
`NtFreeVirtualMemory`. They are a retained **SoftPC display/regen-memory owner
transfer**, not a broker gap: the original code directly releases and reclaims
the fixed `0xA0000` host virtual range for fullscreen regeneration. It must
later be recovered through the original display/memory package and a
same-shaped modern binding, with the session mapping manager where guest RAM
is involved. It is not legal to solve that path by adding a broker pointer or
by changing this selected BaseVDM cohort.

## Omitted forms remain transferred

CSRSS/BaseSrv server records and transport, PIF/separate WOW, shared-WOW
registration/notification, parent/child lifecycle, batch records,
cross-user/process control, raw resource transfer and real named-pipe/event
transport remain outside S3. The S2 ledger remains their authoritative owner
map.
