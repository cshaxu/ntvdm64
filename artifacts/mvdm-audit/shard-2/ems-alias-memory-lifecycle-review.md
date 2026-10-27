# S10 shard 2: EMS alias and host memory lifetime cohort

The selected changed `softpc.new/host/src/nt_emm.c` and `nt_mem.c` were
re-diffed against frozen OpenNT; both remain partial. Original EMS page
numbering, frame map/unmap, host copy/exchange and EMM manager own algorithm
and result; the CCPU session mapping provider supplies absent NT kernel VDM
aliasing. No source/guest modification or runtime test occurred here.

## Concrete owners and tested profile

`nt_mem.c::InitIntelMemory` binds physical-map slots before returning its
backing allocation (DIV-035); `FreeIntelMemory` retires the mappings before
freeing `intelMem` and the header table. Original VdmAdd/Remove paths retain
allocation, alignment, cache invalidation and physical page-table update.
`nt_emm.c` retains original `VdmMapDosMemory`/`VdmUnmapDosMemory` and page
number ABI (DIV-096). Its DIV-267 `em_loads`, `em_stores`, `em_moves` keep
original native macros for unaliased RAM and use the session guest-memory
copy/move only when an alias overlaps. `sas_overwrite_memory` remains the
original destination invalidation. Negative length is explicitly rejected.

The separate `mvdm_softpc_guest_memory_copy_*` helpers acquire and release
bounded chunks under the active session. `move` preserves overlapping address
order by selecting front/back chunks. These operations do not themselves
establish a global transactional exchange; each chunk can fail after earlier
chunks committed. Map lifecycle and producer registration remain separately
owned by `mvdm_softpc_physical_mapping` and the original caller.

T420 S30 records real DOS guest EMS allocation/map/remap/unmap/free with an
explicit EMS profile; the default COMMAND profile can have EMS deliberately
uninstalled. T420 S21 cites earlier T406 guest EMS map/move/exchange evidence.
These are historical bounded witnesses; they do not by themselves prove the
current image's injected allocation/revocation failure or every EMS profile.
README DIV-035/096/267 records the respective mirror boundaries.

## Specific bounded failure and profile gaps

- `host_exchg_con_to_con` performs load(source) → move(destination,source) →
  store(temp,destination). Under the new lease path any later operation can
  return FAILURE; if move has changed source and store then fails, this source
  has no rollback. Chunked copy/move also may commit earlier chunks before a
  later acquisition/release failure. This is a source-level failure boundary,
  not proof such a failure is reachable in an active session or that the
  original public EMS contract promises atomic exchange. A controlled actual
  original-caller fixture should inject failure before/after each chunk and
  exchange step, record guest memory, return and any session retirement, then
  compare the original caller's contract before selecting a remedy. Avoid
  inventing rollback semantics without that evidence.
- Under `BACK_M`, original `EM_pointer(temp,length)` returns the end of its
  allocation and original `EM_loads`/`EM_stores` adjust host pointers by
  `-(length-1)`. The alias helper branches adjust the guest address but pass
  the host pointer directly to copy_from/copy_to. For a positive multi-byte
  exchange, source reading suggests a potentially incorrect host buffer range
  in that compile profile. Current x86 Ninja graph and generator search found
  no `BACK_M` definition; no preprocessed/build proof or execution of that
  profile is available. Treat as a source-level conditional-profile candidate,
  not a reproduced current product defect. If BACK_M is an admitted product
  configuration, first verify preprocessed selection and run bounded alias
  tests with guarded buffers; otherwise record it as outside current profile.
- Verify map initialize/release and unmap failure, session epoch change,
  overlap at frame boundaries, zero-length handling and whether dirty/cache
  invalidation occurs before any rejected write. Real guest and source fixture
  receipts must identify the same image and admitted memory profile.

No percentage is derived from the matching lines. Other XMS/DPMI page owners
and device assumptions require their own audit and are not reclassified here.
