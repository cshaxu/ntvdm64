# S15 P32 - N022 USER globals and message metadata

The ordered file is `windows/core/ntuser/client/clglobal.c`: XP blob
`6cbb445357c5944f6112900df64c8bb5a9a8c389`, replacing registered OpenNT blob
`61253b83407c25e33f3b29341efde2d4b1a948c4`. Read both as raw Git bytes.
The original XP global declarations, message-box button table, zero object,
resource callbacks and scalar state are restored. The old OpenNT message
classification array moved upstream to `messages.h`; its actual EDIT consumer
now reads the XP `MessageTable[].bThunkMessage` field. Original listmung,
msgdef.lst and msgdef.tpl generate msgdef.h through shared CMD/CMake rules.
No guest bytes, runtime frontend behavior or callback lifecycle are changed.

## Residual scope and ownership

- Includes bind modern public host declarations and narrowly transcribed XP
  zero-object/cache declarations. Source paths and original lines are in
  `wow_user_globals_binding.h`; no algorithm is implemented in that header.
- DBG, _JANUS_ and MESSAGE_PUMP_HOOK retain their original condition with
  #error bodies. Selected logical-line audit is clean; lines100 and220 follow
  the closing conditional so their correction applies to the selected path.
- USER `gpsi` remains omitted. WOW32 already defines the same symbol as a
  DWORD guest shared-information identity in wow32.c:142 and assigns it during
  registration. Original USER and WOW were separate DLLs. A successful common
  link is not evidence that these two storage objects may be coalesced.
- Six Local* callback globals remain represented by the existing registered
  table used by recovered EDIT/dialog sources through wow_edit_memory_bindings.h.
  Their original DispatchLocal* defaults in XP clinit.c:1166-1232 are simple
  public Local* wrappers, not missing complicated algorithms. Reintroducing
  unused standalone globals does not replace that registered table's guest
  allocation/handle ownership; changing this boundary is not part of this file.
- Four LPK pointers remain omitted at this migration frontier: the two default
  functions are XP drawtext.c providers not yet present in the retained older
  drawtext.c; the other two belong to the same LPK callback ABI. This is a
  specific missing-provider dependency, not proof that all LPK declarations
  must remain omitted permanently. Revisit with the ordered drawtext migration.
- The initialized USERAPIHOOK object depends on private Real*/DefaultOWP/
  ForceResetUserApiHook providers of the original full USER32 runtime. The
  selected standalone host uses modern USER32, not a second full USER32.
  Hook scalar declarations remain; no empty providers or second state machine.
- Existing registration storage, validation, exchange and lifecycle functions
  remain intact after the original file. Their input providers and immutable
  guest callback ABI remain those of P19 and the current registration bridge.

Paired clmsg.c, editec.c and user.h retain their OpenNT baseline until their
whole-file migrations. The four new required generator/metadata files are
byte-identical XP prerequisites, individually registered in the census.

## Verification and initial failures

Cached full build and dual publication completed for runtime33. Registration
lifecycle, the38-case control cohort and real SOL Options passed. The controls
initially failed before execution: their independent build lacked generated
msgdef.h, then its historical whole-globals projection linked unused resource
providers. The final fixtures share the production generator; the existing
projection extracts actual original lock declarations, and conversion checks
include the actual original XP message table. No fake provider was introduced.
The attempted /Gw-only fix was insufficient and was removed. Failed logs remain
under build/t485-s15/n022-globals/control-tests through control-tests-r4;
control-tests-r5 contains the passing38 cases. SOL verifies modal controls,
owner relationship, close and parent recovery on the published runtime33.
Final shared-rule cached build and package identity are recorded in the receipt.
Full was24/24 at preceding P31; no new Full is claimed for this targeted P.

## Census

| Metric | Before | After | Delta |
| --- | ---: | ---: | ---: |
| Non-guest files |1325|1329|+4|
| Byte-identical |777|781|+4|
| Nonerror identical |788|792|+4|
| Nonerror different |537|537|0|
| Nonerror points |7798|7805|+7|
| Nonerror changed lines |130239|127713|-2526|

clglobal.c changes from1point/2702lines against OpenNT to8points/161lines
against XP. LF restoration unfolds the old whole-file block; the+7points
are reported rather than described as a point reduction. Paired clmsg +3lines,
user.h +12lines, editec unchanged line count; new prerequisites0diff.
Current baselines1269XP/60OpenNT. N0223/39; overall57/159; S15 9/14.
Eight changed mirror files since P31 Full; next ordered file clmenu.c.

[Receipt](n022-globals-result.json) and [census](n022-globals-census.json)
record per-file identities, actual validation and publication hashes.
