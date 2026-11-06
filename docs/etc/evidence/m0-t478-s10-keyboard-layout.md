# T478 S10 — host keyboard-layout boundary disposition

Baseline: 0cc3139d4. Finding 15 owns the host cmdGetKbdLayout reader, not a guest
keyboard project. The owner requires original semantics without extra tolerance
or new capability. This S makes no production or guest changes.

## Current and original boundary

Both pinned OpenNT families and current cmdkeyb.c have szBuf[28], szPrev[5],
RegQueryValueEx with capacity sizeof(szBuf), and strcpy(szPrev,szBuf) after
forming the command. Current selected map binds cmdGetKbdLayout to
original-mvdm-command:cmdkeyb.obj. The project registry provider is already
connected; it accepts a quoted, terminated REG_SZ that fits 28 bytes, and does
not impose a separate five-byte layout limit. Malformed hex syntax and an
absent mapping take the original no-install path. Neither is proof of a guard
against an overlong but syntactically valid string.

The keyboard language token contract is exactly two characters: original
OpenNT dos/v86/cmd/keyb/parser.asm lines 490–504 rejects both fewer and more
than two characters. This read-only reference is used solely to establish the
host input contract; no guest source/media implementation is admitted. Normal
US and the other two-character codes fit the original previous-layout buffer.
The companion ID query uses eight bytes and its previous-ID buffer also has
eight bytes. No new identifier syntax or unterminated-registry-value promise
is inferred from that observation.

## Current-object witness

[Probe](../../../artifacts/mvdm-audit/shard-3/cmd-keyboard-s10-probe.c),
[runner](../../../artifacts/mvdm-audit/shard-3/cmd-keyboard-s10-probe.cmd) and
[results/hashes](../../../artifacts/mvdm-audit/shard-3/cmd-keyboard-s10-results.json).
The probe derives from the existing S45 original-reader fixture and links the
current cmdkeyb and actual shadow-registry objects. Only Console layout/CP and
system-directory inputs, guest output storage and register plumbing are
controlled. The S9 package's KB16.COM/KEYBOARD.SYS are read-only presence
checks; no guest code is executed or changed.

Configured US reaches DX=1 and the original cache update. Layout unavailable,
malformed hex:zz and absent mapping reach DX=0. Injected ABCDE reaches DX=1,
and all five bytes of szPrev contain ABCDE without a NUL. This demonstrates an
inherited host buffer defect for invalid keyboard configuration, not a missing
original implementation or a valid longer keyboard identifier. The probe's
limitation marker intentionally characterizes that defect; it does not claim
safe handling. The initial ad-hoc placement outside system32 exercised the
host registry fallback instead of the intended sidecar; the final probe runs
under its own build/t478-s10/system32, as the current common root requires.

## Disposition and delivery

No supported host connection is missing and no already-solved reader is
reimplemented. Under the owner capability/tolerance ceiling, retain and register
the original malformed-configuration limit rather than invent truncation,
substitution or longer-code support. Reopen if a valid original keyboard token
exposes the problem, project drift is shown, or the owner explicitly admits
stronger malformed-configuration handling. This is a bounded disposition, not
a declaration that malformed registry data is universally safe.

The unchanged S9 0.0.478/protocol-67 package
694495a1-285e-4e8c-940f-3f2425de4445 remains the local release; all ten asset
hashes were rechecked. S9's 16-gate Full result remains its product acceptance.
No additional product build/publication or new Full claim is needed for this
source-unchanged disposition. No O:/winnt write and no push.
