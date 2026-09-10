# M0 T401 S4 — WRITE compatible-DC diagnostic non-equivalence

## Scope

S4 admits one compile-time, x86-only marker in the original WOW32
`WG32CreateCompatibleDC` provider.  Its only purpose is to record the native
input DC and the original converted WOW16 return for an unchanged selected
`WRITE.EXE` workload.  It does not admit a USER, CSRSS, guest-media, return
value, or application repair.

## Matched diagnostic pair

The prior diagnostic DLL could not load beside an older parent because that
parent did not export the current mapping-manager surface.  This run first
relinked the current x86 parent and paired it only with the S4 provider:

| Image | SHA-256 |
| --- | --- |
| `current-parent-x86/original-softpc-process.exe` | `c6fb695f299b151732262103ae4829323ce1de56dccae95bbffb8401aa451404` |
| `wow32-write-probe-x86-r2/wow32.dll` | `255b9dfd30d483f7c9b2c5e2dd9c9ed34022847632cb02aa4e93c64811f77c1c` |

The parent exports `mapping_manager_lookup_value`,
`mapping_manager_lookup_identifier`, `mapping_manager_publish`,
`session_host_resource_mappings`, and `session_thread_current`; the diagnostic
WOW32 import graph names the same five symbols.  The generated provider graph
also contains `wuman.c`, `wow32.c`, and
`adapter-wow/wow_private_user_compat.obj`.  Thus this is not the earlier
missing-import failure and it has not omitted the selected FinalUserInit
compatibility adapter.

## One bounded observation

Only a new short diagnostic stage, `O:\t401s4r`, was used.  Its immutable
guest media remained unchanged; its `ntvdm32.exe` and `WOW32.DLL` were the two
matched images above.  The manifest was updated solely for that DLL identity.
The Console-owning observer invoked:

```text
console-startup-observer.exe O:\t401s4r\ntvdm32.exe O:\t401s4r \
  build\M0-T401\S4\write-probe-same-parent.txt -f -o --command \
  system32\WRITE.EXE --observation-timeout-ms 30000 --observe-without-diagnostics
```

The resulting report records all of the following:

```text
loaded-wow32=yes
loaded-wow32-path=O:\t401s4r\WOW32.DLL
visible-top-level-window-first-title=WOW Error
child-window-02-title=user.exe and user32.dll are mismatched.
result=timeout
```

There is no `write-probe compatible-dc` marker.  Consequently, the run never
reached `WG32CreateCompatibleDC`; it supplies neither an input DC nor a return
handle and cannot classify any WRITE-local DC failure.

## Disposition

The clean ordinary S3 control reaches WOW32, WOWEXEC and WRITE's original
low-memory dialog.  This separately built provider instead reaches an earlier
private USER FinalUserInit mismatch panel, despite loading the matched
provider.  It is therefore a non-equivalent diagnostic outcome, not evidence
that `CreateCompatibleDC` fails or succeeds.

Diagnosing or changing that private USER handshake would broaden beyond S4's
single named API body and is expressly a stop condition.  No repair is
selected, no second S4 run is performed, and the staged guest remains
unchanged.  The task retains only the exact limitation: the named return
boundary is unavailable under an S4-compatible observation.
