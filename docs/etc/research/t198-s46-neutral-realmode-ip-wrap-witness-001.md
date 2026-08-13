# T198 S46 neutral real-mode IP-wrap witness

## Question

Does the current CPU5/Pentium-MMX `bx-core`/`bx-mantle` closure execute a neutral five-byte 16-bit instruction that begins at real-mode `CS:FFFF` and continues at `CS:0000`, without OpenNT, BOP, adapter, device or firmware input?

## Inputs

- `tests/bx-mantle/t198_s46_realmode_ip_wrap_fixture.cc` enters `1000:FFFF` (physical `1ffff`).
- Its sole entry byte is `2e`; the existing bounded mantle mechanical action writes `89 1e 18 00 f4` to physical `10000`. Together these form `2e 89 1e 18 00` (`mov word ptr cs:[0018], bx`) followed by `HLT` at `1000:0004`.
- The fixture uses the existing finite runner and 64-tick controlled stop. It has no BOP bridge request, OpenNT image, provider, adapter, product startup, device archive or CLI input.

## Reproducible procedure

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/Invoke-T198S3FiniteNativeRunProbe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t198-s46-realmode-ip-wrap-r4 `
  -ExternalFixtureSource O:\repos.hobby\ntdos64\tests\bx-mantle\t198_s46_realmode_ip_wrap_fixture.cc `
  -HostArchitecture x64
```

The r4 run compiled the full CPU5/Pentium-MMX source membership and fixture under MSVC x64 `/MT`, linked PE32+, and exited zero. Its retained `run.log` contains the original core diagnostic:

```text
prefetch: EIP [00010000] > CS.limit [0000ffff]
```

The fixture's finite timer subsequently performs controlled cleanup. The terminal copied register query is `0000:00000000`, because the fatal prefetch path resets retained CPU state before that post-loop query; it is not evidence of the split instruction's entry state. The exact prefetch diagnostic is the relevant observation.

## Interpretation and follow-up

This reproduces the S45 source-derived boundary without guest composition. It confirms a generic current core execution/fetch-width defect, not a DOS, NTIO, BOP, adapter, device or host-capability problem. S47 may now admit one registered selector-blind `bx-core` repair and a positive fixture that reaches the `1000:0004` terminal. It must not add guest or OpenNT vocabulary.
