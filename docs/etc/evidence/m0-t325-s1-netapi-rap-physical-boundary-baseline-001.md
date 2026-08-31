# M0 T325 S1 — NetAPI/RAP Physical Boundary Baseline

## Scope

The reviewed owner cohort is the complete reached NetAPI/RAP surface in the
original `mvdm-host/vdmredir/vrnetapi.c` and `vrremote.c`, with the original
`cmdredir.c`/`vdmredir.h` caller and ABI context and the selected original
`opennt-host/netapi/netlib/ntstatus.c` status mapper.

`vrnetapi.c` source SHA-256 is
`9074aa1c5d287996c95547c08b7e06dac578a29d90aa3b91278c00bbe7392a70`;
`vrremote.c` is
`f75d2984624cee1a77b6f5bf6a18fe4a7f38a059d32b0e711edb97d03a08c9b8`.

## Findings

- Three small public NetAPI families are candidates for source-shaped modern
  bindings: workstation information, workstation user information and their
  paired `NetApiBufferFree` lifetime. They require proof of original string,
  buffer and status behavior before selection.
- `NetpNtStatusToApiStatus` is already a selected byte-identical original
  source slice. It remains the sole status-map algorithm; an adapter may bind
  only its modern status primitive, not replace its mapping table.
- `NetpCopyTStrToStr` is an original header macro to `NetpCopyWStrToStr`, not
  a missing host runtime. Its code-page boundary remains a narrow binding
  issue, not a reason to import a generic Netlib package.
- `XsNet*`, `RxNetUserPasswordSet` and `RxpTransactSmb` are downlevel
  Xactsrv/Rx/SMB-RAP protocol interfaces. Their request layouts and status
  rules are original source evidence, but a public API with the same name or
  broad purpose is not layout/lifetime equivalent. They therefore remain
  explicit unavailable/transfer edges, not fabricated network success.

## Result

The accompanying physical-edge ledger enumerates all reached external network
call groups and gives each exactly one source-first disposition. It proves
that the next recovery S can concentrate on the bounded public workstation
query bindings and original status mapping without importing an SMB/RAP stack
or silently expanding into NetBIOS, DLC, VDD, RPC, CSRSS or kernel facilities.

## Current Binding Correction

Historical T290 fixtures showed `VrGetComputerName` and bounded
`VrGetUserName(BX=1)` on both architectures, but they used the now-retired
`mvdm_redirector_pointer_scope` experiment. They are historical evidence, not
the current production binding. The retained original bodies remain valid
recovery candidates; the next S must invoke them only through the existing
`mvdm_guest_location` synchronous lease API at their actual `ES:DI` crossing.
That preserves the real-mode numeric address and uses the session's existing
guest-memory mapping manager without reviving a Redirector-private mapper or
pointer-scope lifecycle.
