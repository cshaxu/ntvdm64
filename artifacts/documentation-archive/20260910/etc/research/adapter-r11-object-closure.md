# r11 Adapter Object Closure

## Scope

This is deliberately not a Bochs executable build. It is the first bounded
compile after the Bochs 2.6 object graph was defined: adapter runtime sources,
the generic #UD wrapper and the generic ordinary-RAM copy implementation.

The fresh r11 container was configured with the existing native `nogui` UCRT
recipe. Its outer configuration invocation exceeded the caller's 124-second
limit after generating `config.h` and the required Makefiles; no compile was
requested by that invocation. The bounded object commands then used that
generated configuration with explicit UCRT temporary-directory paths.

## Compiled Objects

The following object target request completed successfully on 2026-08-10:

```
make -j1 \
  ntdos64_adapter/byob_profile.o \
  ntdos64_adapter/byob_identity.o \
  ntdos64_adapter/byob_image.o \
  ntdos64_adapter/bx_ntvdm_cpu_state_abi.o \
  ntdos64_adapter/bx_ntvdm_instruction_window_abi.o \
  ntdos64_adapter/bx_ntvdm_guest_range.o \
  ntdos64_adapter/bx_ntvdm_guest_write_abi.o \
  ntdos64_adapter/bx_ntvdm_host_service_catalog.o \
  ntdos64_adapter/bx_ntvdm_host_transaction_abi.o \
  ntdos64_adapter/bx_ntvdm_session_resource_abi.o \
  ntdos64_adapter/bx_ntvdm_host_service_plane.o \
  ntdos64_adapter/bx_ntvdm_host_session.o \
  ntdos64_adapter/bx_ntvdm_exception_abi.o \
  ntdos64_adapter/bx_ntvdm_adapter_runtime.o \
  cpu/exception.o
make -j1 memory/misc_mem.o
```

This proves C/C++ declaration and include closure for the installed adapter
session bridge and `BX-MEM-007`. It does not prove linking, reset, guest
execution, a device model, or the native Bochs r10 full-build issue resolved.
No `make bochs.exe` and no full `make` was run.

## Remaining Gate

The next executable evidence must use one explicit minimal reset ROM that
causes an admitted transaction, observes the payload in ordinary RAM, and
stops under a bounded instruction/benchmark policy. It must be launched with
the adapter environment pair supplied by the CLI, not hand-set as an implicit
developer environment.
