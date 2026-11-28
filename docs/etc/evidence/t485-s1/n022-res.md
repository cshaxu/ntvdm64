# S15 P65 N022 USER resources

## Source and scope

Pinned XP SP1 a2f6d7c93aa4f11efc51147566d5961c59a8ef89,
Source/XPSP1/NT/windows/core/ntuser/client/res.rc,
blob7bb09bbd3738d83a1eaad30f9701adbec57ee574,374 lines.
Prior OpenNT blob00e353f6e817c3230c3ee08f055dd7a6d36e67b0.
P64/runtime73 is the delivery baseline. No guest or winnt.zip changes.

## Whole-file decisions

- Restore XP33-71 EDIT menu,79-139 IME/button/tooltip/shutdown strings and
  212-374 system/help/MDI/scroll menus, MDI dialog and shutdown messages.
  These are static resources: absence of current callers does not justify
  omitting their original contents. Preserve original resource IDs and text.
- Replace C-layout header imports with the existing resource ABI header's
  RC-only declarations. Exact definitions and original path/line provenance
  are retained in [resource identities](n022-res-identities.json). Ordinary
  C consumers keep their previous declaration set; no new service or state.
- Preserve original version constants and _HYDRA_ conditional text, but do
  not emit common.ver: wow32.rc already emits this image's VERSIONINFO.
  The USER32 internal name/description must not replace WOW32's identity.
- Omit the first USER string block. wow32.rc owns
  strings0-13 in RT_STRING block1 and256-269 in its error namespace.
  In particular USER STR_ERROR=2 and STR_MOREWINDOWS=13 collide with
  iszChooseClose and iszChooseIgnoreAlignment. Even STR_NOMEMBITMAP=14
  shares block1. Do not renumber either to conceal the collision.
- Restore original strid.mc (blob1f9fa14d912970060e0373b510077e8f57bdc6a8,
  66 lines) as a direct res.rc dependency. Native mc.exe generates strid.rc,
  strid.h and the message-table bytes. Its five messages are RT_MESSAGETABLE,
  not the colliding RT_STRING block; initial omission was rejected on review.
- USER32 system bitmap/icon/cursor resources at XP140-211 remain outside
  the admitted WOW32 resource carrier. Their rcscale/rc96dpi/rc120dpi and
  cursicon binary inputs are not current mirror inputs; current native
  USER/GDI resource providers are unchanged. This batch does not import a
  second system-image package or claim those resources are migrated.

All omissions remain visible nonerror differences; no artificial exclusion
condition was added. Original resource bodies retain diagnostic #line mapping.

## Metrics and delivery

Delivered result:10points/204lines against OpenNT becomes4points/89lines
against XP, delta-6points/-115lines. Global1330files,789byte-identical,
800nonerror-identical,530different,8102points/66273lines.1304XP/26OpenNT.
N02236/39 plus one exact dependency delivered. S15 9/14, overall57/159.
P61 runtime70 Full24/24 remains the checkpoint; five files including the direct dependency since it.
Cached full build and runtime75 dual13-image26hash publication pass.
Published EDIT menu and WOW critical string blocks are byte-identical to
runtime73; restored menu/dialog/string resources are present and version
identity remains WOW32. Real SOL Options and original Win16 control gates pass.
Initial build failures and their corrections remain in the receipt.
[Receipt](n022-res-result.json), [census](n022-res-census.json).
