# DOS4GW / NESticle startup regression

`verify-dos4gw-nesticle.ps1` is mandatory in the Product/Full product gate.
The default external fixture root is `O:\WINNT`; override
`-Dos4gwFixtureRoot` and `-Dos4gwPif` when invoking
`tools/audit/Invoke-ProductVerification.ps1`. Missing or mismatched fixtures
fail the gate. These external executables are not distributed by this test.

| Fixture | SHA-256 |
| --- | --- |
| DOS4GW.EXE | B8265123AC8A189637448618409EF3ECD2E9F3E1A47062C685A02240F688DEC1 |
| NESTICLE.EXE | 224E348E1FD5C11B16EF096FF5607C7C57C49D65FA7E4DBBC9B2A5C9C53E832B |

Supply the existing DOS4GW.PIF and its CONFIG/AUTOEXEC files. The PIF must
select DOS4GW.EXE in the fixture directory without preset arguments. The
runner copies and relocates these configuration files into its owned physical
stage, records their hashes, and verifies the originals remain unchanged.
The PIF's short PATH is required for DOS4GW 1.97's independent PATH limitation.
The observer defaults to a bounded minimal environment because the agent's
large inherited environment can also fail original COMMAND startup before
DOS4GW. `-MinimalEnvironment:$false` is a diagnostic control, not the standard
regression configuration.

Use the freshly built **x64** `wow_message_observer` target for the native
NTCON frontend. The runner owns a private desktop and a 45-second outer
deadline. No Computer Use, user desktop input, ROM, or original guest mutation
is involved. It preserves the package identity and records the actual eleven
image manifest. For publication verification, copy the published images and
manifest, hash-verified, into a build-owned package before invoking the runner.

The observer captures the frontend's stable graphics frame, delivers two
Escape press/release pairs through the existing scoped input facility, and
captures both resulting frames. `check-nesticle-frames.py` requires the exact
CPU/View/Settings/Misc glyph mask before and after, with that mask area cleared
between the key presses. The reference comes from the independently verified
P102/repair-candidate screenshot in
[the regression evidence](../../artifacts/mvdm-audit/dos4gw-pif-sti-20261115/verified-repair.json).
Cursor animation is outside the sampled region. RGB captures, PNGs, input
receipt, logs, fixture identities and package manifest are retained.

An existing window, a living process, a timeout, or a nonblank image alone
cannot pass. DOS4GW errors, visible error dialogs, missing captures or a failed
menu/input witness fail. The persistent menu application is cleaned up by the
test after observation; this is startup/input acceptance, not proof of normal
NESticle exit or ROM gameplay.
