# DEM FCB actual callers: ignored metadata-failure experiment

This independent host fixture invokes selected `demfcb.obj`'s real `demOpenFCB` and `demGetFileInfo` with fake OEM file acquisition, metadata APIs, registers and error callback. The runner copies the selected object and rebinds five import symbols for the fixture; it does not rebuild or change product or guest. [Fixture](dem-fcb-ignored-errors-fixture.c), [runner](dem-fcb-ignored-errors-run.py), and [recorded output/hashes](dem-fcb-ignored-errors-observation.json) are retained. Selected object SHA-256 `40550EA098C987667A0114709625B57D814AD25EB14E4286B5D644188ADC872D`; current `demfcb.c` SHA-256 `DC69282B091A5B2216F85B3F70F7D9D493FF6AA40A3ECFAA1E86EC1D4D354155`. This does not separately prove that object is the currently installed release.

Each actual caller was exercised for (0) all-success control, (1) `GetFileTime` FALSE, or (2) `FileTimeToLocalFileTime` FALSE. Both failing fake APIs deliberately supply deterministic output bytes anyway, so subsequent paths can be isolated; real failed APIs do not promise that output. For all six cases the selected object continued with no error callback and CF remained or became zero. In the open caller it published fake handle AX:BP `0000:1234` and did not close it; in the info caller it closed the fake handle once. Thus the helper-level ignored errors previously seen are now observed through **both selected actual-object callers**, including publication and differing handle ownership. This is a controlled counterexample, not proof that a real guest or OS failure reaches the branch or that its failure return maps to a particular DOS code.

The baseline runner asserts these **observed defects** to make a pre-repair comparison reproducible; it must be changed to positive failure/ownership assertions after repair. It does not call original `demClientError`: the fixture supplies a register-setting seam, which was not called in these cases. Linker emitted a `.drectve` warning but succeeded. S12 still must test original LastError/hard-error mapping, real/native errors, one close vs published handle, output buffer undefinedness and version-bound immutable-guest behavior; see [ownership and guest route](dem-fcb-callers-reachability-review.md) and [real sentinel boundary](dem-fcb-error-sentinel-review.md).

Replay from the repo root with the recorded x86 worker object and bundled i686 MinGW:

```powershell
python artifacts/mvdm-audit/host-ch-scope/dem-fcb-ignored-errors-run.py
```

Run outputs are temporary under `build/t473-s10/fcb-ignored-errors-probe`. No previous blocked probe or product/guest mutation was used.
