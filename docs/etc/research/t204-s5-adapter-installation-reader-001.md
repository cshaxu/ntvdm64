# T204 S5 P4 — adapter installation configuration reader

The adapter now has an installation-time reader for the paired CLI child
environment values.  It accepts only both nonempty paths, opens each once for
read, limits each raw OEM input to 4095 bytes, copies the bytes into the fixed
T204 provider input, captures OEM root/code-page facts, and discards every
path and handle before provider construction. `CONFIG.SYS` must be nonempty;
an empty `AUTOEXEC.BAT` remains a valid explicit input, matching the provider
ABI rather than accidentally reviving a synthetic empty default.

`bx_ntvdm_adapter_runtime_v1_install` creates the current direct-mode COMMAND
session-context policy, invokes the reader, and binds the ready provider into
the boot namespace.  Missing, unreadable, oversized or invalid paired input
rejects installation rather than retaining the synthetic production result.

The strict MSVC x64 `/MT /W4 /WX` translation-unit compilation of both the
new reader and current `bx_ntvdm_adapter_runtime.c` passes.  During this work
three pre-existing strict-build drifts were repaired: shadowed loop locals, a
stale `cmd_return_exit_code` call lacking its launch plan, and an uninitialized
DTA registration local.  No BOP or Bochs behavior changed.

An installation-level positive/negative fixture remains required before S5
closure.

The focused reader regression
`tests/bx-vdm/bx_ntvdm_startup_configuration_source_v1_test.c` was built and
run with MSVC x64 `/MT /W4 /WX`. It proves a copied nonempty CONFIG plus an
empty explicit AUTOEXEC, then rejects a missing paired value, empty CONFIG and
a 4096-byte source. This is reader-boundary evidence only; it does not replace
the pending adapter-installation fixture.

`tests/bx-vdm/bx_ntvdm_adapter_runtime_startup_configuration_install_test.c`
now supplies the paired environment, performs a full adapter installation,
deletes both source files immediately after installation, and then proves that
the bound COMMAND `54:0C` preparation returns `C:\\CONFIG.SYS`. Its MSVC x64
`/MT /W4 /WX` object closure links with `bcrypt.lib` and `ntdll.lib` and runs
successfully. The source-file deletion proves that dispatch does not retain a
source handle or path. The fixture deliberately still uses the legacy profile
materialization descriptors because the namespace initializer currently needs
them before bind; it does not claim the remaining synthetic initializer has
been eliminated.
