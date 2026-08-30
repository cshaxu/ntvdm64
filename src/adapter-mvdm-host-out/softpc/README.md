# softpc family

This family supplies only source-shaped, non-machine bindings required by
selected OpenNT MVDM host code. Original SoftPC/CCPU40/SAS code remains the
machine owner. This component neither owns a CPU nor substitutes execution.

## Registered divergences

| ID | Original purpose | Reason | Implementation | Files |
| --- | --- | --- | --- | --- |
| ADAPTER-SOFTPC-001 | Declare historical host error forms required by reached source. | The matching declaration survives in a different original header. | Build-island declaration bridge only; no mirror body changes. | `include/error_abi.h` |
| ADAPTER-SOFTPC-002 | Preserve a host identity in a 32-bit MVDM field. | A native pointer cannot become MVDM data. | Reuse the session host-resource mapping instance. | `include/mvdm_host_identity.h`, `mvdm_host_identity.c` |
| ADAPTER-SOFTPC-003/006 | Retain a 16:16 guest location and copy synchronously. | `GetVDMAddr` cannot provide a durable native alias. | Retain numeric location; acquire a fresh bounded session-memory lease per access. | `include/mvdm_guest_location.h`, `mvdm_guest_location.c` |
| ADAPTER-SOFTPC-004 | Expose temporary VDD SFT/JFT mutation. | Original permanent guest aliases and host handles are unsafe. | Session-owned shadow plus explicit bounded write-back. | `include/mvdm_vdd_sft_shadow.h`, `mvdm_vdd_sft_shadow.c` |
| ADAPTER-SOFTPC-008 | Preserve SAS A20 entry points used by XMS. | A20 state is owned by selected CCPU40/SAS. | Direct same-shaped binding to original `c_sas_*20_bit_wrapping`. | `include/mvdm_a20.h`, `mvdm_a20.c` |
| ADAPTER-SOFTPC-010 | Maintain the cached BIOS INT 15 vector. | The full historical keyboard product shell is not selected. | Preserve IVT compare/update/reset order through original SAS calls only. | `include/mvdm_int15.h`, `mvdm_int15.c` |
| ADAPTER-SOFTPC-015 | Publish a host object identity in a DPMI register pair. | A native pointer cannot be guest data. | Reuse `mvdm_host_identity` and the session mapping instance. | `include/mvdm_host_identity.h`, `mvdm_host_identity.c` |
| ADAPTER-SOFTPC-REDIR-001 | Bind Redirector pointer-shaped source calls. | No raw guest-memory alias may cross a provider boundary. | Synchronous bounded copied-range scope. | `include/mvdm_redirector_pointer_scope.h`, `mvdm_redirector_pointer_scope.c` |
| ADAPTER-SOFTPC-022 | Observe one outer original `host_start_cpu` invocation. | Nested original returns must not be mistaken for session completion. | Bind selected session, call unchanged original entry, record its return, unbind. | `include/mvdm_softpc_execution.h`, `mvdm_softpc_execution.c` |
| ADAPTER-SOFTPC-023 | Bind original external physical memory records. | NT4 used a raw host pointer in a 32-bit field. | Publish a stable mapping-manager ID and resolve only immediate CCPU physical access. | `include/mvdm_softpc_physical_mapping.h`, `mvdm_softpc_physical_mapping.c` |
| ADAPTER-SOFTPC-024/025 | Locate firmware and DOS media for original loaders. | NT4 system-directory lookup is not product media selection. | Resolve caller buffer under app/session-configured immutable roots. | `include/mvdm_softpc_firmware.h`, `mvdm_softpc_firmware.c` |
| ADAPTER-SOFTPC-026 | Handle original installer probe system files. | Product must not mutate admitted host drives. | Preserve no-result completion without creating files. | `include/mvdm_softpc_firmware.h`, `mvdm_softpc_firmware.c` |
| ADAPTER-SOFTPC-027/028 | Retain COMMAND packet/redirection identities. | Original code kept permanent guest aliases and 32-bit pointer values. | Session-local numeric location and mapping-manager ID; no pointer exposure. | `include/mvdm_command_guest_state.h`, `mvdm_command_guest_state.c`, `include/mvdm_command_redirection.h`, `mvdm_command_redirection.c` |
| ADAPTER-SOFTPC-029 | Provide CCPU `c_effective_addr(selector, offset)`. | Kernel VDM supplied a raw process alias. | Use original CCPU descriptor state and return numeric guest-linear address only. | `include/mvdm_softpc_effective_address.h`, `mvdm_softpc_effective_address.c` |
| ADAPTER-SOFTPC-030 | Enter the original process-level SoftPC sequence. | The product executable must retain its own CLI entry, while the original source defines `main`. | Compile exact `obj.vdm/ntvdm.c` with only `main` renamed; bind app to that body without duplicating its initialization order. | `include/mvdm_softpc_execution.h`, `mvdm_softpc_execution.c`, `../mvdm-host/softpc.new/obj.vdm/ntvdm.c`, `../../tools/build/New-T310OriginalSoftpcNinja.ps1` |

Retired recovery experiments that replaced CCPU40 registers, SAS, frame
transactions, WOW pointer scopes, or `host_simulate` are evidence only in
`docs/etc/legacy_code/retired-bochs/`; they are not production inputs.
