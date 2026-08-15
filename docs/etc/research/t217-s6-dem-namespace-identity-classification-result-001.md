# T217 S6 — DEM namespace identity classification result

## Scope

S6 adds a default-off fixture record at the existing copied DEM namespace
transaction. It contains only identity class, selected drive, admitted-state,
namespace attachment/generation and existing typed result. It contains no
pathname bytes, guest address, host path, handle, provider pointer or routing
decision.

## Verification

`Invoke-T202S4DemWholeProviderProbe.ps1` rebuilt the complete DEM provider
with MSVC x64 `/MT` at
`artifacts/build/t217-s6-dem-identity-provider-r4`. Its focused fixture proves
that two declared immutable Opens classify the second one as `DECLARED`, slot
1, ready, admitted, while retaining ordinary Open/handle behavior.

The fresh full-current-source native composition at
`artifacts/build/t217-s6-dem-identity-native-r3` completed its existing
controlled guest terminal and recorded:

```text
t217-s6 dem-identity seen=3 captured=1 class=3 attached=1
  generation=4e534002 drive=2 admitted=1 disposition=1 ax=0002
  flags-mask=00000001 flags-value=00000001
```

Class 3 is `OTHER_ADMITTED`: the second normal Open resolved to admitted C:,
and the declared namespace was attached, but the resolved identity was not a
ready declared image. The existing provider therefore took its ordinary local
file fallback and returned the already observed DOS file-not-found result.

## Consequences

This removes three explanations: the normal Open is not a BOP ingress miss,
not an excluded/default drive, and not an unattached declared namespace. It
also does not prove the raw pathname; that data deliberately remains inside
the bounded existing transaction.

The next owner package is guest SYSINIT command-name realization: original
`sysinit1.asm`/`sysinit2.asm` command-name data, the source-built NTDOS image
layout and the current startup materialization contract. No DEM fallback
rewrite, special `50:12` handler, DOS EXEC replacement or Bochs change is
justified.

## Recovery Ladder

| Rung | Disposition |
| --- | --- |
| Original OpenNT source | Required reference: it owns `commnd` (`\\COMMAND.COM`) and SYSINIT's boot-drive/default-drive/Open sequence. |
| Smallest seam | Next map must verify that the source-built guest image realizes that source contract before changing any host provider. |
| External intrusion | Rejected: native CPU and provider mechanics reached a valid DOS result. |
| Newly authored behavior | Rejected: both guest source and existing source-derived DEM composition exist. |
