# Part 8: SHELL.DLL retained guest binary — exact NE thunk observation

The owned `wow16/shell/SHELL.DLL` is a concrete, bounded binary/interface observation, not a source-build or whole-Shell behavioral claim. The independent read-only `shell-ne-entry-observer.py` validates the full bytes of three copies: retained source-tree image, `assets/winnt.zip` `WINNT/SYSTEM32/SHELL.DLL`, and installed `O:/winnt/system32/SHELL.DLL`; SHA-256 is `B5C132C76B5106BE713B2E1B1E519EC9F540A39FCB8EFCA5CCAA42F16ECCA04F`. The existing part-8 inventory separately checks identical bytes against both frozen originals. This confirms installed/archive/prebuilt binary identity, not which DLL a past or current guest loaded.

The observer parses NE segment and entry tables, then validates callable entry bytes and the presence of a relocation at each call-ID immediate for five ordinals:

| Ordinal | Original entry | Segment:offset | Encoded argument bytes | Encoded ordinal and fixup |
| --- | --- | --- | --- | --- |
| 1 | RegOpenKey | 1:001E | 12 | 1 plus recorded fixup |
| 2 | RegCreateKey | 1:002B | 12 | 2 plus recorded fixup |
| 20 | ShellExecute | 1:00A0 | 20 | 20 plus recorded fixup |
| 21 | FindExecutable | 1:00AD | 12 | 21 plus recorded fixup |
| 22 | ShellAbout | 1:00BA | 12 | 22 plus recorded fixup |

Each observed entry begins with push argument-byte count, push zero high call ID, push its ordinal, followed by a far call. The ordinal immediate has a NE fixup record. Original `wow16/shell/shell.asm:93-107` uses SHELLThunk for these functions, `wow16/shell/library/shell.def` supplies the corresponding ordinals, and `src/mvdm/inc/WOW.INC:1011-1100,1156` specifies that thunk shape. This is a targeted source-to-image structural correspondence, not proof that this source built the image or that the unexamined fixup target is resolved correctly at runtime. The observer deliberately does not assume the on-disk immediate already equals MOD_SHELL: NE relocation remains a loader responsibility.

T447 historical `m0-t447-s2-scope-freeze.md:2016-2043` records a separate guest ShellExecute success, missing-target failure and child continuation in `frozen-shell-r1-guests`; the report says its runner recorded DLL hash, but that run's hash manifest is not currently found in this review. Therefore that runtime witness cannot yet be bound to the B5C132... image above. T463 product-r3 retains a SHELL.DLL with the same B5C132... bytes, but its WRITE-lifecycle witness does not test ShellExecute and its input manifest does not hash SHELL.DLL. Do not combine these to manufacture an end-to-end proof.

Suggested ledger decision for exact key `wow16/shell/SHELL.DLL`: **pending**. Deployed original image identity and five source-shaped ordinal entry bytes/fixup sites are proven, but the historical live load cannot yet be tied to this hash and the guest-image partial standard requires that binding; live relocation, actual Shell dispatch, registration, errors, nested callbacks and teardown remain unproved. The `shell.asm`, `shell.def`, `SHELL.RES` and other guest inputs remain pending as full source-to-binary lineage has not been proved. S14 should execute immutable real Shell callers with recorded loaded DLL identity and failure/cleanup; S19 binds them to the current coherent release. This status does not authorize guest modification or rebuilding.

Replay: `PYTHONDONTWRITEBYTECODE=1 python artifacts/mvdm-audit/nine-way/part-8/shell-ne-entry-observer.py`; compare JSON and original macro/definition. No guest, product, build or shared ledger changed; no runtime test was run here.
