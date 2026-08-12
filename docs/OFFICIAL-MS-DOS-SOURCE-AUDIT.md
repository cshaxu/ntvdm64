# Official Microsoft MS-DOS Source Audit

## Purpose and Scope

This is a read-only provenance and applicability audit of the locally cloned
official Microsoft MS-DOS repository. It does not add its files to an OpenNT
build, nor does it claim that an MS-DOS artifact is interchangeable with an
NT4 NTDOS artifact.

## Checked Input

```text
repository: https://github.com/microsoft/MS-DOS.git
commit:     2d04cacc5322951f187bb17e017c12920ac8ebe2
scope:      MS-DOS 1.25, 2.0, and 4.0 source snapshots
license:    repository MIT License; the 4.0 subtree carries its own
            IBM/Microsoft MIT license notice
```

The repository README identifies the contents as MS-DOS 1.25, 2.0, and 4.0.
It is not a source release of Windows NT 4.0 NTDOS, NTIO, DOSX, NTVDM, or
VDMREDIR.

## Findings

| Question | Result | Consequence for this research project |
| --- | --- | --- |
| Does it contain NT4 `NTIO.SYS`, `NTDOS.SYS`, `DOSX.EXE`, `REDIR.EXE`, or the NT VDM host services? | No. | It cannot close the OpenNT NTDOS runtime or host-service build graph. |
| Does it contain early DOS core, command interpreter, PC/AT BIOS, storage, keyboard, display, ANSI, NLS, and memory-manager source? | Yes, notably in `v4.0/src/{BIOS,CMD,DEV,DOS,MEMM}`. | It is a legitimate historical reference for generic DOS and PC/AT behavior only. It must not be substituted into an NT4 artifact build. |
| Does it supply an input needed by the checked OpenNT NTDOS source? | No. The OpenNT checkout already tracks its own `base/mvdm/dos/v86/inc/copyrigh.inc` (blob `d854d5da283a2891c403ef29866f2018ac25baa1`); the MS-DOS 4.0 file is a distinct earlier-version text input. | The official repository neither closes an OpenNT input gap nor is it an admissible replacement for OpenNT's versioned source file. |
| Does it contain historical tools and binary libraries? | Yes. The v4.0 tree includes legacy tools and prebuilt libraries. | Their presence is not source closure and they are not adopted by this project without a separately documented provenance and reproducibility decision. |

## Admissible Use

1. Read-only historical comparison of generic DOS/PC-AT contracts, such as
   BIOS initialization, INT 13h/16h behavior, device request packets, and
   COMMAND-style process conventions.
2. Independent experiments whose input and output are clearly labelled
   **MS-DOS 4.0**, not NT4 NTDOS.
3. Provenance evidence that similar historical filenames do not establish
   source equivalence across MS-DOS and NT4 NTDOS generations.

## Excluded Use

- Do not copy components, include files, binaries, libraries, or build tools
  from this repository into OpenNT staging merely because they have a similar
  filename or DOS role.
- Do not use non-public or leaked MS-DOS source material as a reference,
  reconstruction input, or build dependency.
- Do not describe any output from this repository as NT4-compatible, NTDOS,
  NTIO, or a replacement for the requested NT DOS file set.

## Result

The official source repository is useful evidence and a legally clear
historical reference, but it does not resolve the current OpenNT source-closure
blocker and does not change the NTDOS build plan.
