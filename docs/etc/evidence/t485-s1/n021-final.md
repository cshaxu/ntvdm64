# S15 P29 N021 final SDK declarations

Status: final cached build, dual publication and targeted verification passed.
Pinned XP SP1 a2f6d7c93aa4f11efc51147566d5961c59a8ef89; raw original and prior
product bytes are retained in build/t485-s15/n021-final. No guest change.

## Ordered file review

1. ptypes32.h (blob04d14e3057f0207ef1088c924d4fe4b9c1dd9a19): restore exact
   XP bytes, copyright, pragma once, LF/EOF. Existing portability types/macros
   are unchanged. Real serial consumers are nt_com.c and nt_wcom.c.
2. vddsvc.h (blob20ca93a34504dda189533846770592f583e626d2): restore all XP
   declarations and original order. Preserve three existing non-i386 rejected
   arms under their original condition framework and correct #line anchors.
   Eleven setters retain WINAPI: setEAX/AX/EBX/ECX/EDX/ESP/EBP/ESI/EDI/EIP/ES.
   Their actual exports in generated/ntvdm-wow32-provider.def bind to the
   ntvdm_debug_*@4 wrappers in src/ntvdm-exe/debugger/register_exports.c.
   Restoring cdecl in this public header would mismatch the existing external
   callee-pop contract. Internal CCPU setters remain cdecl; no provider or
   algorithm is changed. VJOY/VCDEX consume this header and are rebuilt.
3. vdmdbg.h: restore complete XP constants, original ordering, TEMP_BP_NOTE,
   VDM_SEGINFO, API declarations, typedefs, WINAPI and C++ linkage wrappers.
   No current product caller requires the obsolete VDMGet/SetThreadContext
   declarations in place of XP VDMGet/SetContext. Preserve only the existing
   non-x86 context arm exclusion. Its only non-exclusion difference is the
   #line after that arm. Actual consumers are dbg.c, dbgdll and WOW wkman.c.

All matched nonempty original diagnostic lines are verified. Retain only
source/ABI-backed adaptations, not old bulk subset omissions. No state,
queue, provider table, fallback or new autonomous implementation is introduced.

## Mirror changes

| File | Previous OpenNT nonerror | XP nonerror | Delta |
| --- | --- | --- | --- |
| ptypes32.h | 1point/124lines | 0point/0lines | -1point/-124lines |
| vddsvc.h | 1point/367lines | 13points/25lines | +12points/-342lines |
| vdmdbg.h | 1point/801lines | 1point/1line | 0points/-800lines |

Raw-byte LF restoration expands the old whole-file blocks; #line remains
counted. Four excluded branch bodies are excluded, their frameworks remain
counted. Final global1325files,777byte-exact,788nonerror same/537different,
7784points/135222lines; delta0files/+1exact/+1same/-1different/+11points/
-1266lines.1262XP/63OpenNT; three baseline transitions, no added files.

## Directory reconciliation and delivery

All17 N021 inventory paths resolve to their pinned XP object and current hash
in the candidate global census. The paired original string/buffer prerequisite
was already delivered in P27. N021 is complete17/17:
final cached full build and runtime30 dual13-image26hash publication passed.
Both capture fixtures pass3475 assertions, both layouts pass, service42 passes,
actual guest COMMAND/NetAPI returns NETAPI-OK, native VDD object contract
passes and real VDD/MEM verifies exports, register round trip, debug event
writeback and matching task termination. [Receipt](n021-final-result.json);
[census](n021-final-census.json). No physical device coverage is claimed.
Overall57/159 directories; S15 9/14. Six mirror files completed since P26
Full24/24; Full is not yet due. Next N022 USER client, in DIR order.
