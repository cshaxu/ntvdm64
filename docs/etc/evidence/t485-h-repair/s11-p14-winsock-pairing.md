# S11 P14 Winsock guest pairing

P14's host code changes do not introduce this failure. The same independent
Win16 probe stops on runtime13, runtime16 and runtime17 after CreateWindow
succeeds, before WSAStartup returns. Ordinary logon environment gives the same
result. The production dispatch trace has no WWS32WSAStartup entry; instead
the guest calls WU32SysErrorBox and retires its task. Run16 exit zero is not
successful execution of the probe. The debugger records one worker thread
exit 1067, without a captured host access violation; this is not the cause.

The packaged WINSOCK.DLL is the earlier file with SHA256
629e86c59c83f66d3623388b9ed6ae9d5a664979e588a9eb45b8a54441126fd9.
H033 paired twelve XP guest files but did not replace WINSOCK.DLL.
Replacing only this DLL in a fresh isolated runtime with the unmodified XP
Professional SP3 file fixes all three actual Win16 witnesses:

| Case | Phase | Startup | Completion | Freed | Cancel | Errors |
| --- | --- | --- | --- | --- | --- | --- |
| Completion | 2 | 0 | 1 | 1 | 0 | 0 |
| Cancel | 2 | 0 | 0 | 1 | 1 | 0 |
| TaskLoss | 3 | 0 | 0 | 0 | 0 | 0 |

SP3 input: build/t485-sp3-pro-media/winsock.dll, SHA256
2b22a58572bb65f3fa3be137337613531d192759c48bdc4a8e37ba1911962212,
matching the existing Professional SP3 media inventory. No original guest
was modified or rebuilt. This isolates the package compatibility defect;
it does not yet identify the precise differing instruction or relocation
within the two immutable DLL versions.

Evidence lives under build/t485-s11/p14/final: async-diagnostic,
async-baseline, async-prior-p10, async-logon, async-debug, async-trace,
async-sp3-pair, async-sp3-Cancel and async-sp3-TaskLoss. The last three have
result.exit=0; they are isolated pairing evidence, not formal publication
acceptance. The independent probe now records three initialization phases.

CMD/native tar produced build/t485-s11/p14/winnt-paired.zip.
The read-only archive audit verifies the same 67 file paths and exactly one
content change: WINNT/SYSTEM32/WINSOCK.DLL. Other 66 contents are identical.
See build/t485-s11/p14/guest-archive-delta.json. No new archive member is added.

Automatic review initially rejected formal publication. The owner then
permanently approved original XP file replacements on 2026-11-26; see the
[source policy](../../operations/policy/source-policy.md). The original SP3
WINSOCK.DLL is now published in the existing archive member, runtime17 and
O:/WINNT, with matching hashes. Formal package Completion, Cancel and TaskLoss
all pass under p14/final/async-final-*. No candidate substitution is used by
those tests. Commit and push remain pending. Non-error mirror metrics are
unchanged by the guest pairing repair.
