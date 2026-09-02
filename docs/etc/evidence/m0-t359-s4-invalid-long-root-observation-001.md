# M0 T359 S4 P1 — invalid long-root observation disposition

## Attempt record

One console-owning, non-debug observation was launched from the new immutable
stage `artifacts/runtime/m0-t359-s4-command-transient`. Its staged product hash
was `B8B852035E5ADC6585C0DEE05DACCE3130350582F2512DA116C193491D9D9B27`.

The stage's effective `mvdm` root was:

```text
O:\repos.hobby\ntvdm64\artifacts\runtime\m0-t359-s4-command-transient\mvdm
```

The observer records 74 characters for that root. The original NTDOS
`commnd` contract admits at most 63 visible `shell=` value bytes. The stage is
therefore capacity-invalid before any interpretation of its timeout.

## Result

The observer timed out after 8000 ms and produced no BOP, SAS-store or
exception report. That absence is deliberately **not** attributed to the
COMMAND transient lifecycle, app admission, CPU40, NTDOS or DEM. The long-root
input alone makes the run non-evidence for all of those owners.

## Disposition

S4 is closed as an invalid container attempt. It consumes no valid frozen
runtime observation. The replacement S5 is restricted to exactly one new,
non-overwriting stage beneath the previously approved short root `O:\ntvdm64`,
with identical product/media inputs and the same `TrnSeg` two-byte observation.
No source change, guest/media mutation, retry of the invalid stage or new
diagnostic is authorized by this disposition.
