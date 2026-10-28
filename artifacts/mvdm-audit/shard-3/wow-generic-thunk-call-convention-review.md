# S10 shard 3: generic thunk selected call-convention boundary

Read-only follow-up of selected `wow32/wkgthunk.c` and its original generic native call. No product modification, guest build or execution. The cached WOW32 `build.ninja` compiles `wkgthunk.c` with project wrapper renames, `/DCCPU /DSPC386` and no explicit `/Di386`; it contains no `callpr32` build edge. The cached `wkgthunk.obj` SHA-256 is `50E3000F28A6A6C943B7B521BDCD39E6DF1B61D2EC8C07A42C1D8599811C83B5`. `objdump -t` shows `_wow_original_WK32ICallProc32@4` and no `WK32ICallProc32MakeCall` reference; `objdump -dr` shows many indirect `call *%eax` within that function, consistent with the C switch branch. Cached map contains wrapper and original entry but no MakeCall. This is stronger than source-name inference, but does not bind the snapshot to the installed DLL or prove all compiler macro settings; preserve exact identity in any conclusion.

Original `wkgthunk.c:105-206`: checks no more than 32 arguments and nonzero target; converts parameters indicated by address bitmask in the guest frame, adjusts argument order for source calling convention, synchronizes DOS directory, then the `i386` branch calls `WK32ICallProc32MakeCall`. Its non-i386 branch instead calls target directly through C with different convention/order handling and a 0..32 switch. The original `i386/callpr32.asm:62-116` explicitly saves/restores ESP around the call to tolerate a target that does not restore the expected stack, documented as required by some Works 95 wizards. The observed selected object appears to use the C branch, so **that explicit stack-recovery contract is not established for this product profile**. This also contradicts the unqualified historical claim in `m0-t447-s2-scope-freeze.md:631` that the selected path necessarily uses MakeCall; the more recent `m0-t473-s1-unselected-host-source-disposition.md` treats that assembly as unselected. Neither document should be applied to all package snapshots without object evidence.

The real dispatch is `wktbl2.h:318` -> project native-extension scope -> `wow_original_WK32ICallProc32`; host target owns its native state, WOW maps argument addresses and wrapper manages execution ownership/return. Prior guest generic-thunk exception evidence proves a specific route, not all cdecl/stdcall mismatches, pointer masks, counts, callbacks or x87 behavior. A deliberate calling-convention mismatch case must compare stack pointer and continuation for this exact selected object; matching and mismatching conventions, 0/1/32 arguments, conversion mask, faults and nested callbacks should be independently bounded. Do not characterize the mismatch as an already reproduced product defect. Preserve original source and guest; any behavior-changing fix requires audited architecture and policy.

Replay:

```powershell
Get-FileHash build/.ninja/msvc-x86-release/wow32/obj/wkgthunk.obj -Algorithm SHA256
objdump -t build/.ninja/msvc-x86-release/wow32/obj/wkgthunk.obj | Select-String 'ICallProc32|MakeCall'
objdump -dr build/.ninja/msvc-x86-release/wow32/obj/wkgthunk.obj | Select-String 'call\s+\*|MakeCall'
rg -n -i 'wkgthunk|callpr32' build/.ninja/msvc-x86-release/wow32/build.ninja
rg -n 'ICallProc32|MakeCall' build/.ninja/msvc-x86-release/wow32/wow32.dll.map
Get-Content src/mvdm/wow32/i386/callpr32.asm | Select-Object -Skip 60 -First 58
```
