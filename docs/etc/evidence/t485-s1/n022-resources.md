# S15 P35 N022 resources review

Base commit:98a23f1c6. Fixed XP clres blob8085809158b0e31f60058823bb36bc71f524c85c;
OpenNT blobe6fd8db7f4ef05d5377143874bcb5a2a4ebd69ef. No guest or winnt.zip change.

## Whole-file disposition

Retain the original types/constants and29 complete resource/template function
units in original order, including GetResourceBpp required by XP image scoring.
Retain original four disabled/DBG arms as#error with original guards and#line.
Keep the existing dialog/bitmap translation-unit split; adjacent selections
are merged rather than adding a conditional around each function.

Native USER resource frontiers remain explicit whole-function omissions:
- SplFindResource/SplFreeResource and LoadBmp/LoadDIB/LoadIcoCur participate in
  original shared USER resource/module ownership. The original globals and
  resource-call table are not the native USER cache. Supplied bitmap bytes
  enter WOWLoadBitmapA; native named bitmaps use existing LoadBitmapW binding.
- Accelerator load/cache/public entrypoints use private ACCELCACHE validation
  and native accelerator ownership. Actual WOW waccel.c::CreateAccel32 calls
  native CreateAcceleratorTable; it already owns guest/native aliases.
- Public string wrappers use native resource lookup; actual walias.c/wkman.c/
  wow32.c call native LoadString. Do not replace the host resource module owner.
- Public dialog A/W/modeless wrappers are supplied by existing scoped creation
  entrypoints. Retained GetSizeDialogTemplate and modal body still execute.
  Preserve native public symbols rather than defining competing USER exports.
- Cursor/icon construction, shared cache, public load/copy/info, and server
  create wrappers require private CURSOR data and NtUser cursor allocation.
  Actual compat/wow_private_user_compat.c uses CreateIconFromResourceEx and
  LoadImageW; WOW win95.c calls native CopyImage. Original private USER cache
  and kernel object layouts are outside the admitted finite client boundary.

These are ownership boundaries, not permission to delete merely-unused
original code. All selected algorithms remain XP source; none is reimplemented
in B. Original constants and types remain even when not presently expanded.

## Necessary adaptations and paired dependency

Header binding and diagnostic lines are host composition. GETINITDC and
RELEASEINITDC use existing paired GetDC/ReleaseDC: XP system-startup branch
requires gfSystemInitialized, which this host never transitions; native USER
has already initialized. First build failed on that unbound variable and
NtUserGetDC; no invented initialization flag or private-API replacement added.

CopyDibHdr retains the existing OLDICON pointer-order repair: capture old
abBitmap before replacing upbih with the stack Fake header. The original
macro otherwise dereferences the replacement header as OLDICON. Existing
fixture records cover this input; XP does not fix that ordering.

XP InternalDialogBox drops cb; source audit proves the old provider reads it
zero times. Restore six-argument signature, INT_PTR result and LPARAM together
with usercli and scoped declarations/fixture calls. No compatibility thunk.
Whole dlgmgr/usercli files remain OpenNT-baselined until their DIR turn.

Bitmap display data moves within the same call-owned context from screen to
XP server shape. The existing lock/DC lifetime remains unchanged. TAGMSG retail
macros are no-ops; TEST_PUSIF reads the existing palette capability value.

## Verification state

First cached build failed on original USER startup macros. Second cached
full build passed. Runtime38 dual publication passed26 image hash checks.
First bitmap test build failed because its isolated compile context omitted
resource-cover lookup and text-conversion bindings. A /Gy experiment did not
resolve it and was withdrawn. The small native fixture header now binds the
cover to actual FindResourceExA and existing WCSToMBEx, with no fake success.
This does not claim guest-resource-cover coverage; real SOL uses the actual
production registration. Final bitmap normal/failure suites, actual modal lifecycle, and runtime38 SOL
Options passed. Added negative DIB test first used NULL and correctly hit the
original precondition assertion; corrected to a nonzero invalid bitmap handle
to exercise GetObject failure. Production was unchanged during test retries.
No Full repetition is required this P.

Current nonerror census:1329files,781exact,792same/537different,
7848points/123546lines; delta+27points/-3355lines.
clres OpenNT1/6424 -> XP25/3061; dlgmgr OpenNT37/420 ->40/428;
usercli remains1/4693. Baselines1272XP/57OpenNT. Original LF restoration
unmerges old whole-file byte differences; no source-line totals substituted.

Final publication matches all26 hashes; current WOW32 build input matches
manifest InputSha256 after fixture builds. Supplemental legacy standalone
template/header fixtures were updated for source coherence but were not run;
no passing result is claimed for those standalone files.

[Receipt](n022-resources-result.json), [census](n022-resources-census.json).
