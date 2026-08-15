# T201 S3 Mutation-Policy Resolution Evidence

S3 adds one generic resolution result derived solely from the shared profile
and registered owner/class:

| Session mode | Result |
| --- | --- |
| direct | direct host operation permitted |
| readonly | mutation rejected |
| overlay | session overlay required |
| virtual | virtual-volume operation required |

The resolver accepts no caller-supplied mode and names no BOP selector or
service. A result is policy only: `direct host operation permitted` does not
perform host I/O, and `readonly mutation rejected` does not choose a DOS error
until the owning historical provider is recovered.

Reproduce with:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools\Invoke-T201S3MutationPolicyProbe.ps1 `
  -RepositoryRoot (Get-Location).Path `
  -BuildRoot artifacts\build\t201-s3-mutation-policy-r1
```

The MSVC x64 `/MT` fixture passed with exit code zero for all four modes and
for unregistered-owner and mode-mask mismatch rejection.
