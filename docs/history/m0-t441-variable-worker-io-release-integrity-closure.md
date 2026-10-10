# M0 T441 — Variable worker-I/O release-integrity closure

T441 closes the `COMMAND → ver` stall without changing the bounded variable
worker-I/O protocol.  The controlled replacement proved that the failure was a
mixed runtime package: the old released AMD64 `ntvwm.exe` did not participate
in the S5 I/O release sequence expected by the newer x86 `ntvdm.exe`.  On
native-child completion, the DOS worker therefore timed out reacquiring the
frontend route (`ERROR_TIMEOUT` at `nt_event.c:1604`).

The release now contains a matching pair:

| Image | SHA-256 |
| --- | --- |
| `ntvdm.exe` | `EA385722141854F2696A96549FA6CBCC798136E1AAD92F610524ABDBC2DEB1E7` |
| `ntvwm.exe` | `4503AFB838D057E99EA29DA14309EBADBED45F27699FBC077476BB09B5AA02A8` |

Both `assets/release` and `O:\winnt\system32` were verified against the
updated release manifest.  The new real-channel probe queues a key, consumes
it, and immediately writes output over the variable-record path.  It passed,
as did the existing 307,200-byte and 1,310,720-byte transport test.  The
formal deployed pair also passed repeated `COMMAND → ver`, exit, and re-entry
from the retained terminal session.

The full product matrix is intentionally not claimed: it remains unavailable
to this small release-integrity repair because its legacy runner still uses
prohibited `subst Z:`.  Detailed A/B hashes and traces are retained in the
[T441 evidence](../etc/evidence/m0-t441-s1-command-input-regression-bisection.md).
