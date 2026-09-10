# M0 T195 S3: Adapter-Owned BOP Observation

## Question

Can the current BOP catalogue/listener trace be removed from Bochs while
preserving copied-fact BOP observation at adapter ingress, without changing a
provider, guest result, machine handler, device, or runtime claim?

## Inputs

- T195 S1 identifies BX-ABI-040 and BX-TRACE-043 through BX-TRACE-046,
  BX-TRACE-059 through BX-TRACE-061 as the live catalogue/listener repair
  group.
- T195 S2 fixes the copied event/state/window and generic result boundary.
- The previous listener in `refs/bochs/cpu/exception.cc` called the adapter
  catalogue, decoded BOP identity, formatted selector/service trace data, and
  additionally performed stack/frame guest-RAM diagnostics.

## Change

`refs/bochs/cpu/exception.cc` no longer declares
`BX_NTVDM_ENABLE_BOP_CATALOG_LISTENER`, includes the catalogue, decodes BOP
bytes, formats BOP identity, or reads the diagnostic stack/frame.  Its generic
adapter interceptor now sends the already copied exception facts to the
adapter-owned `bx_ntvdm_exception_observer_v1_observe` function and logs only
the observer's opaque diagnostic string.

The new adapter observer uses the pre-existing adapter catalogue and emits the
established `ntdos64 adapter bop observed` identity marker from copied event,
state, and instruction-window values.  It has no CPU result, provider,
guest-memory, or mutable machine operation.  The old stack/frame trace is not
recreated: restoring it would require a separately admitted, bounded generic
guest-read transaction and is not necessary to preserve BOP identity
observation.

The current-adapter derivative generator now compiles the observer as an
adapter object and removes the obsolete Bochs listener macro from its
exception-object recipes.

## Verification

The following checks passed on 2026-08-12:

1. `tests/bx-ntvdm-bop-catalog-listener-boundary.ps1` proves Bochs retains no
   catalogue/listener/selector-formatting terms and the adapter observer has
   the copied-fact catalogue trace without guest-memory or result operations.
2. `tests/bx-ntvdm-adapter/Test-BochsUdInterceptBoundary.ps1` proves the
   remaining Bochs #UD transport has no BOP/OpenNT/DOS/WOW/DEM/COMMAND token
   or marker byte.
3. `New-T98S1CurrentAdapterEngineDerivative.ps1` generated the fresh
   `artifacts/build/t195-s3-generator-smoke` closure with
   `-BopRegisterObservation`.
4. In that root, the MSVC x86 command
   `nmake /nologo /f ntdos64-t98-current-adapter.mak
   ntdos64-t98-current-adapter.exe` linked successfully.  The generated
   command compiles `cpu/exception.o` once and
   `adapter/bx_ntvdm_exception_observer_v1.obj`, then links the latter in the
   explicit adapter object list.  It makes no Bochs archive or device archive
   a prerequisite.

The compiler reported the pre-existing `exception.cc(1499)` C4244 warning;
there was no error or observer-specific warning.

## Interpretation

The catalogue BOP identity and observation responsibility is now
adapter-owned.  Bochs still owns only generic copied exception delivery and
opaque diagnostic output for this path, and has no catalogue selector/service
formatting or BOP-specific build switch.  The separately existing historical
machine-handler `02h`/`06h` selection is deliberately still present until S4;
it is not a catalogue/provider observation path.  The result is a source/build
boundary repair, not a claim that all BOP providers run or that the guest
executes continuously.

## Follow-up

S4 must move the separate historical `02h`/`06h` machine-handler selection
out of Bochs.  Its typed native mechanics callback may retain checked RAM and
port operations, but handler selection and its result interpretation must be
adapter-owned.
