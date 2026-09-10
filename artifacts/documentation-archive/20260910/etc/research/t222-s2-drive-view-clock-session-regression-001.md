# T222 S2 Drive-View Clock Session Regression

The grouped clock owner now has Direct/Readonly normal-session evidence for
`50:14`, `50:15`, `50:19`, and `50:1C` through the common drive-view route.
Date and time queries resume with their OpenNT-shaped register layouts and no
flags update; the assertions intentionally validate ranges rather than a
fragile host-clock instant.

No explicit Direct clock-mutation capability is admitted in T222.  Therefore
both setters retain the source-derived non-mutating result: preserve `AH`, set
`AL=FF`, and do not write flags or call the ambient host clock.  Readonly does
not grow a selector-local alternative.

The focused MSVC x64 `/MT` fixture compiled and linked from
`build/M0-T222-S2/029-drive-view-clock`; its executable exited zero.  This
closes only the normal-session clock matrix entry.  DPB/full-DPB, computer
identity, and the remaining family assertions are still open.
