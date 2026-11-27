# N021 VDD declarations - S15 P22

Starting HEADce651710d. Fixed XP SP1 a2f6d7c93aa4f11efc51147566d5961c59a8ef89.
Original public/sdk/inc/nt_vdd.h blob00821b561abf0edfba44701440eef70848ed8a89;
paired base/mvdm/dpmi32/precomp.h blobeb50c9f2e37aba6e61b37457d1eb94d3a520f307.
Raw originals, before bytes and delivery logs: build/t485-s15/n021-vdd.
[Census](n021-vdd-census.json) records current source hashes and all non-guest
mirror totals against individually selected XP/OpenNT originals.

## Complete file review

Restore XP declarations, copyright, LF, pragma once, extern-C scope, trace
prototype, default getMODE and original declaration order. Preserve unused
original declarations. The existing DBG rejection now occupies the matching
XP DBG arm's single line; no invented exclusion and no #line required.

Two residual nt_vdd differences:

1. VdmMapFlat retains WINAPI. sim32.c's actual provider is stdcall; the worker
   export maps VdmMapFlat to _VdmMapFlat@12 (tools/build/worker/exports.cmake).
   The rebuilt native NTVDMD and DPMI callers and real VDD use this same ABI.
   Removing WINAPI would mismatch that contract, not merely change formatting.
2. The original x86 no-op Unmap/Flush branch is selected only without CCPU.
   CCPU uses the original function declarations in the other arm. sim32.c
   defines both actual functions; its Flush updates video memory and performs
   SAS memory notification. dpmiselr.c:265 calls it after updating LDT contents.
   Restoring unconditional x86 no-op macros would also macro-expand the real
   function definitions and erase that caller's required behavior. Keep the
   existing original two-arm structure; no second cache or callback system.

The paired DPMI precomp.h keeps its existing CCPU VM-aware mode calculation,
which tests both CR0.PE and EFLAGS.VM. Add only #undef before overriding the
restored header default. The expression and all callers are unchanged. The
new line is inside the existing appended adapter block, after original text;
no original diagnostic line moves. The rest of the file was reviewed, and its
existing descriptor/table binding declarations are unchanged.

## Mirror result

| File | Previous baseline / points / lines | XP points / lines | Delta |
| --- | --- | --- | --- |
| nt_vdd.h | OpenNT / 1 / 480 | 2 / 4 | +1 point, -476 lines |
| dpmi32/precomp.h | XP / 1 / 10 | 1 / 11 | 0 points, +1 line |

The first old whole-file block included CRLF and reordered declarations.
Restoring XP LF/order exposes two small ABI/backend differences. The paired
one-line macro cleanup is within the standing small mirror-change approval.
Batch: +1point/-475lines, no added or removed mirror files. Global1324files,
774byte-identical,785nonerror-identical/539different,7742points/154167lines.
One baseline transition;1251XP/73OpenNT. Byte-sensitive LF/EOF/#line accounting.

## Delivery

Cached full build and runtime22 dual13-image26hash publication passed.
CCPU BIOS mapping123/123, VDD303 assertions, IOS70 assertions and both
wrong-width ABI negative controls passed. Real VDD/MEM verified exports,
debugger register round trip, event writeback and matching task termination.
[Receipt](n021-vdd-result.json) retains the two initial fixture link failures.

The existing all-contract fixture build exposed two missing test boundaries
from the already-restored XP VDD/IOS objects: RcErrorDialogBox in the VDD
fixture and HostUndefinedIo in the IOS fixture. Add same-signature test-only
providers: unexpected dialog fails immediately; IOS models no dynamic VDD.
Existing default-port/routing/handler assertions remain unchanged. No product
source or published image changed after the passing build; retain BIOS pass,
retry the failed VDD build, then run IOS and the outstanding live integration.
The first live invocation used the wrong terminal-observer CLI and returned64
before starting guest. Reused the historical live-event-final runner contract
with console-startup-observer; only this unexecuted integration was retried.

N0215/17 delivered; next ntcsrdll.h. Overall56/159 and S15 8/14 unchanged.
Two changed mirror files since P21 Full24/24. No original guest modification
or rebuilding. This does not claim physical I/O, VDD DLL unload or all device
hardware acceptance; these fixtures exercise their stated controlled scopes.
