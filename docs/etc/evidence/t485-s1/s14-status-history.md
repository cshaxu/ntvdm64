# Archived S14 status detail

Moved from CURRENT during S15 P9; original closure facts retained.

## S14 Closure Record

9/9 directories verified for delivery in S14. [N004 BaseClient](n004.md)
and [receipt](n004-result.json): XP vdm.c plus required
original vdm.h; paired WOW registration field leads N005. Cached full build,
dual13-image26hash publication, targeted tests and consolidated Full23/23 pass.
First Full exposed Run16's rejection of XP's PE64 classification; one native
acceptance condition now admits both32/64 through the existing launch path.
Nine unaffected Full passes and three Console cases were retained; only failed
and unexecuted cases continued. Runtime2 is the current published package.
Nonerror vdm.c diff: OpenNT1point/6087lines -> XP16points/239lines; original LF
restoration exposes residual points. Paired basemsg.h adds0points/2lines.
S14 P1 delivered N004. [N005 Base declarations](n005.md)
([receipt](n005-result.json)) completes P2: cached full
build, dual13-image26hash publication and affected BASE/RPC/COMMAND/WOW tests
passed. Four mirror files reduce nonerror6points/983lines to0/0; ntcsrmsg.h
leads N021 only as a dependency. Runtime3 was N005's published package. N004 Full remains
its checkpoint; no repeat Full is claimed. Four files since that checkpoint.
[N006 BaseSrv VDM](n006.md) and
[receipt](n006-result.json) complete P3 verification:
XP desktop/logon shared-WOW records, real worker task/Console binding and
original request validation. Runtime6 cached-build/dual13-image26hash pass;
Full24/24 consolidates21 initial passes,2 bounded continuations and the new
SOL Game > Options modal-dialog gate. Initial CAF find-KVM failure is retained;
no full-suite rerun. Runtime6 differs from Full-tested runtime5 only in named
package-ID/PE timestamp fields after format/#line cleanup. Primary mirrors
OpenNT3points/6586lines -> XP17points/95lines; paired vdm.c drops4lines,
5 original declaration headers are byte-exact. This is the new ten-file
checkpoint. Other paired directories remain open.
[N007 system fonts](n007.md):42/42 actual files are
byte-exact OpenNT/XP originals, with matching complete directory membership.
No production or media change; nonerror0points/0lines. P4 documentation-only
registration retains N006 runtime6 Full24/24 without rebuild/republication.
The unchanged42-file audit uses that same checkpoint. [N008 OLE conversion](n008.md) completes P5 verification: XP output initialization and active-path line mapping, runtime7 cached full build and dual13-image26hash publication, real Win16 descriptor roundtrip/invalid-input/ownership pass. Nonerror OpenNT1point/5096lines -> XP22points/3936lines; LF restoration exposes residual points. One changed file since the checkpoint; no repeat Full. [N009 Netlib](n009.md) completes P6:10 existing sources plus required original midluser/RPC declarations; runtime8 cached full build, dual13-image26hash publication, targeted packstr and Full24/24 pass. Primary nonerror10points/7071lines -> 15points/19lines; paired header costs are separately recorded. This is the new Full checkpoint. P6 committed/pushed0262e23b1. [P7 omission repair](p7-omissions.md) completes
bounded H markfrom/N001-N003 follow-up:14 changed files, protected XP
ntcsrmsg/BaseClient and retained S13 closure. Runtime9 cached full build,
dual13-image26hash publication, x86/x64 capture3475 checks each and Full24/24
pass. Nonerror XP byte diff14points/16684lines -> 11points/257lines; CSR
normalized9points/500lines -> 5points/240lines. Eight modified files are XP
byte-exact. No original guest change. This is the new ten-file Full checkpoint.
[P8 N010 RAP](n010.md) delivered7 XP-byte-exact
sources plus2 paired declarations; runtime10 cached full build, dual13-image
26hash publication,11 RAP assertions and real guest NetAPI pass. Full remains
P7 runtime9;9 files since checkpoint. [Global mixed-baseline census](n010-census.json):
1319 files; byte-exact729; nonerror same731/different588;7765points/214351lines.
P8 delta: total0, exact+8, nonerror same+8/different-8,points-8,lines-7736.
[P9 N011](n011.md) relocates apiworke.h to XP's
public/internal/net/inc path byte-exact and removes old include search paths.
Runtime11 cached full build, dual13-image26hash publication, real guest NetAPI
and Full24/24 pass. This is the ten-file checkpoint after N010's nine files.
P9 global census1319files:byte-exact730,nonerror same732/different587,
7764points/214251lines. P9 delta:total0,exact+1,same+1,different-1,
points-1,lines-100;1 relocation and OpenNT->XP baseline switch.
1173 files use XP,146 retain OpenNT-family originals. S14 delivery8/9,
overall47/159; next N012 XACTSRV, then mandatory S14 closure Full.




[P10 N012](n012.md) delivers XP XACTSRV handlers and
conversion helpers; obsolete xsdata removed, original xsdebug replaces local
macros. Runtime12 cached full build, dual13-image26hash publication, RAP11/11,
enum42/42, real guest NetAPI and Full24/24 passed. Existing Win95 exit1 boundary
retained. Current global census1319files:byte-exact740,nonerror same742/different577,
7760points/197875lines; P10 delta0files,+10exact,+10same,-10different,-4points,
-16376lines.1188 XP and131 OpenNT-family originals. All N004-N012 are verified
for delivery:9/9 S14,48/159 total. [S14 closure audit](s14-closure.md) passed; S15 admitted.
