# Shard 1: WOW MCI reverse-thunk and alternate call path

## Scope and disposition

`wow32/wmmstru2.c` remains **pending, not proven recovered or safely removable**. Its exact current bytes match both frozen OpenNT sources and the source reconciliation lists one DOS and two WOW catalog rows. The frozen production build compiles and links its object, but the original file's outer `#if 0` excludes the `UnThunkMciCommand16` implementation: the symbol is absent from both object and linked map. Inclusion of the object is therefore not proof of working MCI conversion.

## Evidence and architecture

The original WOW16 `mmsystem/init.c:73,613,633` loads `winmm.dll` and resolves `mci32Message`; `mci.c:915,972` issues `THUNK_MCI_SENDCOMMAND` via `mciMessage`. The assembly `thunks.asm:630–660` passes that resolved address and parameters to `MMCALLPROC32`. `wow32/wmmtbl2.h:18` names the corresponding WOW32 entry; `wmmedia.c:136` implements the generic call of a supplied 32-bit procedure. Project wrapper `src/wow32-dll/source/wow_user_native_extension.c:82` delegates to the renamed original. The observer binds both named procedures' object bytes to the released WOW32 code; it does not prove that a live guest takes the complete chain.

On the audited host only, the x86 `C:/Windows/SysWOW64/winmm.dll` export table contains `mci32Message`. Its identity and all inspected inputs are hashed in `wow-mci-image-observations.json`; this is not a cross-version compatibility guarantee or proof the export provides the expected NT4 contract. The observer checks identity, build inclusion, symbol absence, source call-chain anchors and image/object binding using only existing artifacts. Replay: `python artifacts/mvdm-audit/shard-1/wow-mci-image-observer.py`.

## Remaining boundary

No product execution, guest-image provenance verification, MCI command/device behavior, error fallback, pointer marshalling, callback ordering, cross-host compatibility or teardown was validated. The source-level alternate call route does not demonstrate that modern `winmm.dll` implements the historical MCI thunk ABI. Determine actual runtime resolution and representative Win16 MCI operation before accepting a behavioral disposition. No product, guest, shared ledger or build tree was modified by this review.
