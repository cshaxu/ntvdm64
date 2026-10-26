# M0 T463 S1 — WOW32 everyday baseline

## Frozen package

The S1 ordinary package is `build/t461-s3/product-r1/runtime`; guest media is
read-only.  SHA-256 inputs are: `WINMINE.EXE`
`633C1328922B4092351E0019B1BA1C6D56C0F0FEA83824CC8B7E2E731B15E80`,
`SOL.EXE` `68B196C6FD496BAC0E2F30F162EE0B4B4D6EB5BFEC1AF086C8EA759222211F31`,
`WRITE.EXE` `EB44B5D5F5A405B6A8CD018B9F790BCA2E7E895DAD4F7E1F5462EC14CCDAA87A`,
`WOW32.DLL` `0E73912D4F9DB609A8DA825956090DD3931FF0B076AC81ADDC832B0D7D0D5E84`, and
`ntvdm.exe` `D77E9F7D1622BA53F1CAD23F46A15E2B24A6F76D50D43AD382D8CC4E5DD1799D`.

## Application ledger

| App | Existing proven frontier | S2 required behaviour | First owner / disposition |
| --- | --- | --- | --- |
| WINMINE | Current no-input private-desktop run has visible native frame, patterned board/counter and unchanged title. | Reveal/flag/timer/menu/new game, repaint and ordinary close/restart. | Consume W07 drawing and W06 input/message owners; no failure yet assigned. |
| SOL | Startup is distinct from usable play; prior owner report identifies the Options route as a `WOW32.DLL` access violation. | Deal, legal move, draw/new game, Options, repaint/resize and close/restart. | Trace the Options call to its exact original WOW32 owner before any fix; do not substitute host UI. |
| WRITE | Prior startup is not edit/save evidence. | Read/edit/select, menu/dialog, Clipboard, save/reopen, cancel and close/restart. | Trace each first failing operation to W03 EDIT, W09 menu/resource, W10 Clipboard, W18 common-dialog, or W04 file owner; no acceptance by window existence. |

## Preconditions and exclusions

T462 did not reproduce the alleged residual strip and admits no visual repair.
The accepted W04/W05/W06/W07/W08/W09/W10/W12 results are inputs only; their
unaccepted extended families (OLE, device/printing, DDE, COMM, sound, Winsock
and ToolHelp) remain separate Queue owners unless an observed first failing
call proves one is required.  No guest mutation, new queue, mirror replacement
or autonomous compatibility UI is admitted.

## S2 contract

S2 begins with source tracing of SOL Options, then performs one coherent batch
only if its exact original owner and lifecycle/ABI contract are known.  It does
not repair WRITE or WINMINE merely because their scenarios are in the same
acceptance milestone.  Every changed edge must meet the minimum-diff admission
governance and retain normal, failure and retirement evidence.
