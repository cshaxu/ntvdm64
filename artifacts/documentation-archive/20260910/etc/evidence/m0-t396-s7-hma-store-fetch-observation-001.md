# M0 T396 S7 — HMA physical-store/fetch observation 001

## Question

Does the reached original `disa20_xfer` lose or corrupt its far-return target
between its physical word stores and CCPU's immediately following `RETF` fetch?

## Inputs and procedure

The source ledger is `src/mvdm-guest/dos/v86/inc/lmstub.asm`:
`disa20_xfer` performs `SVC 50:36`, installs the child stack, then executes
`push DS`, `push SI`, and `retf`.  The observation is default-off and is
called only after the existing `phy_w16` dispatch and after the existing
`RETF` stack pops.  It receives copied scalar addresses, values and SAS class;
it does not retain a guest mapping or alter CPU state.

The corrected Console-owning non-debug observer launched the freshly
relinked formal x86 product (`SHA-256
6819BF2BC0C36600FCA6EF3E8B92052727C29F833B570B6DD864598F25167675`) as:

```text
O:\t396\ntvdm32.exe -f -o --command system32\krnl386.exe
```

The first source/trace pass established the actual final transfer stack at
`8AFF:007C`, physical `0008B06C`.  The relinked product was then run with
only the four-byte scalar observation window there.  The focused fixture also
proves that missing configuration writes no report and that the observer
disables itself after the selected fetch.

## Observation

The second run reached original `50:36` and `00A7:1113`, then recorded:

```text
MVDM-HMA-STORE ordinal=1 requested=0008B06E masked=0008B06E width=2 value=8AFF sas=00000000 state=copied
MVDM-HMA-STORE ordinal=2 requested=0008B06C masked=0008B06C width=2 value=0000 sas=00000000 state=copied
MVDM-HMA-FETCH ordinal=3 source=00A7:1113 stack=8AFF:0000007C requested=0008B06C masked=0008B06C target=8AFF:0000 state=copied
```

This relinked run produced exactly these three HMA records: the one-shot
observer did not record later reuse of the selected RAM words.  The Console
observer subsequently recorded the existing `0xc0000409` process termination.
That termination is not attributed to this transfer: the physical words and
fetched target agree exactly.

## Correction and disposition

S6's `0xA006C`/`0xA006E` fixed-address expectation was a pre-push arithmetic
error.  More importantly, a prior scalar trace's `FFFF:FFFF` attribution is
not reproduced at the actual store/fetch boundary.  This record therefore
rules out a proposed HMA/A20 or `phy_w16` repair.  This closes S7: it has
proved the selected stores and fetch agree and that ownership lies beyond this
machine boundary.  The normal product's subsequent `0xc0000409` belongs to a
separate native-fast-fail provenance audit.  A debugger-only observation has
shown a fast-fail parameter but changes the terminal result, so it is not a
valid product acceptance or caller-attribution source.  The successor must
not change app launch routing, guest media, BOP behavior, CPU semantics, or
product hardening.
