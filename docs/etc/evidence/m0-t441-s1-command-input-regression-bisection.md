# M0 T441 S1 — COMMAND input regression bisection

## Question

Which post-`a3608c6df` change causes a DOS `COMMAND` session to echo a typed
command such as `ver`, then fail to produce its result or the next prompt?

## Owner observations

The owner used the same Windows Terminal CMD launch sequence for each package:

```text
CMD -> O: -> cd winnt -> run16 command -> ver
```

| Package/source boundary | Observation |
| --- | --- |
| `a3608c6df` complete ten-image package | Normal. |
| `644e1557f` complete package | COMMAND fails during startup; this is distinct from the post-input stall. |
| `3293669d5`, later `e2f17ce15`, and HEAD | `ver` is echoed, then the session stalls. |
| `b6671ad8e` worker bridge with `a3608c6df` native images | Normal; owner confirmed success. |

## Controlled bridge

`b6671ad8e` was exported with `core.autocrlf=false`, because a normal archive
conversion changed the mirrored source hash.  Its x86 `ntvdm.exe` and
`VDMREDIR.DLL` were rebuilt in build-owned output and deployed over the known
good `a3608c6df` native package only.

| Image | SHA-256 |
| --- | --- |
| `ntvdm.exe` | `7ADC650E7EF610A2C0D18557D5A8B0C12F4A9A82AD67A4AAC3D2DCF87C269AA2` |
| `VDMREDIR.DLL` | `F0A3BA2E12EF17E8A4BAAD12EA3919C45D4ED33300D5685DAAB71D353EA8974A` |

The focused `console-video-test.exe` passed.  The historical private-desktop
`console-client-test.exe --variable-video` reports Win32 error 6, matching the
known private-desktop limitation recorded in T440 evidence; it is not product
acceptance evidence.

## S2 controlled replacement and conclusion

The initial source boundary was insufficient: the published native worker had
not been refreshed with the S5 worker-I/O changes.  S2 held every other live
component fixed, stopped the test-owned runtime, and made this exact A/B
replacement in `O:\winnt\system32`:

| NTVWM image | `CMD → run16 command → ver` | NTVDM reacquire |
| --- | --- | --- |
| old `assets/release/ntvwm.exe`, SHA-256 `2F159EE0296EC822822DC33DB8BF9253DDCA042D41EA1026B1273DF1DB0C16C2` | Stalls after echo | `ERROR_TIMEOUT` (1460) at `nt_event.c:1604` |
| current native build | Prints `Microsoft Windows [Version 10.0.26300.9550]` | succeeds |

The temporary traces show the old worker never performs the compatible
release sequence.  The current pair performs DOS release, native acquire and
complete/release, then DOS reacquire.  Thus the defect is a mixed-generation
deployment, not a framing/order defect in the current bounded variable-record
protocol, mirrored OpenNT, guest `COMMAND`, CCPU, or the graphics publisher.

The corrective release refreshes both coupled worker images and the manifest:

| Image | SHA-256 |
| --- | --- |
| `ntvdm.exe` | `EA385722141854F2696A96549FA6CBCC798136E1AAD92F610524ABDBC2DEB1E7` |
| `ntvwm.exe` | `4503AFB838D057E99EA29DA14309EBADBED45F27699FBC077476BB09B5AA02A8` |

The bridge remains diagnostic-only.  The retained regression probe exercises
queued input followed immediately by output over the real variable-record
channel; it does not restore fixed 16 KiB records.
