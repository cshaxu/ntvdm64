# M0 T246 S3 — SoftPC 02/06 package native checkpoint

## Verification

Formal MSVC x64 `/MT` Ninja graph `build/M0-T246-S2/formal-r3` rebuilt:

- `t225-s23-opennt-int06-provider-fixture.exe`;
- `t246-s2-softpc-machine-interrupt-ingress-fixture.exe`;
- `ntdos64-native.exe`.

Both focused fixtures exit zero.  The second uses the actual active generic
ingress and covers `02`, `06`, checked-preflight STOP and a non-package
decline.

The bounded source-built guest checkpoint used the original DOS/WOW16 bundle:

```text
ntdos64-native.exe --dos-root build\output\dos --wow16-root build\output\wow16 \
  --mutation-mode direct --instruction-tick-budget 1000000 \
  --observe-bop-sequence --observe-ud-sequence --observe-dem-open \
  --observe-demfile-create build\output\dos\share.exe
```

## Observed result

The passive BOP sequence contains the formerly blocking `06` at
`0216:0740`, each with `disposition=1` (typed resume).  It is no longer the
old active-registry pass-through.  The first run of those accepted entries is
immediately followed by a distinct accepted owner:

```text
bop[40] selector=06 disposition=1
bop[41] selector=06 disposition=1
bop[42] selector=06 disposition=1
bop[43] selector=50 service=1e disposition=1
```

The trace later contains additional `06` calls.  They are new guest calls at
the same historical handler entry, not evidence that the original registry
miss survived: every recorded `06` is `disposition=1`.  The bounded run
eventually returns the existing machine execution detail `6`
(`UNEXPECTED_LOOP_RETURN`) after later guest control flow.  That outcome is
outside the completed `02/06` ingress contract; this S does not claim full
guest execution continuity.

The Direct host side remains healthy during this checkpoint: six accepted
`50:12` opens and `CreateFileOem` conversion/first/retry errors all report
zero.

## Result

The source-derived shared SoftPC `02/06` package is formally linked, locally
exercised through the active route, and observed accepting the source-built
guest's prior `06` edge before it advances to a distinct later BOP owner.
No bx-core/bx-mantle source was changed.  The remaining repeated handler
calls and eventual non-package loop are transferred as future native
continuity investigation, not repaired by extending this provider.
