# T198 S47 real-mode IP-width repair

## Question

Can the adopted CPU5/Pentium-MMX core execute a generic 16-bit instruction
whose bytes span `CS:FFFF` to `CS:0000`, while retaining the existing neutral
HLT and selector-blind generic `#UD` STOP mechanics?

## Repair

S46 proved that a five-byte instruction at this boundary reached
`EIP=00010000` and failed in prefetch. The only upstream sequential increment
sites are in `cpu.cc`, chained `BX_EXECUTE_INSTRUCTION` in `instr.h`, and
the temporary split-fetch advance in `icache.cc`.

`BX-CORE-076` adds `BX_CPU_C::advance_ip(unsigned)` in `cpu.h` and replaces
only those advances. It retains full RIP in long-64 mode, existing 32-bit EIP
behavior for a 32-bit code segment, and truncates the advance through the
existing 16-bit IP view for a 16-bit code segment. It contains no selector,
opcode, guest address, BOP, OpenNT, adapter, device, firmware or host term.

The required UD2 regression exposed two pre-existing early-STOP lifecycle
holes that the prior watchdog path had hidden. The already registered
generic STOP now sets the native async-event latch before its existing
`longjmp`, so the native loop observes its existing finite stop latch.
`BX-MANTLE-077` deactivates the finite private watchdog through the original
PC-time API before unregistering it on an early stop. Neither change alters
`#UD` recognition or outcome routing.

## Reproducible verification

All commands compile the full CPU5/Pentium-MMX source membership as MSVC x64
`/MT` PE32+, with no Bochs product target, device archive, adapter, BOP,
OpenNT, CLI or MinGW object in the finite executable.

```powershell
# Positive split instruction: r4 exits zero and prints 1000:00000005.
powershell.exe -ExecutionPolicy Bypass -File tools/probe/Invoke-T198S3FiniteNativeRunProbe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t198-s47-realmode-ip-wrap-r4 `
  -ExternalFixtureSource O:\repos.hobby\ntdos64\tests\bx-mantle\t198_s46_realmode_ip_wrap_fixture.cc `
  -HostArchitecture x64

# Existing one-byte HLT: r3 exits zero.
powershell.exe -ExecutionPolicy Bypass -File tools/probe/Invoke-T198S3FiniteNativeRunProbe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t198-s47-finite-hlt-r3 `
  -HostArchitecture x64

# Existing generic UD2 STOP: r3 exits zero without timer panic.
powershell.exe -ExecutionPolicy Bypass -File tools/probe/Invoke-T198S3FiniteNativeRunProbe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t198-s47-finite-ud2-r3 `
  -UdStopFixture -HostArchitecture x64
```

The positive run's `run.log` contains `WARNING: HLT instruction with IF=0!`
and `t198-s46 terminal=1000:00000005`, proving the split five-byte `MOV`
executed and then the HLT at `1000:0004` executed normally. It contains no
former `EIP [00010000] > CS.limit [0000ffff]` diagnostic. The HLT r3 log
retains its normal finite controlled stop. The UD2 r3 log has no prefetch or
`unregisterTimer` panic; its process exit proves the accepted typed STOP
reached finite cleanup.

## Bound and follow-up

This closes only generic native real-mode code-offset mechanics and finite
fixture cleanup. It does not prove the original NTIO run has reached a BOP,
nor enable DOS, BOP provider, host capability, device or CLI behavior. S48
must rerun the unchanged source-built NTIO plan and classify the next observed
guest state before any further capability work.
