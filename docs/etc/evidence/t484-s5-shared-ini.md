# T484 S5 P5 shared INI editor for Windows launch preparation

Moved the existing guest-ini implementation into the INI.EXE addon and retired
the parallel fixture target. Test preparation and both patch packages use
this owner. Win31 and Win95 launchers choose file states and call the same
editor. Their separate SYSTEM.INI writer loops were removed. INI.EXE validates
a unique section and exact installed filename matches before writing a fresh
candidate. Installation inserts in caller order, selection updates existing
entries in place and removal preserves unrelated entries. The tests also
compare exact unrelated bytes for LF, CRLF, non-ASCII bytes and exclamation
marks. The native network and isolated driver operations remain supported.

The original Win31 and Win95 configuration baselines and the existing INI
suite passed before the refactor. The updated INI suite passed. Isolated
addon publication verified 10 utilities and 5 drivers, rejected invalid
receipts, missing destination and locked output, and preserved driver
qualification metadata. The cached full host build matched all eleven
images in both release locations. INI.EXE was published to assets/release
and all fourteen existing addon files retained their hashes.

Using the published editor, the Win31 six-profile and Win95 four-profile
configuration tests passed. Win31 original-media apply, repeat, rejection
and unapply passed on a build-owned copy. Win95 applied all four profiles
and restored SYSTEM.INI exactly on unapply. These are launcher/package
tests. Guest desktop runtime acceptance follows in S6. The original
installations were read only. Receipts are under build/t484-s5-launch-ini.
