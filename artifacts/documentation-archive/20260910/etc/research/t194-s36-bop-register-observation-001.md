# M0 T194 S36: One BOP-Register Observation

Packet: M0 T194 S36  
Disposition: one invocation completed with retained partial terminal logs; the
controller failed while hashing still-locked logs before it could write its
outcome JSON. No retry was made.

## Question

Does the frozen S35 listener engine reach DEM `50:21`, and if so which
`demIOCTL` branch does the copied register state identify?

## Inputs

The fresh evidence root is
`artifacts/analysis/t194-s36-bop-register-observation-001`. Its
`inputs.json` records the exact one-shot inputs:

| Input | SHA-256 |
| --- | --- |
| S15 runner | `706882B0EE453CB5E4A64A81A9E357375766DB4B99BDBAD74932577C1570CB37` |
| S15 shim | `57AD850F46782FFF798DBEA39EFE6C7EB69FAD3B192DDF2DA5850E696F3F6A41` |
| S35 listener bundle engine | `49D950AE537B0F9343F2B07E0A2B92DD6B18EB612D572A459559EC0FB3B2AB5D` |
| v6 profile | `F91B27EF63FEEEB64A7DCB18F25FCBF2B626DE05CFC1961F4D96CF21B4262548` |
| target | `69DABBDB754B358AC4FE4B22DE04C0E4C93076816F14BB0730CAA9FD223996FC` |

The invocation used the recorded S15 runner/shim, S35 bundle, v6 BYOB root,
one 15-second watchdog and `retryPolicy: none`. The retained partial logs are
`stdout.log` (389 bytes, SHA-256
`2D6323D6DBD6FE4B6C63E3F4D6B1AB032ED4CC3F5C93ED5D236323EDFA271DA2`) and
`stderr.log` (13,320 bytes, SHA-256
`4F9A410BD05032448D36E0C1374CD7471E84C4332FDFDD862824C06B7A8DEEE5`).

## Procedure

One S34 bounded-controller invocation created the evidence root and launched
the immutable inputs. At watchdog cleanup, `taskkill /T /F` was denied. The
controller used its bounded wait, but its `finally` block then attempted a
direct `Get-FileHash` on a log still held by the surviving child process. That
hash operation threw before `observation.json` could be written. The resulting
verified process tree was then explicitly terminated by its exact wrapper PID
only; no name-based or unrelated process action was used. This manual cleanup
does not constitute a second observation.

## Observations

The listener observed the prior startup sequence `50:11`, `12`, `50:3B`,
`50:0F`, `50:1B`, `50:32`, `54:05`, `50:46`, `15`, and `50:0D`. The existing
`50:46` provider committed `writes=14 bytes=2a` before later BOPs.

At the requested boundary it recorded exactly:

```text
selector=50 service=21 ax=4408 bx=f002 cs=9346 ...
adapter boundary passed through ... bytes=c4c45021
```

`AL=08` selects the `demIoctlChangeable` drive-metadata query and `BL=02`
selects the zero-based C-drive slot. This is the source-proven metadata branch
from `t194-s29-demioctl-source-contract-001.md`; it is not `AL=0D` or `AL=11`
raw-media/BDS access. The current v6 composition had no selected live-snapshot
provider for this pair, so the BOP passed through unchanged.

Later logs show unhandled `50:37`, followed by repeated native prefetch/stack
fault diagnostics and the original Bochs terminal triple-fault panic. This
does not establish a normal return, a `54:11` marker, or the cause of the
later machine fault.

## Interpretation And Follow-up

Confidence is high for the copied `50:21` identity and register facts because
they come from the default-off generic listener in the frozen S35 engine.
Confidence is deliberately absent for a controller outcome, exit code, or
post-watchdog guest state: the controller produced no `observation.json`.

S37 is limited to making final log hashing total: a missing or locked log is
recorded as a null hash while the terminal record is still written. It does
not rerun S36. A later, separately admitted BOP-provider packet must treat
`AL=08` and `AL=09` as the coherent source-backed drive-metadata group, use
the immutable CLI drive snapshot, and leave raw-media operations deferred.
