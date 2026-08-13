# T188 S2 v5 boot-file predicate fixture 001

Date: 2026-08-12  
Packet: M0 T188 S2  
Disposition: fixed input succeeds; the real pass-through is state-dependent.

## Method

The existing `bx_ntvdm_adapter_runtime_test` received one focused v5 branch. It installs its existing v5 profile, then calls the unchanged public `bx_ntvdm_adapter_runtime_v2_dispatch` once with the exact observed BOP bytes, `fault_rip=0x732`, real-mode `DS:DX=8DC8:33D9`, and then takes the one pending multi-write. It neither changes production code nor decomposes, repeats, or reorders the v2 compound expression.

The test asserts the original runtime outcome: `RESUME` at `0x736`, one write at `0x91059`, and the 14-byte `C:\CONFIG.SYS` payload. This covers provider availability, COMMAND plane gate, boot-file preparation and host-session queue as a single operation.

## Execution

The existing MinGW island `artifacts/build/current/adapter-cli-r1` rebuilt only its existing adapter-runtime test target and ran:

```text
bx-ntvdm-adapter-runtime-test.exe --t181-v5-install
```

It exited zero. No Bochs target, engine, CLI executable, firmware bundle or guest process was built or run.

## Result and interpretation

The exact static profile/register shape accepts `54:0C`. The T187 runtime pass-through is therefore not explained by a bad selector, command gate, provider initialization, `DS:DX` range, pathname preparation or an empty session in isolation. It depends on state accumulated before the real call, most plausibly the adapter host-session pending/lifecycle state, but this fixture does not and must not infer which state field differs.

T187 S1 already proved that replacing the runtime short-circuit with separate observations changes the route under inspection. S2 consequently closes with a bounded result: no safe fixture can identify the live first-false predicate merely by reusing the same inputs. The next S may only audit whether an existing, side-effect-free state observation exists; it must not add a selector-specific handler or re-run the guest.
