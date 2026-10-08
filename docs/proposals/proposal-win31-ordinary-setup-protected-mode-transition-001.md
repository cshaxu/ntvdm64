# Proposal: Windows 3.1 ordinary-Setup protected-mode transition recovery

## Objective

Make the normal original Windows 3.1 Setup path diagnosable and, only after a
source-owned cause is proved, repair the post-copy transition into its first
Windows/protected-mode load. The intended workload is the owner-confirmed
path: ordinary hardware detection, destination selection, file copy, then the
original "Please wait while Setup loads Windows" boundary — never `/I`.

## Established baseline

The build-owned derived media representation is necessary and already proved:
Setup copies `KRNL386.EX_` and `WIN386.EX_` before its first Windows load, so
the two approved candidate images must be present as the compressed source
inputs. The representation changes no other media files and has exact
round-trip/recovery evidence. Nevertheless, the real normal path still stalls
after copy.

The previous no-COM/no-LPT candidate was manually falsified and retired. The
direct-disk warning has a separate original Ignore continuation. Neither fact
proves the post-copy condition.

## Required sequence

1. Establish one valid, bounded automated witness whose working directory,
   executable package root, console ownership, and input sequence are
   demonstrably equivalent to the user-facing setup launcher. A witness
   rejected before guest execution is harness evidence only.
2. Use only default-off, bounded source witnesses to distinguish the reached
   transition: final Setup/DOS code, DOSX/DPMI entry, real-to-protected mode,
   first WOW/kernel compatibility path, and any subsequent source-defined
   wait/failure branch.
3. Attribute the first blocking condition to unchanged guest media, inherited
   OpenNT/MVDM host code, or project-owned integration. Apply the source
   policy: retain unchanged-guest defects; reuse a matching SoftPC correction
   for inherited host code if one exists; repair project-owned code only with
   focused positive and negative proof.
4. If a repair is admitted, preserve original hardware-dialog Abort/Ignore
   semantics, build the selected x86 CCPU40 closure, and prove a normal Setup
   run crosses the post-copy boundary before claiming any installation result.

## Boundaries

- No `/I`, synthetic machine profile, serial/parallel emulation, host hardware
  discovery, runtime helper, drive substitution, live guest-memory patch, or
  replacement execution backend.
- No retail-media mutation, `SETUP.EXE`/`SETUP.INF` rewrite, or extra derived
  media file beyond the already approved two source representations without a
  new owner decision.
- No claim that enhanced-mode reliability, normal Windows exit, or broad
  Windows 3.1 compatibility is repaired merely because Setup crosses one
  transition.

## Acceptance

The package closes with either a source-supported disposition recorded for the
actual first post-copy blocker, or an owner-approved repair that passes a real
normal Setup crossing of that boundary. Harness failures, traces alone, and
the pre-copy media test are not substitutes.
