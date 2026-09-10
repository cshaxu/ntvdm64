# M0 T252 S1 — guest SFT opaque-token source/ABI audit

## Question

Does asynchronous Redirector I/O need a new guest-SFT token mechanism, or do
the imported v2 DEM routes already preserve the OpenNT producer/consumer
layout when the stored 32-bit value is an opaque token rather than a raw NT
handle?

## Inputs and procedure

- Read staged original `dos/v86/inc/sf.inc` and every `sf_NTHandle` reference
  in `doskrnl/dos/{file,handle,misc,lock,dev,abort,fcbio,fcbio2}.asm`.
- Read original `redir/namepipe.asm:MapNtHandle` and imported
  `bop/opennt/dem/demfile.c` plus its `demhndl_shim` and shared handle manager.
- Rebuilt and ran formal Ninja targets
  `t232-s1-host-handle-manager-fixture.exe` and
  `t230-s3-demfile-direct-import-fixture.exe` outside the sandbox.

## Source/ABI matrix

| Original role | Original source and register/layout contract | Current v2 route | Result |
| --- | --- | --- | --- |
| SFT storage | `sf.inc`: four-byte `sf_NTHandle` at the end of `sf_entry` | Layout remains unchanged in the guest image. | Retained. |
| Open/Create producer | `file.asm:203..219`: DEM service returns `AX:BP`; guest stores low BP then high AX in SFT. | Imported `demfile.c` preserves source ordering; `demhndl_publish_handle` returns one manager token in the same AX:BP order. | Code complete. |
| FCB producer | `fcbio2.asm:556..557` stores BP/AX in the same low/high layout. | Same v2 opaque 32-bit ABI applies. | Contract complete; no separate handle representation. |
| Read/write/seek/commit consumer | `handle.asm` loads SFT low word to BP and high word to AX before the DEM services. | `demhndl_get_handle(high=AX, low=BP)` reconstructs the token and performs manager lookup. | Code complete. |
| Redirector consumer | `namepipe.asm:MapNtHandle` loads low word to BX, high word to BP. | The required token is exactly `(BP << 16) | BX`; no host pointer is involved. | ABI complete; `57:23/24` still need later interrupt delivery. |
| Close/termination | `handle.asm`, `abort.asm`, `fcbio.asm` use the same split value before source close/release paths. | `demhndl_close_handle` releases the corresponding shared-manager token. | Local DEM lifecycle complete. |
| Historical VDD helper | `demfile.c:VDDAllocate/Associate/RetrieveNtHandle` directly maps guest SFT/JFT via `Sim32GetVDMPointer` and casts raw `HANDLE`. | Current named shim correctly declines this unavailable NT4 VDD/SAS composition; no active BOP route selects it. | Explicitly excluded, not a missing v2 route. |

## Four-rung recovery ledger

1. **Original source reuse:** staged guest `file.asm`, `handle.asm` and
   `namepipe.asm` already preserve the SFT offsets, write order and split
   register ABI; no guest overlay is required.
2. **Smallest shim:** `demhndl_publish_handle/get_handle/close_handle` and
   `bx_ntvdm_host_handle_manager` replace only the raw host value behind the
   same 32-bit guest field with a session-owned opaque token.
3. **External intrusion:** none; bx-core/bx-mantle are untouched.
4. **New behavior:** none.  The manager is the existing approved modern ABI,
   not a second SFT implementation.

## Observation and confidence

The formal manager fixture validates publish/lookup/release identity and
invalid-token rejection.  The formal imported DEM fixture validates the
source-mirror Open/Create publication path.  Static source inspection proves
the low/high word arrangements agree with `MapNtHandle`.

Confidence is high for the SFT token seam.  It does not prove functional async
pipe completion: that still needs the independently owned selector-blind
guest `int 5Ch`/ANR delivery seam.  T252 S2 therefore has no source change to
make; its only correct disposition is to retain the existing v2 seam and
remove SFT publication from the async-pipe blocker list.
