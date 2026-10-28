# Original input, keyboard-code and NLS owner reconciliation

This S10 checkpoint follows `010952cdd`. It reviews five pending host source
roles: **three partial integrations and two generic error-table file
exclusions**. No production source, original guest or runtime image changes.
An actual-object keycode bound defect is established below; it is not repaired
or waived by the passing audit consistency gate.

## Source and image ownership

The [observer](input-image-observer.py) and [observations](input-image-observations.json)
join five DOS16 catalog rows and ten OpenNT/OpenNT-4.5 WOW32 identities.
Both editions' original source bytes match their frozen hashes. Every current
cohort file exactly matches primary OpenNT. Catalog Git-blob fields are
retained metadata, not newly reverified index claims. Source retention is
100%, not behavioral completeness.

The original byte-identical NT host recipe selects nt_input, nt_keycd and
nt_nls. Actual Ninja compile/archive edges and VALID dependency records select
the same inputs. **Six mapped functions** match all non-relocated code bytes
at their actual candidate addresses; complete executable sections match the
cached map-producing worker. This does not validate every call relocation.

Additional checks bind the six actual nt_keybd_funcs initializer targets,
all **217 original scan-table bytes** and the real light-on routine's REL32
call/tail-jump target to SyncToggleKeys in nt_event.obj. Original keyboard
table slots 0..3 select the same empty nt_kb_prepare; slots 4..5 select
nt_kb_light_on. These empty hooks are original NT choices, not new project
success stubs or proof that the whole keyboard initializer is absent.

| File | Disposition and actual owner | Remaining behavior/lifecycle proof |
| --- | --- | --- |
| nt_input.c | Partial. Original KEYBDFUNCS and LED change-bit policy delegate synchronization to nt_event; ToggleKeyState is not independently changed by this leaf. | Actual source-object tests cover 2,048 state/pattern combinations with a recording callback; real device/BIOS/Console synchronization, nesting, reset and retirement remain S13. |
| nt_keycd.c | Partial. Original set-1 normal/enhanced mapping feeds nt_event key-down/up callbacks. BIOS return conversion uses actual public OEM/layout services. | The exact-length scan defect remains S13. All locale/DBCS/high OEM, Alt-numpad, failed host conversion/mapping and real guest queue/retirement cases are not proved. |
| nt_nls.c | Partial. Original initialization/lookup uses selected production resources and public USER/system message services. nt_error supplies positive EHS_MSG_LEN. | Present/missing string capacity tests are bounded; missing initialization resources and noncontinuable failure, other locales and actual error/dialog/worker lifecycle remain untested. No valid zero/negative-capacity caller is established. |
| nt_ertbl.c | Excluded generic severity/variant table file. Original NT recipe omits it and nt_error supplies original NT behavior without host_errors. | Error capability remains at the selected partial NT owners; generic table-driven severity/ErrData behavior is not silently installed over original NT behavior. |
| nt_mess.c | Excluded generic compiled-string table file. Original NT recipe omits it and nt_nls uses original resource strings, including both old message texts. | Localization, actual dialog/failure and retirement remain independent obligations, not exclusions of message capability. |

These table exclusions are not based solely on absent map names: both are
omitted from the original NT recipe, current compile/archive graph and image;
the actual original nt_error caller delegates to nt_nls; original
obj.vdm/resource.rc and its selected generated resource retain EHS_FUNC_FAILED,
EHS_SYSTEM_ERROR and mouse-menu strings. Original nt_error_ext itself ignores
ErrData and calls host_error. This is the original NT owner boundary, not a
project substitution of generic error behavior.

The [per-file ledger](input-native-role-dispositions.csv) preserves these
chains, original identities and limits. Whole restored-code percentages are
explicitly unmeasurable from these bounded witnesses; no branch/lifecycle
ratio is invented from the six mapped functions. No mirror diff or exception
is added, and all already-different mirror files remain partial.

## Actual-object input and NLS contracts

The [keycode fixture](../../tests/mvdm-host/keycode_object_contract_fixture.c)
links actual selected nt_keycd.obj and nt_input.obj, not copied algorithms.
It uses the original KEYBDFUNCS type and real KEY_EVENT_RECORD, with x86 ABI
checks. A SyncToggleKeys recording seam owns only fixture state; no real LED,
keyboard layout or input queue is changed. BIOS conversion uses real public
OEM/layout services. It passes **268,322 assertions / 1,184 callbacks**:

- All 16-bit scan values in normal/enhanced contexts, except the two exact
  length cases reserved for the strict failure witness below. In-range table
  contents are data inputs to this selector test, not an independent oracle
  for every keyboard mapping. Representative Enter/Ctrl/F1/Home mappings have
  explicit semantic expectations. Input records must remain unchanged.
- All eight initial toggle states by all 256 light/change-bit patterns;
  unrelated modifiers are preserved, callback count/value is checked, and
  the leaf does not create a second ToggleKeyState authority.
- Eight independent BIOS cases cover Shift/Ctrl/Alt F1, F0 normalization,
  Ctrl-enhanced remapping and keypad divide/Enter/Ctrl-Enter. Record fields
  and surrounding canaries are checked. Other conversions remain open.

The [NLS fixture](../../tests/mvdm-host/nls_object_contract_fixture.c) links
actual nt_nls.obj and the production softpc-resource.res. It uses actual
LoadString/GetModuleHandle/FormatMessage, not fake resources or a success
provider. **3,645 assertions** cover initialization, three existing strings
at every capacity 1..260, missing-string fallback at those capacities, correct
NUL termination, truncation and neighboring canaries. FormatMessage output is
compared in the current system language; no host language is changed.

The [shared runner](../../tests/observation/verify-softpc-object-contract.ps1)
adds two selections with unchanged existing contract branches and library
defaults. Both wrong-size compile negatives fail at C2118. A fresh IOS
control passes 70 assertions and its wrong-ABI compile negative. A first
keycode invocation passed but emitted a fixture-only signed/unsigned warning;
the comparison was corrected and the final suite rebuilt/retested it. That
initial record is not substituted for the final current-source receipt.

## Proven one-past key-table read

Both original KeyMsgToKeyCode branches compare `scan > sizeof(table)`;
the valid indices actually end at `sizeof(table)-1`. Normal size is 133,
enhanced size is 84. The current linked candidate contains those original
code/table identities, not a repaired comparison.

The separate `--boundary` invocation uses the same actual-object fixture.
Its unmodified one-past bytes both read zero, which can hide the mistake.
In **this fixture process only**, ReadProcessMemory/WriteProcessMemory
temporarily places 123 in each exact adjacent byte, calls the original
routine with scan 133 or 84, then restores that byte and reads again:

```text
KEYCODE_BOUNDARY enhanced=0 scan=133 before=0 sentinel=123 observed=123 restored=0
KEYCODE_BOUNDARY enhanced=1 scan=84 before=0 sentinel=123 observed=123 restored=0
KEYCODE_BOUNDARY expected_rejected=2 actual_rejected=0
```

The strict rejection contract returns **exit 1**, deliberately retained as a
failure. The supervisor expects that documented failure as evidence; it does
not label the runtime contract fixed or all input tests passing. No production
process or original guest is modified, and there is no hook/input injection,
Computer Use or UI automation. This proves a controlled original-source
out-of-bounds read, not a naturally occurring bad key, crash or guest symptom.

S13 owns the finite repair: change both range comparisons to reject equality,
preserve the mapping arrays, valid scan behavior, input ABI and original
translation flow; register the minimal mirror exception. Re-run the strict
boundary witness expecting exit zero and both requests rejected after repair, all normal/
enhanced scan values, BIOS conversion and actual DOS/WOW input/reset/retirement
regressions. Trace the real Console/worker caller before claiming a user
symptom. No guest workaround, fabricated key, new filter engine or native
keyboard-state manipulation is justified.

## Delivery boundary

The exact five-row gate verifies source/image/catalog identity, three final
object receipts and wrong-ABI results, the explicit boundary non-pass and
restoration evidence, frozen inventory, prior full rows and unrelated shards.
Totals become **1,550 pending, 444 partial, 53 full and 2,637 excluded**.
Production images remain unchanged. The [worklist](recovery-worklist.md)
retains the defect and remaining input/NLS boundaries. This does not qualify
full guest input/localization, the held SOL candidate, S10 or T473 closure.

The [delivery result](input-delivery/stdout.txt) passes source/image and
[70 captured metadata hashes](input-runs/manifest.json), the exact five-row
delta, bounded normal/ABI results, explicit strict-bound non-pass and data
restoration, preserved ten-image packages and documentation/link/diff gates.
Use the [delivery verifier](input-native-delivery-verification.ps1) in the
required owned background context; historical fixed-count gates belong to
their named checkouts. Generated native media stays in disposable build storage.
