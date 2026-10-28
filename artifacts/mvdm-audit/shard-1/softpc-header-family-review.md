# S10 shard 1: SoftPC CPU, host and device declaration family

25 pending `softpc.new/base/inc` files were audited as a source/header family. Each SHA-256 matches both OpenNT reference editions. `softpc-header-family-evidence.json` records each hash, exact direct textual includes and all normalized VALID object dependencies from the current x86 worker build; replay `PYTHONDONTWRITEBYTECODE=1 python -B artifacts/mvdm-audit/shard-1/softpc-header-family-observer.py`. Direct include is a basename match and may be conditionally compiled or resolve a different search-path file; VALID dependency identifies the exact current path but does not prove every declaration is active. No other profiles or runtime were tested.

| File | Role and bounded evidence/disposition |
| --- | --- |
| acc_efi.h | Generated effective-address access entries; no direct include or VALID dep observed. Pending generator/profile and actual consumer. |
| cpu2.h | CPU-related structure declarations; 0/0. Pending selected owner and behavior. |
| d_inst.h | Disassembler instruction IDs; 2 direct references, 0 deps. Pending actual disassembler selection. |
| dsktrace.h | Disk tracing flags; 3 direct references, 2 deps (diskbios and fdisk objects). Selected header does not prove debug flags execute. |
| gendrvr.h | Generic-driver initialization declarations; 1 direct reference, reset.obj dep. Compile-time GEN_DRVR/CDROM gates actual paths. |
| getptis.h | Historical PTI-related declaration; 0/0. Pending actual platform and consumer. |
| gfiempty.h | Historical generated-interface support; 0/0. Pending profile/consumer; do not infer feature status. |
| gore.h | Video direction/offset constants; 4 direct references, 0 deps. Pending selected video implementation attribution. |
| harness.h | Test-harness constants; 0/0. Pending harness/profile attribution. |
| hg_cpu.h | Historical host CPU register structures; 1 direct include, 0 deps. Pending active profile and ABI. |
| hg_sas.h | Historical host SAS interface; 0/0. Pending profile/consumer. |
| host_gwi.h | Host GWI definitions; 3 direct includes, 34 deps. Selected declaration path; exact enabled defines and graphics behavior need further evidence. |
| host_sas.h | Host SAS macro interface; 1 direct include, 91 deps. Selected declaration path; actual memory provider, failure and lifetime not proven by header count. |
| hosthunt.h | Historical host hunter/debug support; 0/0. Pending profile/consumer. |
| hunter.h | Hunter/debug declarations; 5 direct references, one VALID dep reset.obj. Actual conditional execution remains unverified. |
| j_c_lang.h | Generated C language/register definitions; 36 direct references, one VALID dep in cvidc_vector_binding_fixture.obj (a test object, not proof it enters NTVDM). Distinguish selected runtime generated functions independently. |
| jcgensyn.h | Generator parser tokens; 0/0. Pending generator-tool attribution. |
| jformate.h | Generator/load format definitions; 0/0. Pending tool/profile attribution. |
| lm_code.h | Legacy encryption-related constants; 0/0. Pending original caller/profile; no runtime behavior inference. |
| lq2500.h | Printer/device constants; 0/0. Pending device/profile attribution. |
| mouse16b.h | Mouse I/O structure, one direct textual include, 0 deps. Do not infer mouse input unsupported. |
| msw_snd.h | Multimedia sound constants; 0/0. Pending actual selected audio path. |
| profile.h | Instrumentation declarations; one include and one dep, profile.obj. Whether profiling is enabled or linked remains unverified. |
| smeg_hea.h | Historical profiling/instrumentation constants; 0/0. Pending profile/consumer. |
| spc_icon.h | Historical icon mask/data definitions; 0/0. Pending resource/provider attribution. |

Zero/zero means zero direct include references in scanned current softpc.new C/H/INC/CPP sources and zero normalized VALID dependencies in this single worker tree, not a global negative or safe-deletion proof. The large header family crosses multiple independent subsystems; per-row evidence is classification input only, and no CPU, graphics, disk, mouse, audio or user-visible behavior is thereby accepted. No product, guest, build tree or shared ledger modified.
