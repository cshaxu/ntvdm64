# T198 S104 complete fast-read native observation 001

## Question

What exact handle and flags arrive at the existing declined-path DEM `50:42`
marker after the S101 AX:BP codec correction?

## Inputs

- S103's source-built fixture/bridge marker.
- Exact S93 locked COMMAND/SHARE input and v7 profile, rebuilt by
  `tools/probe/Invoke-T198S97PostBootstrapNativeProbe.ps1`.
- Current `tools/build/t198-s50-bx-vdm-composition-manifest.json`.

## Procedure

One process was run at
`artifacts/build/t198-s104-fastread-complete-native-r3`.  Its own provenance
record reports `runs: 1`, x64, `/MT`, CPU5/Pentium-MMX and the inherited S93
input contract.  Its fixture return code was `2`; this is a bounded fixture
result, not a DOS program result.

Earlier S104 r1/r2 directories contain interrupted preparation attempts, not
a native executable, `run.log`, or a provenance record with a run.  They did
not execute guest code.  r3 is the sole S104 guest execution.

## Observations

`run.log` and `run.stderr.log` agree on the following line:

```text
fast-read observed=1 state=0032:6351 eax=00004e53 ebx=00000000 ecx=00000020 edx=0000000a ebp=00004005 flags=00000046 ds-si=9fba:0000 es-di=00a7:0137
```

The same run reports `observed-stop=1 next=50:42 plane=0/0/1` and
`error-lock=1`; no provider acceptance is implied by the recorder, because
the recorder is reached only on its pre-existing declined path.

For the corrected OpenNT handle codec, `AX` supplies the high word and `BP`
the low word.  The observed token is therefore `00004005h`.  `EFLAGS=46h`
has ZF set and CF clear.

- `run.log` SHA-256:
  `f59318f786316646ac64efd8463467fc5e38e3feaaa481686c4262e1ea166b00`.
- r3 provenance SHA-256:
  `9e63e4d3d2a37067ac5dcdc68dd034a54b172806411ffed335ca198d5cab038c`.
- Profile SHA-256:
  `3659427b5d6b6ca43930cd0439e9ef9f35e55ba26d5088d06686efe66ebd015c`.
- Current composition-manifest SHA-256:
  `50368b2d14e38d249a151fa1eeac61939d59fab5b8747b6b0dd499f1455df5dd`.

## Interpretation

S104 resolves the missing register-state ambiguity from S102: this is not a
reversed-token observation, and its ZF state selects the no-seek branch of the
current source-derived fast-read routine.  It does not prove why the current
composition declined the event.  In particular, the original `demdisp.c`
maps `SVC_DEMFASTREAD` to `demNotYetImplemented`; S105 must map the full
fast-I/O family and its NTDOS continuation before any provider change.
