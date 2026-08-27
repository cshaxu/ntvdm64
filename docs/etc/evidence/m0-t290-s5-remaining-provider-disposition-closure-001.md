# M0 T290 S5 — remaining Redirector provider disposition closure

## Question

Can every remaining NetAPI, NetBIOS, DLC, window and mode provider in the
original Redirector package receive a source-first S5 disposition without
inventing a Lanman server, network emulator, private kernel bridge or BOP
route?

## Inputs

- Exact original `src/mvdm-host/vdmredir/vrnetapi.c`, `vrnetb.c`,
  `vrdlc5c.c` and `vrinit.c` mirrors.
- The S5 source/ABI audit and the S1 dispatch ledger.
- Local original-body fixture evidence P2, P3 and P4.

## Procedure

The disposition matrix enumerates each S5-owned `57:xx` entry, identifies
the reached original external contract, and gives it exactly one conclusion.
No selector is enabled.  An entry is called composed only when its original
body has focused x86 and x64 execution evidence; an unavailable entry keeps
the original source body but does not claim a fabricated result.

## Result

- Six local original bodies are composed: the four original
  `ERROR_NOT_SUPPORTED` results, both original assignment-mode no-ops, and
  the bounded computer-name path.  The bounded username form is also
  composed, with its source-defined `BX=0` no-capacity form explicitly
  unavailable.
- All remaining NetAPI transaction/use/service/remote bodies require private
  Lanman `Xs`/`Rxp`/XactSrv contracts or source-owned output conversion for
  which the guest ABI provides no checked span.
- NetBIOS, DLC and VDM-window bodies require NetBIOS/DLC driver and physical
  interrupt/monitor contracts.  S5 does not recreate them.

## Verification

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools\build\Invoke-T290S5RemainingProviderMatrix.ps1 -RepositoryRoot O:\repos.hobby\ntvdm64
```

The formal external run regenerated and checked all six focused graphs:
`vrnetapi-local`, `vrnetapi-computer-name` and `vrnetapi-user-name`, each on
x86 and x64. Each fixture exited successfully. The runner also rejects a
missing or extra disposition row; it observed exactly 27 rows.

## Interpretation

This is a complete S5 source disposition, not a claim that all historical
network functionality exists.  Each unavailable branch has a named future
owner package and remains selector-disabled.  No new mapping manager, raw
native identity, direct Bochs call, CSR/BaseSrv emulation or new Redirector
policy was added.

## Follow-up

Run P2--P4 as one formal x86/x64 group and then update S5 status/closure.
