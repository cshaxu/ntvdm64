# T95 S7 Adapter Launch-Declaration Installation Evidence

## Implemented Scope

For an accepted v3 profile only, adapter installation reads
`NTDOS64_ADAPTER_LAUNCH_KIND` and `NTDOS64_ADAPTER_LAUNCH_TAIL` exactly once.
It parses them with the same fixed launch-declaration codec used by the runner,
copies the result into adapter-private session state, and checks that `com`
means selected `TARGET.COM` while `exe` means selected `TARGET.EXE`.

The v1/v2 installation path does not require these values. A v3 installation
rejects a missing, malformed, overlong or kind-mismatched declaration before
setting `installed`. Win32 distinguishes a missing environment value from a
present empty tail by `GetLastError`; a present empty tail is accepted.

No target image is loaded, no read-only namespace entry is created, and no
guest/BOP/Bochs interaction occurs in this change.

## Verification

Fresh root `artifacts/build/t95-s7-adapter-launch-install-c11-001` used CMake
4.1.0 and GCC 16.1.0 to build only
`bx-ntvdm-adapter-runtime-test` (51 explicit modern-island objects). The
fixture first created a valid v3 target profile with no launch environment and
proved installation returns zero. It then set `com` and `/c smoke`, reset the
process-local adapter state, and proved installation succeeds. The broader
existing identity-to-pending-transaction checks also passed.

The executable printed:

```
bx-ntvdm-adapter-runtime-test: identity-to-pending-transaction path verified
```

## Next Gate

The adapter now has matching profile identity, target placement and copied
launch declaration. The next cohesive capability is to re-verify/load the
target image via `byob_image_load_exact`, then extend the adapter-owned
read-only namespace by exactly one canonical target entry. That capability
must retain the current three boot entries, give no host path/handle to a
guest, and prove open/read lifecycle and target-byte identity before any
`BOP_CMD:01` response service is admitted.
