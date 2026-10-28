# DEM FCB metadata: guest source paths and actual caller ownership

The original retained DOS source dispatches INT 21h AH=0F to `$FCB_OPEN`, AH=16 to `$FCB_CREATE`, and AH=23 to `$GET_FCB_FILE_LENGTH` (`dos/v86/doskrnl/dos/mstable.asm:176,183,210`). Open/create's `fcbio2.asm:538-564` calls `SVC_DEMCREATEFCB` or `SVC_DEMOPENFCB`, checks CF, and only on success stores returned AX:BP in SFT and FCB; `fcbio.asm:199` requests `SVC_DEMGETFILEINFO` for the record-count query and checks CF before using BX:DI size. `demdisp.c:144-149` routes those services to `demCreateFCB`, `demOpenFCB`, and `demGetFileInfo`. This is source-level route evidence, not proof that the immutable guest binary deployed now has these exact source bytes. The T420 S33 witness covered guest FCB open/read/close in its documented historical package; it did not cover these injected metadata errors or file-length query.

The new [caller fixture](dem-fcb-callers-fixture.c) calls the actual selected x86 `demfcb.obj`'s `demOpenFCB` and `demGetFileInfo` directly with controlled OEM open, metadata and close callbacks and a mocked register/error boundary. Like the [metadata-only test](dem-fcb-metadata-failure-review.md), its runner makes a temporary symbol-rebound copy of the selected object; it does not rebuild or modify product or guest. The runner and [recorded JSON](dem-fcb-callers-observation.json) capture hashes and outputs. Each case uses a fake acquired handle 0x1234. The initial CF is explicitly 0; the fixture does not execute DOS dispatch.

| Actual-object direct call | Controlled metadata | Recorded acquisition / close / error | CF / AX / BP after call |
| --- | --- | --- | --- |
| `demOpenFCB` | success | 1 / 0 / 0 | 0 / 0000 / 1234: handle published |
| `demOpenFCB` | DOS-time conversion FALSE, LastError=5 | 1 / 0 / 1 | 1 / 0005 / EEEE: no publication or close |
| `demGetFileInfo` | success | 1 / 1 / 0 | 0 / 0000 / EEEE |
| `demGetFileInfo` | same conversion failure | 1 / 1 / 1 | 1 / 0005 / EEEE |

This is stronger than the helper-only test: with controlled acquisition and conversion failure, the **actual object caller** takes different cleanup paths, and the open caller neither closes nor publishes its acquired fake handle. It is still not a real host-handle count, installed-release binding, or live guest reachability test. CF/AX failure in this fixture comes from the mocked `demClientError`, not a test of original error translation. `demGetFileInfo` does not itself clear CF on success in this direct-call setup; we initialized it to 0 and make no assertion about its caller's incoming flags.

Original `demClientErrorEx` reads LastError, maps zero to `ERROR_ACCESS_DENIED`, writes AX for ordinary errors and sets CF; hard errors inspect the still-live native handle for the drive, populate the hard-error packet and save retry state before setting CF. Therefore translation must happen while the handle is valid; closing before that can change error/drive behavior. A proposed single-owner repair is to make the metadata helper consistently **borrow** the handle, correct `GetFileTime`'s BOOL check, check local-time conversion, and let both callers close their own unpublished handles exactly once after translation; published open handles remain owned by guest/`demCloseFCB`. Remove the helper's special internal close consistently with the other caller, not only by adding a caller close. Preserve the exact saved error before any close, and resolve a defensible error mapping for failure APIs or oversized files that do not set LastError; no arbitrary semantic policy is approved here. `GetFileSize` FFFFFFFF requires distinguishing zero versus nonzero LastError, as independently demonstrated in the helper fixture.

S12 acceptance requires controlled success and each metadata failure through both actual callers, real `demClientError` normal/hard-error/retry mapping and live-handle ordering, exactly once close or guest publication, valid/error FFFFFFFF and high size, and then a version-bound actual guest integration where safely reproducible. Do not count mocks, historical success or source call routes as that full acceptance. Current selected `demfcb.obj` SHA-256 `40550EA098C987667A0114709625B57D814AD25EB14E4286B5D644188ADC872D`; the fixture, temporary rebound-object and probe EXE hashes are recorded in JSON. The i686 linker emitted the existing `.drectve` warning but succeeded.

Replay (read-only product, writes disposable probe under `build/t473-s10/fcb-callers-probe`):

```powershell
python artifacts/mvdm-audit/host-ch-scope/dem-fcb-callers-run.py
```

No production source, guest image or public ledger was changed.
