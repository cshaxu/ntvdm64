# T95 S6 CPU3 original-project object inventory 001

## Scope

This is the admitted object-only inventory of the original 93 `.cc` CPU paths
from pinned `refs/bochs/vs2008/cpu.vcproj`, under the accepted CPU3 projected
configuration. It compiles no device, machine, SIM, GUI, adapter or OpenNT
source, and performs no static-library creation, link, executable or run.

The immutable analysis root is
`artifacts/analysis/t95-s6-msvc-x86-cpu3-project-objects-001-20260811-001`.
`logs/input-manifest.json` records all 93 source paths and input hashes. The
source `config.h` hash is
`A1FADC25502FB0A4644917B6F3A62987C125CC7335457F9C59E7CE7E62A45AC8`; the
copied CPU3 projection hash is
`C47030F79880C894A346F56E3AB3242924308983E00B5845EDCE849450C6C19D`.

## Procedure and interruption handling

Each original project path received exactly one BuildTools x86 invocation:

```text
cl.exe /nologo /c /Gy /MT /DWIN32
  /DBX_NTVDM_ENABLE_EXCEPTION_INTERCEPT=0
  /I"...\\bochs" /I"...\\bochs\\cpu" /I"...\\bochs\\instrument\\stubs"
```

The foreground orchestrator was forcibly stopped by the execution host at
about 64 seconds after 13 successful source objects, before source 14 began.
There was no compiler diagnostic or surviving compiler process. The registered
continuation verified those exact thirteen original-order object names and
compiled only sources 14 through 93 once each in the same root. This was an
external execution-time interruption, not a source failure or a retry.

## Result

All **93/93** original project CPU sources produced one MSVC x86 object.
The continuation result lists 80 entries, all exit code zero; the initial 13
objects are present and the final object set count is 93. The aggregate log
has zero `fatal error`, `error C`, or `error LNK` diagnostic matches. The only
repeated diagnostic is the already-known projected-config `FMT_LL` C4005
warning. `logs/object-hashes.json` retains the object hash inventory.

The successful set includes the static-family units that a hand-selected
CPU3 list had omitted: VMX (`vmx.cc`, `vmcs.cc`, `vmexit.cc`, `vmfunc.cc`),
SVM, AVX/AVX2, XOP, XSAVE and the named 64-bit source families. Their
successful object compilation does not enable them in the CPU3 guest profile.

The retained log/input/result/object-hash file SHA-256 values are:

| File | SHA-256 |
| --- | --- |
| `input-manifest.json` | `CE9448F0B3696FC3EE5BB81E1ED4A6BB87C005A5F0B1AA1E5BB5C5E8F3D380AB` |
| `compile.log` | `32AA3151760EA5A9F830F09094BBB53F3818EFA51901E8B250EF33AFE79E0A51` |
| `continuation-results.json` | `68A4080B5656CC5FF6CE5A11FD68AAD4FA77BE7E4EF73D5E59982F347A82A248` |
| `object-hashes.json` | `831AA46ADF54A711F31A9A9D437025362B998CEC58ED9452754961A4A09BC511` |

## Interpretation and next gate

The original CPU subsystem is available as a coherent MSVC/x86 object island
under the projected CPU3 configuration. This eliminates a compiler/toolchain
block; it does not establish a CPU static library, link closure, CRT choice,
machine initialization, reset, firmware, device behavior, adapter bridge or
guest runtime.

The next gate is a read-only symbol/retention inventory across these 93 CPU
objects and the previously isolated minimal-machine objects. It must classify
the remaining native CPU providers versus non-CPU product-shell roots before
one new link observation can be considered.
