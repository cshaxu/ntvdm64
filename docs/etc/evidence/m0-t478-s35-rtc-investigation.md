# T478 S35 RTC investigation

Parent 00449fbcf; finding27. Final disposition below closes this bounded S; earlier checkpoints retain their then-current interpretation.
No product source, package or original guest has changed. Admission governance
passed (background PID40848).

## Separate the available implementations

The earlier audit identifies the linked host `rtc_bios.c::rtc_int`, selected
at BIOS BOP70, and asks for countdown/alarm/EOI evidence. The actual execution
owner must be established before treating that linked body as the current
guest's normal RTC path. Original ROM bios1.asm also contains a pure Intel RTC
handler with a 976-microsecond decrement, alarm INT4A and both PIC EOIs;
bios4.asm implements INT15/83 and /86. The original host tape_io.c explicitly
says those calls no longer pass through from ROM and rejects them in its
default case. The retained NT spckbd source has a separate BOP15/BOP70 route.
None of these source files alone identifies the current loaded guest owner.

## Controlled host-body finding

`test/observation/verify-rtc-bios-contract.py --build-root
build/t478-s35/rtc-before` ran in owned background PID18308 with a 90-second
compile and 15-second execution limit. It compiles the exact selected rtc_int
body with its actual rtc_bios.h, using controlled CMOS/SAS/register/callback
boundaries. It does not execute a real PIC, guest IRQ or INT4A handler.

The header is byte-identical to both pinned original editions, SHA256
EF200BC2174E6BDA4010095C1E7615E55A57EEAAA6A7F37F2484AFA435414F13.
Its DOUBLE_TIME places high before low, so the original half-word load/store
algorithm does not represent the BDA little-endian count on the selected x86
host. For example, input 100000 (000186A0h) produces 3CB1869Fh instead of
50000 (0000C350h) after its original TIME_DEC=50000 decrement. The fixture
has 67 checks and 13 failures, including completion consequences; CMOS
enable/cause masking, controlled alarm CS/IP restoration and EOI port order
controls pass. This is a source-body arithmetic defect under these inputs,
not yet a demonstrated product regression or authorization to replace the
active original ROM implementation. No repair has been made.

## Actual guest entry evidence changes the next action

The independent `rtc_entry_probe.asm` first checks INT12 against the current
BDA memory word, temporarily changes that documented mutable word under CLI,
checks a second INT12 and restores it before continuing. All three runs pass
that part on unchanged S34 package 03d9792b-753e-4407-8a52-883ad9bc83e3.

The first test premise was that INT15/83 would reach host tape_io's rejection.
It did not. Retained attempts:

| Fresh evidence root | Runner PID | Observed result |
| --- | --- | --- |
| build/t478-s35/entry-live | 8308 | Mutable INT12 passes; expected wait rejection fails. |
| build/t478-s35/entry-live2 | 30356 | Added actual return evidence: function83 returns AX0042h, flags low byte46h, CF clear. Thus the request is accepted, not rejected86h. |
| build/t478-s35/entry-live3 | 18592 | Read-only vector/entry bytes: INT15=02F0:020D, bytes80FC88740A80FC87; INT70=0369:0035, bytesE8AF013100EB1000. Neither first entry is the raw F000 ROM address; wrappers/installed handlers must be followed. |

The test cancels an unexpectedly accepted wait through INT15/8301 before
exiting. These runs remain failed assertions of an incorrect test premise,
not product failures and not wait-completion proof. Guest acceptance of /83
supports the need to trace its real implementation; it does not by itself
prove the ROM path or its completion behavior. The observed vectors prevent
prematurely identifying the normal IRQ owner solely from ROM source.

Each run uses the existing medium-token private-desktop runner and a physical
isolated package. No original executable, interrupt vector or firmware was
replaced. The BDA experiment is a bounded caller test of memory_size's explicit
mutable-word contract, not a workaround for an original guest defect.

## Next work

Follow the loaded original INT15/INT70 wrappers and their ultimate provider;
test actual asynchronous completion/cancellation, alarm return and IRQ/EOI
retirement. Determine whether the host arithmetic candidate is actually used,
a retained alternate original owner, or missing wiring before selecting any
repair. Do not revive host wait services merely because the ROM already
supplies them. S35 remains active; no product build/publication/commit claim.

## Loaded owner and normal-wait checkpoint

The installed INT15 entry bytes match HIMEM.SYS at file offset020Dh. The
original HIMEM Int15Handler checks88h/87h and passes other functions through
PrevInt15. The INT70 entry bytes match NTIO.SYS at file offset0A05h; its original
sysinit1.asm int70 calls do_int_stacks with the inline old70 pointer.
Read-only traversal of that loaded pointer finds F000:4B1B and bytes
501EB840008ED8B0, matching original bios1.rom offset4B1Bh. Thus the tested
normal IRQ8 route is the original pure Intel RTC handler, not host rtc_int.
This conclusion is based on the live pointer and original binary bytes,
not merely on the existence of two similar source implementations.

`wait-live` (PID29936) passed mutable INT12, actual 100000-us asynchronous
completion, preservation of lower user-flag bits (35h -> B5h), and a final
negative count within the original 976-us decrement. Its synchronous test
AFTER cancellation was rejected. `wait-live2` (PID33668) moved synchronous
waiting before cancellation and it passed, isolating cancellation's retained
busy state. That attempt still incorrectly required AH86h on busy rejection;
original int15_83_inuse sets CF but leaves AH unspecified. `wait-live3`
(PID29376) retains strict CF rejection and all state checks without that
unsupported AH expectation; it exits zero with all expected markers.
These are explained test-premise corrections, not unexplained retries.

The successful independent probe proves normal asynchronous completion,
synchronous completion, actual unchanged IRQ8 wrapper/ROM path, and cancellation
preventing the caller's completion flag after three BIOS ticks. It also
reproduces an original guest limitation: INT15/8301 disables PIE but writes
rtc_wait_flag using caller DS without selecting BIOS_VAR_SEGMENT. With ordinary
caller DS, BDA0040:00A0 remains busy; a subsequent /8300 request returns CF.
Original bios4.rom has the exact cancellation tail at offset18EEh:
C606A00000 FB33C0 CA0200 (write DS:A0, STI, clear AX, RETF2), following the
PIE-clear port writes. The source int15_83_stop has no DS load/save around it.
This is an immutable original-ROM defect; no host correction or guest patch is
introduced. The owner's standing original-guest limitation disposition applies.

ROM identities: bios1.rom SHA256
1CE33544249FEF59865B91308FAF60F354563CF854F80EB196D47C968899A574;
bios4.rom SHA256
B09FDFF1A500971D54233378CE9C36F1B6A58BF577E4A544DA03A0D78D7E0D6F.
`artifacts/mvdm-audit/shard-3/rtc-s35-wait-progress.json` freezes the
read-only binary observations, successful/unsuccessful guest transcripts,
probe/run identities and remaining scope. Alarm INT4A and final selected-image
reconciliation remain open. No product code or publication changed.

## Final disposition and reproducible verification

Final background supervisor PID19860 used the recorded modern pwsh host,
medium token and isolated physical packages. Both final guest probes returned
zero; their runner context, complete console output, input hashes and selected
image evidence are frozen in
[RTC results](../../../artifacts/mvdm-audit/shard-3/rtc-s35-results.json).
The independent COM programs were assembled, not original guest programs.

Reproduce with `test/integration/verify-ccpu-sign-flags-guest.ps1` using
`-Workload RtcEntry` and then `-Workload RtcAlarm`, each with a fresh
`-BuildRoot build/t478-s35/<unique-name>`,
`-PackageRoot build/t478-s34/package-final`,
`-Observer build/t478-s34/observer.exe` and
`-LogonHelper build/t478-s34/logon-env.dll`. Launch as an owned background
modern pwsh process. These are serial real-package tests, not controlled-body
substitutes. Each uses the runner's existing bounded observer and normal-exit
assertion. Final roots are `wait-final` and `alarm-final`.

| Contract | Evidence and result |
| --- | --- |
| INT12 reads mutable BIOS memory-size word | RtcEntry temporarily changes and restores the word; normal launcher exit and `RTC AUDIT MUTABLE INT12 BDA PASS`. |
| Original asynchronous and synchronous waiting | RtcEntry verifies 100000-us async count at original 976-us steps, flag35h becoming B5h, and 10000-us synchronous completion before cancellation. |
| Cancellation boundary | PIE clears and no completion follows; subsequent request is rejected with CF because original ROM leaves BDA busy. This is reproduced original-guest debt, not successful cancellation-and-reuse. |
| IRQ8 owner | Loaded NTIO inline old70 points to F000:4B1B, matching immutable bios1.rom. INT15 entry matches HIMEM's forwarding wrapper; the entire INT15 chain was not separately byte-walked. |
| Alarm callback, return, retirement | RtcAlarm installs its own INT4A client, leaves IRQ8 intact, observes exactly one callback after disabling AIE, checks both relevant PIC ISR bits clear, restores and reads back vector, alarm registers, B and PIC masks. Marker `RTC ALARM INT4A ONCE RETURN EOI RESTORE PASS`. |
| Selected image | `rtc-s35-image-observer.py` matches eight whole relocation-masked memory/RTC/CMOS bodies and both entire original ROM blobs uniquely inside published NTVDM. Static matches do not imply execution of the alternate host rtc_int body. |

Finding27 does not require a new host RTC implementation or revival of the
alternate BOP70 wait path: normal original ROM functionality is already wired
and now tested. The controlled host-body fixture remains **67 checks / 13
failures**, not a passing gate. Its original x86 word-layout defect is retained
as an explicitly unverified alternate/private route, not advertised as safe or
fixed. A concrete supported caller reaching that body is the reopening trigger.
Original ROM cancellation debt remains in TODO under immutable-guest policy.
No generic fallback, tolerance or guest patch is introduced.

All ten release images match the exact S34 tested package and manifest,
03d9792b-753e-4407-8a52-883ad9bc83e3. S35 changes only tests and evidence;
production sources, build inputs and guest media are unchanged. The S34 coherent
build and Full16/16 remain the exact-package regression evidence; no redundant
product rebuild, Full rerun or republication was performed. No O:/winnt write
or push. S36 remains the next planned finding.

Closure governance passed in background PID36304; git diff --check passed. The earlier PID26036 check rejected the no-active heading format, which was corrected to the mandated explicit marker. This was a documentation-format failure, not a runtime result.
