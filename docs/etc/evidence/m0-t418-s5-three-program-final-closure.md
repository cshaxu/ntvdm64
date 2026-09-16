# M0 T418 S5 — three-program final closure

## Question

Can the final executable-owned layout retain the original-owner boundaries,
avoid a generic shared Win32 component, and deliver the three-program package
without changing the established DOS/Console lifecycle?

## Inputs

- S1--S4 delivery baseline `df34f4ce5`.
- `tools/build/New-T310OriginalSoftpcNinja.ps1`, using the approved Git Bash
  Node 22 toolchain and MSVC Win32/x86 `/MT`.
- Fresh root `build/M0-T418/S5/formal-x86-002`.
- Runtime package `O:\winnt`; all raw observations remain below
  `O:\winnt\logs`.

## Ownership disposition

`run16` owns discovery, broker startup, worker creation/registration and the
parent wait. `basesrv` owns the authenticated endpoint, versioned protocol
and original BaseSrv-record assembly; its client library is linked by run16
and ntvdm without creating duplicate protocol providers. `ntvdm` owns the
worker-local session, guest-memory lease and all local Console/SoftPC/monitor/
Redirector/VDD/WOW/debugger bindings. Original algorithms remain in `mvdm` or
`opennt-host`.

The only non-executable shared implementation island is
`opennt-abi/host-compat`: named same-shaped historical host ABI bindings with
finite public-Win32/NTDLL closures. It owns neither broker wire policy nor
worker/session/guest state. `product-abi` and `product-package` remain
stateless version/layout data. There is no `common` or `win32api` component.

All retired generic roots (`app`, `session`, `broker`, `adapter-mvdm-host-in`,
`adapter-mvdm-host-out`, `adapter-opennt-host`) contain zero non-README files.
The selected formal graph names no source input beneath those roots. Older
task-specific generators still retain historical paths, but are not selected
production inputs and are not silently revived.

## Procedure and observations

1. Updated the final component proposal and the current architecture/coding
   authorities with the shared-code rule. The formal generator's source
   manifest now reports ntvdm-owned families and final product dispositions,
   rather than stale S3 WIP text.
2. Generated the clean x86 graph and built its original candidate (444 steps),
   then explicitly linked `run16.exe`, `basesrv.exe`, `ntvdm.exe`,
   `VDMREDIR.dll`, BaseSrv fixtures, RTL/environment fixtures and CCPU/C-VID/
   original-memory fixtures (72 further steps). The only observed diagnostics
   were pre-existing original-source warnings and the VDMREDIR DESCRIPTION
   linker warning.
3. Passed `basesrv-reservation-test`, `basesrv-service-reservation-test`,
   `cpu40-descriptor-domain-fixture`, `rtl-x86-fixture`,
   `environment-projection-fixture`, `ccpu-halt-reset-test`,
   `original-external-memory-test`, and `cvidc-vector-binding-fixture`.
   The memory fixture covers original disk-buffer alignment, EMS cross-window
   copying, SAS access, allocation grow/shrink, DIB sharing and backing
   recovery.
4. Deployed the four formal runtime files to `O:\winnt`; source and deployed
   SHA-256 values matched. The automatic real ConPTY observer passed
   `COMMAND → MEM → EDIT → MEM` (child's original COMMAND exit code is 1;
   observer exit is 0). Its raw/cell/RPC/stream logs are
   `m0-t418-s5-final-command-edit-mem.raw*` under `O:\winnt\logs`.
5. On the deployed formal package, separately passed broker loss (launcher and
   worker exit, no replay), worker loss (bounded launcher outcome with broker
   survival), and launcher loss with two workers (broker and unrelated worker
   survive). Each run reported `PASS after-loss: 0`.

## Interpretation and limitation

Confidence is high for the admitted no-behaviour-change component closure:
fresh formal build/link, original-owner fixtures, normal interactive command
path and all three standalone failure directions pass. This is not a WOW16
completion claim and does not convert historical unselected generators into
current production inputs.

## Follow-up

T418 closes. The existing queue remains the authority for the next source
recovery package; no new task is allocated here.
