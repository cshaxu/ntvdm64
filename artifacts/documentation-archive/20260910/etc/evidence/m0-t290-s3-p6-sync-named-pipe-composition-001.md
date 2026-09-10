# M0 T290 S3 P6 — Original synchronous named-pipe composition

## Scope

The exact original `mvdm-host/vdmredir/vrnmpipe.c` translation unit is
executed through its public synchronous helper bodies.  This delivery does not
enable a Redirector selector, create a redirector-specific policy provider, or
enter the source's async request, callback, interrupt, NetBIOS, DLC, mailslot
or NetAPI paths.

## Recovered original contracts

- `VrIsNamedPipeName` retains the source's remote `\\server\\pipe\\...`
  classification, slash normalization acceptance and local/ordinary-path
  rejection.
- `VrAddOpenNamedPipeInfo`, `VrIsNamedPipeHandle` and
  `VrRemoveOpenNamedPipeInfo` retain their original name/handle list lifecycle
  and idempotent missing-handle result.
- `VrReadNamedPipe` and `VrWriteNamedPipe` run unchanged against a local,
  public Win32 duplex named pipe opened with an overlapped client end.  The
  source's event, `RememberPipeIo`/`ForgetPipeIo`, result and byte-count order
  are therefore exercised on both architectures.

The test-side `HANDLE` is never a guest value: it is native local state passed
only to the original host helper.  The separately recorded `BP:BX` historical
handle interpretation continues to resolve through
`adapter-mvdm-host-out/redir` and the session `host_resource` mapping before
any selected BOP path can reach this code.

## Commands and result

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/build/New-T290S3VrNamedPipeNameNinja.ps1 -Architecture x86 -RepositoryRoot O:\repos.hobby\ntvdm64
ninja -C build/M0-T290/S3/vrnmpipe-name-x86
.\build\M0-T290\S3\vrnmpipe-name-x86\bin\t290-s3-vrnmpipe-name-fixture.exe

powershell.exe -ExecutionPolicy Bypass -File tools/build/New-T290S3VrNamedPipeNameNinja.ps1 -Architecture x64 -RepositoryRoot O:\repos.hobby\ntvdm64
ninja -C build/M0-T290/S3/vrnmpipe-name-x64
.\build\M0-T290\S3\vrnmpipe-name-x64\bin\t290-s3-vrnmpipe-name-fixture.exe
```

Both fixtures compile, link and exit `0`.  The original `vrnmpipe.c` remains
unchanged.  Its unselected historical guest pointer/string and async branches
remain outside this proof and retain their S3/S4 source-ledger dispositions.
