# WOW button message dispatch

Frozen source universe SHA256 F73FFD45DFF44FCA4ECEFC49EF769BEAB5C085542CD04FFE4D6A7B5F942E3196. `wow32/wmsgbm.c` SHA256 D7EF837D592221481C988CCB2E164FAFF9D7A0B6349B59E1BBC3841323A98692 matches OpenNT.

Original `wmsg16.c` initializes class-indexed thunk/unthunk arrays; `ThunkMsg16` assigns original uMsg, parameters and target, selects a button thunk for a matched WOWCLASS_BUTTON then calls `ThunkBMMsg16`. This maps old WM_USER+0..4 to BM_GETCHECK..BM_SETSTYLE and special Win3.1 WM_USER+99 to BM_CLICK; unrelated messages remain unchanged, and `UnThunkBMMsg16` is empty. Reverse direction is separately owned by `wmtbl32.c` and `wcntl32.c::WM32BMControl/WM32BMClick`, only when fThunk is set. The original caller's SendMessage, HWND classification and lifetime are separate constraints.

`build/.ninja/msvc-x86-release/wow32/build.ninja` selects wmsgbm and wcntl32; historical `build/t448-s12-mine-r04/WOW32.DLL.map` maps both thunks and both reverse owners to these objects. It does not prove installed DLL identity or dispatcher execution. `tests/adapter-mvdm-host-out/wow/wow_standard_class_fixture.c` exercises native BM_GETCHECK/SETCHECK control handling, not the Win16 class dispatch.

Run `python artifacts/mvdm-audit/shard-1/button-thunk-source-fixture.py` with GCC and the frozen source unchanged. The fixture extracts the actual current two forward bodies, compiles against a minimal counted-type harness and confirms 0400-0404 => 00f0-00f4, 0463 => 00f5, 0405 and 04c8 unchanged; unthunk returns. This proves arithmetic in the extracted body, not selected class detection, reverse direction, parameters, native button behavior, callback reentrancy, destruction or task semantics.

Retain pending: current actual DLL identity and real Win16 button message round trip across both directions; verify fThunk false, unknown/app WM_USER messages, parameters, nested callback and destroyed HWND. No proven behavioral divergence or product/guest modification.
