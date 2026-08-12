# T117 S1 machine-composition seam build closure 001

T117 adds a default-off, selector-blind Bochs-to-machine-composition probe.
It only passes copied generic exception/window facts and a transient three
operation mechanics context. With no installed historical handler, the
component declines every call and does not change RIP, CPU state or exception
control flow.

Focused checks passed:

- `Test-BochsMachineCompositionBoundary.ps1` proves the Bochs block contains
  no selector, marker-byte, OpenNT/SoftPC/DOS/adapter or control-flow term;
- the pre-existing generic #UD boundary check still passes; and
- `bx-ntvdm-machine-bop-v1-test` passes default decline and negative inputs.

The isolated x86 derivative
`artifacts/build/current/t117-machine-composition-derivative-r3` retains all
Bochs archives and recompiles exactly one Bochs object plus one project object:

| Object | SHA-256 |
| --- | --- |
| `cpu/exception.o` | `6D4F939636455462B0D160ABE0BAB2768F1DE0E8F77804A1784AC4730F76D19D` |
| `machine/bx_ntvdm_machine_bop_v1.obj` | `F034EFAA18D3FF10257D78C93FF39FF37069E05ACF501863B7F213AEDCA47FF9` |

The generated shim hash is
`446A9EE3E504564E16A2CEFBCB68930F7C049068263CD79DD4F6E63DC6F0FAD7`.
No archive, executable link, handler invocation or guest runtime was run.
One existing `bx_address` to `unsigned int` warning appeared while compiling
`exception.cc`; it is not attributed to this seam.

T118 must separately define the accepted handler result and guest resume
contract before `unexpected_int` may be invoked.
