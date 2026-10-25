# M0 T454 S2 — resource load/lock witness

## Scope and conclusion

S2 closes the selected original Find/Load/Lock/Unlock execution boundary.  It
found no missing product binding and made no runtime-source, guest-media,
release-image or divergence change.  The only source edit is an owned test
parser repair: `verify-wow-mine-gameplay.ps1` now parses the already-authoritative
`CDL2` report emitted by `win16_common_dialog_probe.asm`, rather than the
obsolete `CDL1` three-record layout.

The selected original providers remain the authority:

- `wres32.c` dispatches USER resource calls;
- `wres16.c` owns `RES`, the guest callback routes and copy/alias distinction;
- `wcall16.c` provides the real guest lock bridge; and
- the immutable WOW16 callback shim carries the fixed-width results.

No replacement resource manager, PE fallback, guest edit, accelerator
fallback, or new alias allocator was introduced.  W03 remains the sole
allocation and alias owner.

## Executed evidence

The existing x86 linked provider fixture completed on 2026-10-24:

```text
WOW_ORIGINAL_MENU_RESOURCE errors=0 resource_cases=16 parser_cases=7 invalid_repeats=64
WOW_OWNER_DRAW_RESOURCE errors=0
WOW_ORIGINAL_MENU_THUNKS errors=0 cases=6 allocations=0
WOW_RESOURCE_MENU_CCPU registered=1 nested=1 rollback=1
WOW_RESOURCE_EXECUTION_SCOPE ready=1 restored=1 no_scheduler=1
WOW_FIXTURE_OK case=menu-resource
```

It is bounded fixture evidence for the selected provider/consumer parsing and
negative paths; it is not presented as the guest proof.

The real immutable-guest run used a build-owned package copied from the
current deployed runtime, then the existing physical short-path stage helper.
The test was run on the private test desktop with:

```text
verify-wow-mine-gameplay.ps1 -RetiringSibling
```

Its retained evidence is `build/t454-s2/guest-resource-r4/`.  The current
`CDL2` report has four 24-byte checkpoints:

| Checkpoint | Phase | Created | Closed | Hooks | Relevant observation |
| --- | ---: | ---: | ---: | ---: | --- |
| Task initialized | 0 | 0 | 0 | 0 | task `1737h` exists |
| Owner installed | 1 | 0 | 0 | 0 | FindText has not yet run |
| First dialog destroyed | 1 | 1 | 1 | 1 | `DestroyWindow=1`, `IsWindow=0` |
| Second dialog left for task exit | 2 | 2 | 1 | 2 | live dialog retained for original task retirement |

The test passed:

```text
PASS same-worker sibling normal exit and resource cleanup; surviving WINMINE flag/reset/open/close
PASS flag/counter, reset, open cell, WM_CLOSE and run16 completion
```

`lifetime.txt` additionally records the selected callback return codes
`0014` and `0012` before the sibling task's one and only
`TaskRetirement ... stage=complete status=0`.  The WINMINE survivor continued
through flag, reset, open and close.  Thus this is a real provider/guest
exercise of normal lookup/load/lock/unlock plus the normal consumer cleanup
edge; it does not close S3's dedicated free/rollback matrix.

## Parser correction

The old assertion expected three 18-byte `CDL1` records.  The probe has
emitted `CDL2` since T447 S2: it adds guest-observed `DestroyWindow` and
`IsWindow` results, and records both the initial task checkpoint and the
owner-creation checkpoint.  The corrected parser now requires exactly four
24-byte `CDL2` records and asserts the extra two result fields.  It is stricter
with respect to current probe semantics and does not change any product path.

## Handoff

S3 owns only the remaining finite original free/rollback/retirement work:
host-to-guest and guest-to-host result normalization, partial-load retention,
raw producer-key `DestroyRes16` matching, repeated operation and failed
callback cleanup.  MENU/DIALOG drawing and complete common-dialog feature work
remain later consuming scopes.
