# T478 S40 multimedia/fax investigation

Status: bounded finding54 investigation closed; no product source change.

## Existing work retained

S25 already repaired the original fax initialization and installation failure
paths, with135 focused checks and coherent Full/release. Current wow32fax.c
hash matches the S25 recorded input. S40 binds all12 functions in its current
object to the exact S38 released image. This is not a new fax runtime test.
Four current wmmedia object functions also bind to the release. The receipt is
`build/t478-s40/current-host-bindings.json`.

T466 established original MMSYSTEM to WINMM private-protocol ownership.
S37 subsequently repaired the public interrupt calling convention and verified
38 timer notifications across the original16-slot ring, explicit cancellation
and normal exit. The original disabled structured converters are not missing
active dispatch bindings and are not to be re-enabled by assumption.

## Actual synchronous guest/native comparison

Independent `win16_media_probe.asm` calls original MMSYSTEM ordinals through
an unchanged app-local copy of the packaged guest DLL. The private desktop
runner hashes that copy and all runtime inputs. No guest media is modified.

`build/t478-s40/media-r1` passes enumeration, format query, invalid wave/MIDI
ids and invalid MCI id/error text. Native x86 WINMM at `native-r1` reports the
same two wave outputs, two wave inputs, one MIDI output and zero MIDI inputs.
Both report query0, invalid-wave2, invalid-MIDI2 and invalid-MCI257. Guest
query/rejection handles remain NULL; the error text buffer guards survive.

## Actual asynchronous silent wave

`media-r2` adds a100ms mono8kHz8-bit buffer filled with unsigned silence128.
Original waveOutOpen, PrepareHeader, Write, Reset, UnprepareHeader and Close
all return0. The guest yields and observes flags3: DONE|PREPARED, no INQUEUE.
The original guest header completion is therefore observed through the real
host backend, not a fabricated completion. Normal guest/launcher exit follows.
The probe has a2-second completion deadline and the existing45/55-second outer
observer limits. It does not capture input or change device settings.

The app uses CALLBACK_NULL; header completion does not prove application
wave callback semantics. Shared application-callback evidence is the separate
S37 timer test. No claim of abnormal task-loss cleanup or every wave device.

## Follow-up plan after first wave probe (resolved below)

Reconcile successful MIDI open/close and MCI device prerequisites where an
existing native provider permits them. Actual fax driver deployment and guest
fax operations must remain distinguished from S25 failure/ownership checks.
Do not install drivers, fabricate hardware, activate original disabled bodies,
or add a second callback queue to obtain a passing test. Final disposition
must identify any external provider limits rather than silently equating the
current no-defect evidence with comprehensive multimedia acceptance.

## MIDI/MCI and final verification

`media-r3` adds actual MIDI device0 open/close and MCI waveaudio open/close
of an independently generated800-byte silence WAV in the disposable stage.
All four operations return0; MCI returns device1. No MIDI notes/messages or
MCI playback/capture are requested. The current original MCI parameter block
includes its original reserved word; guest layout is not guessed from Win32.
Final `media-r4` strengthens assertions to require successful MCI opening and
closing and a nonzero id, and passes all earlier query/wave/MIDI checks.
The guest/launcher exits0. Native enumeration/error/query results are identical
where compared. Actual successful MIDI/MCI lifecycle is guest evidence, not
inferred from that native-only comparison.

This supersedes the old T466 statement that MCI was unavailable in the selected
runtime: the present immutable MMSYSTEM and native driver route demonstrably
open and close waveaudio. It does not justify enabling the unrelated original
#if0 MCI conversion bodies or adding an alternative protocol owner.

## Fax and original lifetime limits

S25's current source hash and12 released fax bodies remain verified, including
its registry/provider boundary. The135-check controlled fixture is retained
rather than repeated under a misleading new result label. The selected
WOW32FaxHandler/wkman host client exists; the separate WOWFAX driver/UI and
actual external fax-driver deployment are not supplied as an operational fax
acceptance environment here. No driver installation or physical fax transaction
is performed, and ordinary media/printing tests cannot certify that path.
Existing malformed registry/path/driver-failure limits in S25 remain explicit.
No new missing host client connection is established by lack of that environment.

Original guest drvproc.c AppExit owns device rundown. These new tests verify
explicit completion/reset/unprepare/close and normal exit; they do not establish
safe forced task loss with queued buffers. The actual S37 timer callback-ring
wrap/cancel evidence is retained separately, not relabelled as wave/MIDI
application callback coverage. No new scheduler, retained buffer registry,
forced-drain promise or out-of-original recovery is introduced.

## Bounded disposition

Finding54 has no demonstrated missing project connection to repair. Active
original wave, MIDI and MCI routes now have actual guest witnesses beyond the
previous timer-only evidence. The historical excluded converter bodies remain
excluded; external fax acceptance and device-specific callback/task-loss
certification are explicitly not claimed. [Frozen results](../../../artifacts/mvdm-audit/shard-3/multimedia-s40-results.json)
record current image bindings, source/provider identities, real guest and native
results and ten verified release hashes.

This local P changes tests/evidence only. Exact S38 package
818fff8c-efa7-4570-93d6-1f9c820f40ed and its Full16/16 remain the product delivery;
no redundant product build, Full or republication is claimed. No O:/winnt write
or push. S41/finding61 follows for the original VDD/optical host chain.
