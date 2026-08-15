# T95 S6 original CPU project closure

## Question

What is the original Bochs 2.6 CPU source closure that a future MSVC Win32
minimal-machine target must start from after the static opcode-table decision?

## Input and procedure

The pinned source project's `refs/bochs/vs2008/cpu.vcproj` was parsed read-only.
Its `File` entries with a `.cc` suffix were counted and compared with the
static-opcode evidence in `t95-s6-cpu3-static-opcode-closure-decision.md`.
No project invocation, compiler, linker or generated configuration was used.

## Observation

The original project is a static-library project (`ConfigurationType=4`) with
two Win32 configurations, Debug and Release. It declares **93** CPU `.cc`
translation units. The list includes the expected foundational decode/state
units (`cpu.cc`, `event.cc`, `exception.cc`, `fetchdecode.cc`, `init.cc`,
`io.cc`, paging, protection, real/v8086 control and arithmetic/string families)
as well as feature-family units such as `vmx.cc`, `svm.cc`, `vmfunc.cc`,
`vmcs.cc`, `xsave.cc`, AVX, XOP, TBM and 64-bit files.

Twenty declared paths match the architecture/extension family names
`64`, `fetchdecode64`, `xsave`, `avx`, `vmx`, `svm` or `xop`. Their presence
does not enable those CPU features in a CPU3 guest profile; it demonstrates
the project-level CPU compilation model that supplies the static opcode
metadata's referenced members.

## Decision

The 93-unit original CPU project is the starting upper bound for the Bochs CPU
toolchain island. A future CPU3 minimal-machine recipe may derive a smaller
set only through original source preprocessor evidence that preserves every
decoder reference; it may not use an ad-hoc hand-selected handler list.
Until such proof exists, the complete original CPU project closure is the
safe, non-reimplementation choice.

This is deliberately distinct from the machine product shell. The CPU project
does not admit GUI, configuration frontend, plugin, disk, timer, CMOS, PIC,
PIT, VGA, networking or other device object sets. Those remain deny-by-default
and separately gated.

## Peripheral root disposition

The parallel source review preserves the prior negative decisions:

- `bx_ntvdm_minimal_sim_initialize` constructs the original `SIM` interface
  through `bx_init_siminterface`; narrowing its virtual surface requires a
  class/lifecycle redesign and is not admitted.
- `bx_pc_system` construction and the GUI/device `exit` path inhabit the same
  original lifecycle owner. No access-only alternative class is evidenced.
- `SAFE_GET_GENLOG` reaches the original logging type, whose fatal shutdown
  path calls `bx_atexit`. No accessor-only source-object split is proven.

No additional intrusion is selected for these roots. The next permitted
mechanics investigation is a separately admitted CPU-project object inventory
under the accepted projection, followed by a fresh retained-owner analysis;
it is not a link, reset or runtime authorization.
