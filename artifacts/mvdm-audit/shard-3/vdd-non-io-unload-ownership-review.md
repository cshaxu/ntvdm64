# VDD non-I/O resources at module unload

Read-only source review, with existing controlled actual-object tests; no new product, guest, or device operation. Source and fixture identities and test result limits are recorded in `../vdd-native-role-review.md` and `vdd-loader-quiescence-review.md`.

## IRQ reservations

Public `VDDReserveIrqLine(hVdd, line)` stores the module handle in `IrqLines`, with `host_ica_lock` protecting reserve and release. `VDDReleaseIrqLine(hVdd, line)` clears only a matching owner under that lock; invalid/nonowner release fails. Source documentation explicitly allows these APIs at any time and separately warns that `VDDSimulateInterrupt` does not enforce reservation ownership. `VDDDeInstallIOHook` only walks its port ranges and adapter; it does not scan `IrqLines`. The examined `ISV_DeRegisterModule` calls `FreeLibrary` and clears the ISV table but likewise contains no IRQ-owner sweep. No source evidence establishes that the loader automatically releases reservations, or that every real VDD releases them in detach. This is a lifecycle obligation, not a demonstrated live leak or proof that original API guarantees automatic release.

The existing actual `nt_vdd.obj` fixture exercises allowed IRQ 11/12/15, automatic choice/exhaustion, explicit owner release and rejection of wrong owner; it records lock calls. It does not load/unload an actual DLL while an IRQ remains reserved, test concurrent detach, or prove release by a module's real detach routine. A test-owned module and the real loader/owner path are required before attributing a lifecycle failure.

## Two distinct BOP tables

`bios.c` routes BOP 55 to `MS_bop_5` → `ms_bop`, whose private 10-slot `MS_bop_tab` stores bare callback pointers. `get_ms_bop_index` allocates a slot and `free_MS_bop_index` clears by index; no module handle is recorded in that table. The examined source has no separate module cleanup path or demonstrated external consumer of these allocation helpers. Existing actual-object fixture allocates all ten, checks exhaustion and 256 dispatch values, then explicitly frees all entries across eight rounds. This validates the controlled direct helper contract, not registration by or retirement of a real unloadable VDD.

Separately, BOP 58 → `MS_bop_8` → `ISV_RegisterModule` publishes `isvbop_table` with a real module handle and dispatch pointer. Guest deregistration → `ISV_DeRegisterModule` calls `FreeLibrary` and only afterward clears that table entry. This distinct table is the loader's own registration, not `MS_bop_tab`; the current code contains no explicit pre-unload callback quiescence. Failure paths after DLL load also directly release a reference. The actual table and module lifetime need a test-owned DLL/real-loader fixture; the generic loader-lock probe alone does not exercise them.

## Other callback registrations

`nt_msscs.c` separately owns VDD memory hooks and user-create/terminate hook lists keyed by module handle; their explicit deinstall functions and independent callbacks must be traced through actual callers and unload. The samples include `sample2` explicit memory hook removal during DLL detach, but this does not prove universal cleanup or concurrency safety. Process termination, installed-at-start VDD references, failed attach/export resolution, direct native `FreeLibrary`, callback in flight, and self-deregister are distinct boundaries. Do not silently assume port uninstall clears these resources, infer missing cleanup from a single sample, or blanket-sweep slots whose authority cannot be identified.

## Reusable verification and scope

Reuse `tests/mvdm-host/vdd_object_contract_fixture.c` for original IRQ and MS-BOP controls and `tests/observation/verify-softpc-object-contract.ps1 -Contract vdd` to bind real cached objects. Extend with test-owned module identities and deterministic event gates: reserve IRQ under two owners; unregister one and inspect whether the original detach releases it; verify wrong-owner/other-owner preservation, missing explicit release and same-handle reuse. Independently exercise the real BOP 58 loader with an isolated DLL for successful registration, failed export, dispatch in flight, explicit deregistration and reference counts, while the MS-BOP 55 tests remain a separate fixture. Check memory/user hooks and any guest/process lifecycle separately. The test must not manufacture evidence of automatic cleanup or require user hardware.

The current evidence supports specific original roles and controlled API behavior, not a complete module-lifecycle guarantee. Any host repair must be bounded to the chosen owner and original contract, track mirror differences, and avoid guest changes.
