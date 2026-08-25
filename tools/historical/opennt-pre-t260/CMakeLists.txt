# Research-only standalone input gate. SOURCE-MANIFEST.json defines the fixed
# input set. This project intentionally has no parent modern-runtime target:
# it must be configured in an x86 historical compiler/CRT island.
cmake_minimum_required(VERSION 3.20)
project(ntdos64_opennt_historical_inputs LANGUAGES C)

# This target is intentionally excluded from normal builds and is never linked
# by ntdos64-run. It cannot be combined with any non-OpenNT include root.
if(NOT CMAKE_C_COMPILER_ID STREQUAL "Clang")
    message(FATAL_ERROR
        "The OpenNT historical object gate requires clang-cl in a 32-bit "
        "toolchain environment; current MSVC rejects the historical SDK "
        "preprocessor constructs before source-level closure can be tested.")
endif()

# The archived CCPU compile used all 130 production units in ccpu386.  Keep
# that source closure intact: localfm.c and vglob.c were not in the archived
# response file and remain outside this gate.
file(GLOB NTDOS64_OPENNT_CCPU_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/base/mvdm/softpc.new/base/ccpu386/*.c")
list(REMOVE_ITEM NTDOS64_OPENNT_CCPU_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/base/mvdm/softpc.new/base/ccpu386/ccpusas4.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/base/mvdm/softpc.new/base/ccpu386/c_xcptn.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/base/mvdm/softpc.new/base/ccpu386/localfm.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/base/mvdm/softpc.new/base/ccpu386/vglob.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/base/mvdm/softpc.new/base/ccpu386/zfrsrvd.c")

add_library(ntdos64-opennt-ccpu-objects OBJECT EXCLUDE_FROM_ALL
    ${NTDOS64_OPENNT_CCPU_SOURCES}
    overlay/base/mvdm/softpc.new/base/ccpu386/c_xcptn.c
    overlay/base/mvdm/softpc.new/base/ccpu386/ccpusas4.c
    overlay/base/mvdm/softpc.new/base/ccpu386/zfrsrvd.c)

# Preserve the 130-unit source closure above independently from the historical
# software-FPU link profile. The latter is the same closure except ntstubs.c:
# fpu.c supplies the NPX implementation and the two units duplicate only the
# no-op initialise_npx/npx_reset exports. A prior archived 129-member FPU
# profile proves this exact selection; it avoids relying on MSVC's legacy
# duplicate-symbol warning or lld-link's /force:multiple escape hatch.
set(NTDOS64_OPENNT_CCPU_FPU_LINK_SOURCES ${NTDOS64_OPENNT_CCPU_SOURCES})
list(REMOVE_ITEM NTDOS64_OPENNT_CCPU_FPU_LINK_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/base/mvdm/softpc.new/base/ccpu386/ntstubs.c")
add_library(ntdos64-opennt-ccpu-fpu-link-objects OBJECT EXCLUDE_FROM_ALL
    ${NTDOS64_OPENNT_CCPU_FPU_LINK_SOURCES}
    overlay/base/mvdm/softpc.new/base/ccpu386/c_xcptn.c
    overlay/base/mvdm/softpc.new/base/ccpu386/ccpusas4.c
    overlay/base/mvdm/softpc.new/base/ccpu386/zfrsrvd.c)

# CVIDC is the separately generated C/video execution library selected by
# historical CCPU link profiles. Keep it separate from the CCPU archive until
# its original source closure and ABI role have been independently audited.
file(GLOB NTDOS64_OPENNT_CVIDC_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/base/mvdm/softpc.new/base/cvidc/*.c")
list(REMOVE_ITEM NTDOS64_OPENNT_CVIDC_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/base/mvdm/softpc.new/base/cvidc/ev_stubs.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/base/mvdm/softpc.new/base/cvidc/sascdef.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/base/mvdm/softpc.new/base/cvidc/vglob.c")
add_library(ntdos64-opennt-cvidc-objects OBJECT EXCLUDE_FROM_ALL
    ${NTDOS64_OPENNT_CVIDC_SOURCES})
add_library(ntdos64-opennt-cvidc-archive STATIC EXCLUDE_FROM_ALL
    $<TARGET_OBJECTS:ntdos64-opennt-cvidc-objects>)

# R1 C-VIDC-only vector evidence target. Compile the unmodified generated
# vglfunc.c a second time with its table owner renamed to generic Video. This
# emits the original typed 0--77 initializer without a copy, alias, link, or
# lifecycle hook. It deliberately does not select the CCPU 5-slot owner and
# is not a CCPU/SoftPC composition or runtime target.
add_library(ntdos64-opennt-cvidc-generic-video-vector-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/softpc.new/base/cvidc/vglfunc.c)

# R1 reconstruction provider: retain the original 0--77 C-VIDC initializer
# as a separate object, then bind only the three type-compatible original glue
# entries in 78--80. This is not a historical ccpu486 composition or runtime.
add_library(ntdos64-opennt-reconstructed-cvidc-video-tail-objects OBJECT EXCLUDE_FROM_ALL
    ${CMAKE_CURRENT_SOURCE_DIR}/../reconstruction/video/reconstructed_cvidc_video_tail.c)

add_library(ntdos64-opennt-host-stack-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/softpc.new/base/bios/bios.c
    base/mvdm/softpc.new/host/src/sim32.c
    base/mvdm/dos/dem/dem.c
    base/mvdm/dos/dem/demdata.c
    base/mvdm/dos/dem/demdir.c
    base/mvdm/dos/dem/demdisp.c
    base/mvdm/dos/dem/demerror.c
    base/mvdm/dos/dem/demfcb.c
    overlay/base/mvdm/dos/dem/demfile.c
    base/mvdm/dos/dem/demgset.c
    overlay/base/mvdm/dos/dem/demhndl.c
    base/mvdm/dos/dem/demioctl.c
    base/mvdm/dos/dem/demdasd.c
    base/mvdm/dos/dem/demlabel.c
    base/mvdm/dos/dem/demlock.c
    base/mvdm/dos/dem/demmisc.c
    base/mvdm/dos/dem/demmsg.c
    base/mvdm/dos/dem/demsrch.c)

# The original BIOS selector table is kept separately for SM-2. The broader
# host-stack group contains only historical OpenNT owners and approved source
# overlays; the frozen trace-only register bridge is linked explicitly by its
# own fixtures and never enters a historical provider archive.
add_library(ntdos64-opennt-bios-table-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/softpc.new/base/bios/bios.c)

add_library(ntdos64-opennt-nt-bop-objects OBJECT EXCLUDE_FROM_ALL
    overlay/base/mvdm/softpc.new/host/src/nt_bop.c)

# Keep the original Sim32 and DEM owners selectable on their own so an R1
# closure can prove the historical chain without a retired local adapter.
add_library(ntdos64-opennt-sim32-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/softpc.new/host/src/sim32.c)
# This gate is solely for no-SM0 closure diagnostics. Unlike the SM0 fixture
# gate below, it preserves Sim32's historical host_sas_init/host_sas_term names.
add_library(ntdos64-opennt-sim32-owner-closure-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/softpc.new/host/src/sim32.c)
# The original host/src sources file includes sim32.c without a SIM32 define.
# This inactive role is the companion of DPMI32's original i386 mapper in the
# full historical link profile. Keep it separate from the SIM32-enabled R1
# trace object above.
add_library(ntdos64-opennt-sim32-ntvdm-profile-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/softpc.new/host/src/sim32.c)

# The i386 half of a raw shared-backing aperture check. It does not link SAS,
# CCPU, BIOS, BOP, DEM, or any historical runtime owner.
add_executable(ntdos64-opennt-shared-aperture-v1-helper EXCLUDE_FROM_ALL
    local/trace/fixtures/shared_aperture_v1_helper.c)
target_include_directories(ntdos64-opennt-shared-aperture-v1-helper PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../reconstruction/transport)

# This helper changes only the host allocation seam used by original SAS.
# It is intentionally separate from the raw mapping helper and has no CPU,
# BIOS, BOP, DEM, or DOS caller.
add_executable(ntdos64-opennt-shared-aperture-sas-v1-helper EXCLUDE_FROM_ALL
    local/archived/legacy-adapters/ccpu_monitor_sm0_foundation.c
    local/compat/host/shared_aperture_sas_host_v1.c
    local/trace/fixtures/shared_aperture_sas_v1_helper.c
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-effective-addr-realmode-fixture-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-register-bridge-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-ntstubs-objects>)
target_include_directories(ntdos64-opennt-shared-aperture-sas-v1-helper PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/local/compat/host
    ${CMAKE_CURRENT_SOURCE_DIR}/../reconstruction/transport)
target_compile_definitions(ntdos64-opennt-shared-aperture-sas-v1-helper PRIVATE
    CCPU_SM0_USE_HISTORICAL_SAS
    CCPU_SM2_USE_EFFECTIVE_ADDR_COMPLETION)
target_link_libraries(ntdos64-opennt-shared-aperture-sas-v1-helper PRIVATE
    ntdos64-opennt-ccpu-archive
    ntdos64-opennt-ccpu-platform-provider-archive
    ntdll.lib
    msvcrt.lib ucrt.lib legacy_stdio_definitions.lib)
# Original software-CCPU SAS owner for the non-CPU_40 x86 profile.  It is
# kept separate from the RISC/SIM32+nt_mem diagnostic pair until a bounded
# reset trace proves the exact host-SAS lifecycle contract.
add_library(ntdos64-opennt-ccpu-host-sas-x86-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/softpc.new/host/src/sim32.c)
add_library(ntdos64-opennt-dem-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/dos/dem/dem.c
    base/mvdm/dos/dem/demdata.c
    base/mvdm/dos/dem/demdir.c
    base/mvdm/dos/dem/demdisp.c
    base/mvdm/dos/dem/demerror.c
    base/mvdm/dos/dem/demfcb.c
    overlay/base/mvdm/dos/dem/demfile.c
    base/mvdm/dos/dem/demgset.c
    overlay/base/mvdm/dos/dem/demhndl.c
    base/mvdm/dos/dem/demioctl.c
    base/mvdm/dos/dem/demdasd.c
    base/mvdm/dos/dem/demlabel.c
    base/mvdm/dos/dem/demlock.c
    base/mvdm/dos/dem/demmisc.c
    base/mvdm/dos/dem/demmsg.c
    base/mvdm/dos/dem/demsrch.c)

# The normal x86 DEM build assumes V86 maps guest physical addresses at their
# host-linear values. This parallel, default-disabled profile preserves DEM
# sources but translates GetVDMAddr through the CCPU SAS owner for S5 only.
add_library(ntdos64-opennt-dem-ccpu-sas-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/dos/dem/dem.c
    base/mvdm/dos/dem/demdata.c
    base/mvdm/dos/dem/demdir.c
    base/mvdm/dos/dem/demdisp.c
    base/mvdm/dos/dem/demerror.c
    base/mvdm/dos/dem/demfcb.c
    overlay/base/mvdm/dos/dem/demfile.c
    base/mvdm/dos/dem/demgset.c
    overlay/base/mvdm/dos/dem/demhndl.c
    base/mvdm/dos/dem/demioctl.c
    base/mvdm/dos/dem/demdasd.c
    base/mvdm/dos/dem/demlabel.c
    base/mvdm/dos/dem/demlock.c
    base/mvdm/dos/dem/demmisc.c
    base/mvdm/dos/dem/demmsg.c
    base/mvdm/dos/dem/demsrch.c)

# R1.3 measures one original DEM caller. Its desired call is only
# demClientErrorEx's non-hard-error register result path; it does not install
# a replacement DEM dispatcher or link the full DOS runtime.
add_library(ntdos64-opennt-demerror-register-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/dos/dem/demerror.c)

add_library(ntdos64-opennt-demlock-register-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/dos/dem/demlock.c)

# The original CCPU `sources` target is a static library. Preserve that build
# shape; this archive is the historical link-selection boundary.
add_library(ntdos64-opennt-ccpu-archive STATIC EXCLUDE_FROM_ALL
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-fpu-link-objects>)

# ntstubs.c is the original CCPU-to-generic SAS/CPU forwarding layer. Keep it
# separate from the software-FPU archive: the full no-FPU profile needs an
# unrecovered generated SAS-vector header and is not a valid closure shortcut.
add_library(ntdos64-opennt-ccpu-ntstubs-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/softpc.new/base/ccpu386/ntstubs.c)

# Source-missing CCPU vector reconstructed from original selector/descriptor
# callers. It stays default-disabled and has no BOP, BIOS, DEM, device or
# host-policy behavior.
add_library(ntdos64-opennt-ccpu-effective-addr-reconstruction OBJECT EXCLUDE_FROM_ALL
    overlay/base/mvdm/softpc.new/base/ccpu386/c_effective_addr.c)
add_library(ntdos64-opennt-ccpu-effective-addr-realmode-fixture-objects OBJECT EXCLUDE_FROM_ALL
    overlay/base/mvdm/softpc.new/base/ccpu386/c_effective_addr.c)
target_compile_definitions(ntdos64-opennt-ccpu-effective-addr-realmode-fixture-objects PRIVATE
    NTDOS64_CCPU_EFFECTIVE_ADDR_REALMODE_ONLY)


# SH-10 uses the complete CCPU compilation closure through its historical
# archive boundary, never a hand-selected executable subset. The fixture calls
# c_cpu_init plus register accessors only; it does not enter c_cpu_simulate,
# V86, BOP, BIOS, or DEM.
add_executable(ntdos64-opennt-ccpu-register-bridge-fixture EXCLUDE_FROM_ALL
    local/archived/legacy-adapters/ccpu_register_abi_bridge.c
    local/archived/legacy-adapters/ccpu_host_foundation.c
    local/trace/fixtures/ccpu_register_abi_bridge_fixture.c)
target_link_libraries(ntdos64-opennt-ccpu-register-bridge-fixture PRIVATE
    ntdos64-opennt-ccpu-archive
    ntdos64-opennt-ccpu-platform-provider-archive)

# R1.3 uses the original nt_aorc.c forwarding implementation. It is distinct
# from the frozen local register adapter and exercises no DOS/runtime path.
add_executable(ntdos64-opennt-original-register-bridge-fixture EXCLUDE_FROM_ALL
    local/trace/fixtures/original_register_bridge_fixture.c
    local/archived/legacy-adapters/ccpu_monitor_sm0_foundation.c
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-effective-addr-reconstruction>
    $<TARGET_OBJECTS:ntdos64-opennt-register-bridge-objects>)
target_link_libraries(ntdos64-opennt-original-register-bridge-fixture PRIVATE
    ntdos64-opennt-ccpu-archive
    ntdos64-opennt-ccpu-platform-provider-archive
    msvcrt.lib ucrt.lib legacy_stdio_definitions.lib)

add_executable(ntdos64-opennt-original-demerror-register-fixture EXCLUDE_FROM_ALL
    local/trace/fixtures/original_demerror_register_fixture.c
    local/archived/legacy-adapters/ccpu_monitor_sm0_foundation.c
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-effective-addr-reconstruction>
    $<TARGET_OBJECTS:ntdos64-opennt-register-bridge-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-demerror-register-objects>)
target_link_libraries(ntdos64-opennt-original-demerror-register-fixture PRIVATE
    ntdos64-opennt-ccpu-archive
    ntdos64-opennt-ccpu-platform-provider-archive
    msvcrt.lib ucrt.lib legacy_stdio_definitions.lib)

add_executable(ntdos64-opennt-original-demlock-register-fixture EXCLUDE_FROM_ALL
    local/trace/fixtures/original_demlock_register_fixture.c
    local/archived/legacy-adapters/ccpu_monitor_sm0_foundation.c
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-effective-addr-reconstruction>
    $<TARGET_OBJECTS:ntdos64-opennt-register-bridge-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-demlock-register-objects>)
target_link_libraries(ntdos64-opennt-original-demlock-register-fixture PRIVATE
    ntdos64-opennt-ccpu-archive
    ntdos64-opennt-ccpu-platform-provider-archive
    msvcrt.lib ucrt.lib legacy_stdio_definitions.lib)

# SM-0 is a bounded software-monitor proof only: CCPU executes C4 C4 FE and
# exits through its own c_cpu_unsimulate path. It has no BIOS, nt_bop, DEM,
# DOS, device, or normal-runner dependency.
add_executable(ntdos64-opennt-ccpu-software-monitor-sm0-fixture EXCLUDE_FROM_ALL
    local/archived/legacy-adapters/ccpu_monitor_sm0_foundation.c
    local/trace/fixtures/ccpu_monitor_sm0_fixture.c
    $<TARGET_OBJECTS:ntdos64-opennt-register-bridge-objects>)
target_link_libraries(ntdos64-opennt-ccpu-software-monitor-sm0-fixture PRIVATE
    ntdos64-opennt-ccpu-archive
    ntdos64-opennt-ccpu-platform-provider-archive
    msvcrt.lib
    ucrt.lib
    legacy_stdio_definitions.lib)

# SM-1 executes bounded guest instructions through original CCPU SAS to prove
# RAM access, ROM write rejection, and A20 wrapping. It remains BIOS/DEM/DOS
# free and does not define a modern guest-memory or fault contract.
add_executable(ntdos64-opennt-ccpu-software-monitor-sm1-fixture EXCLUDE_FROM_ALL
    local/archived/legacy-adapters/ccpu_monitor_sm0_foundation.c
    local/trace/fixtures/ccpu_monitor_sm1_fixture.c
    $<TARGET_OBJECTS:ntdos64-opennt-register-bridge-objects>)
target_link_libraries(ntdos64-opennt-ccpu-software-monitor-sm1-fixture PRIVATE
    ntdos64-opennt-ccpu-archive
    ntdos64-opennt-ccpu-platform-provider-archive
    msvcrt.lib
    ucrt.lib
    legacy_stdio_definitions.lib)

# SM-2 admits the original BIOS[] table as a whole. The closure diagnostic is
# deliberate: no reduced local selector table may substitute for bios.c.
add_executable(ntdos64-opennt-ccpu-software-monitor-sm2-fixture EXCLUDE_FROM_ALL
    local/archived/legacy-adapters/ccpu_monitor_sm0_foundation.c
    # These are original owners required by the full BIOS[ ] table itself.
    # Their absence previously made this trace target misclassify composition
    # omissions as missing host behavior.
    local/compat/host/historical_vdm_session_context_v1.c
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-effective-addr-reconstruction>
    local/trace/fixtures/ccpu_monitor_sm2_fixture.c
    $<TARGET_OBJECTS:ntdos64-opennt-register-bridge-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-ntstubs-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-host-eoi-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-host-timer-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-event-host-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-bios-table-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-bios-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-xms-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-suballoc-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-umb-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-system-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-support-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-keymouse-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-disks-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-comms-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-video-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-nt-bop-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-sim32-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-dem-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-nt4-emm-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-x86-emm-host-objects>
    # Preserve the original owner of the SoftPC version/copyright strings.
    # This remains a closure diagnostic, not an admitted modern lifecycle.
    $<TARGET_OBJECTS:ntdos64-opennt-host-idle-lifecycle-objects>)
target_compile_definitions(ntdos64-opennt-ccpu-software-monitor-sm2-fixture PRIVATE
    CCPU_SM2_USE_HISTORICAL_BIOS
    CCPU_SM2_USE_EFFECTIVE_ADDR_COMPLETION
    CCPU_SM2_USE_HISTORICAL_EOI
    CCPU_SM2_USE_HISTORICAL_TIMER)
target_link_libraries(ntdos64-opennt-ccpu-software-monitor-sm2-fixture PRIVATE
    ntdos64-opennt-ccpu-archive
    # C_Video is owned by the independently generated historical C-VID source
    # closure. This is not a local Video=C_Video binding.
    ntdos64-opennt-cvidc-archive
    ntdos64-opennt-ccpu-platform-provider-archive
    ntdll.lib
    msvcrt.lib ucrt.lib legacy_stdio_definitions.lib)

# R1 link-only diagnostic. It retains only original chain owners: CCPU,
# BIOS[], nt_bop, nt_aorc, Sim32 and DEM. The fixture observes addresses and
# must not call BIOS, BOP, DEM, or CCPU execution; no self-authored BOP/SVC or
# register adapter is linked into this target.
add_executable(ntdos64-opennt-original-chain-closure-fixture EXCLUDE_FROM_ALL
    local/trace/fixtures/original_chain_closure_fixture.c
    # This direct CCPU forwarding completion is separately fixture-verified.
    # It resolves only CCPU effective-address translation; it cannot dispatch
    # a BOP/SVC, alter DEM state, or provide a device response.
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-effective-addr-reconstruction>
    $<TARGET_OBJECTS:ntdos64-opennt-register-bridge-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-ntstubs-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-host-eoi-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-host-timer-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-copy-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-sound-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-sound-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-event-host-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-bios-table-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-bios-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-sim32-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-nt-bop-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-dem-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-xms-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-suballoc-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-umb-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-system-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-support-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-keymouse-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-disks-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-comms-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-video-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-nt4-emm-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-x86-emm-host-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-idle-lifecycle-objects>)
target_compile_definitions(ntdos64-opennt-original-chain-closure-fixture PRIVATE
    CCPU_SM2_USE_HISTORICAL_BIOS
    CCPU_SM2_USE_EFFECTIVE_ADDR_COMPLETION
    CCPU_SM2_USE_HISTORICAL_EOI
    CCPU_SM2_USE_HISTORICAL_TIMER)
target_link_libraries(ntdos64-opennt-original-chain-closure-fixture PRIVATE
    ntdos64-opennt-ccpu-archive
    ntdos64-opennt-ccpu-platform-provider-archive
    # Historical sources call the NT native import surface directly. This is
    # an import library only, not a replacement VDM API or host shim.
    ntdll.lib
    msvcrt.lib ucrt.lib legacy_stdio_definitions.lib)

# R1.2 executes one bounded original selector-50 path.  The historical BIOS
# table is retained as a whole; /FORCE:UNRESOLVED only permits uncalled table
# entries to remain a trace-time diagnostic.  It never establishes a complete
# historical runtime closure or a shim admission.  The fixture itself reaches
# only CCPU -> BIOS[50h] -> MS_bop_0 -> DemDispatch -> demQueryDate.
add_executable(ntdos64-opennt-original-selector50-trace-fixture EXCLUDE_FROM_ALL
    local/archived/legacy-adapters/ccpu_monitor_sm0_foundation.c
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-effective-addr-reconstruction>
    local/trace/fixtures/original_selector50_trace_fixture.c
    $<TARGET_OBJECTS:ntdos64-opennt-register-bridge-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-ntstubs-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-sound-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-event-host-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-bios-table-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-bios-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-sim32-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-nt-bop-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-dem-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-xms-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-suballoc-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-umb-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-system-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-support-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-keymouse-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-disks-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-comms-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-video-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-nt4-emm-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-x86-emm-host-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-idle-lifecycle-objects>)
target_compile_definitions(ntdos64-opennt-original-selector50-trace-fixture PRIVATE
    CCPU_SM2_USE_HISTORICAL_BIOS
    CCPU_SM2_USE_EFFECTIVE_ADDR_COMPLETION)
target_link_libraries(ntdos64-opennt-original-selector50-trace-fixture PRIVATE
    ntdos64-opennt-ccpu-archive
    ntdos64-opennt-ccpu-platform-provider-archive
    ntdll.lib
    msvcrt.lib ucrt.lib legacy_stdio_definitions.lib)

# R1.4 forces MS_bop_0's original IDLE_disk branch with an invalid service.
# It is bounded instrumentation, not a runtime error policy or an IRQ shim.
add_executable(ntdos64-opennt-original-selector50-idle-trace-fixture EXCLUDE_FROM_ALL
    local/archived/legacy-adapters/ccpu_monitor_sm0_foundation.c
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-effective-addr-reconstruction>
    local/trace/fixtures/original_selector50_idle_trace_fixture.c
    $<TARGET_OBJECTS:ntdos64-opennt-register-bridge-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-ntstubs-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-sound-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-event-host-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-bios-table-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-bios-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-sim32-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-nt-bop-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-dem-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-xms-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-suballoc-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-umb-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-system-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-support-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-keymouse-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-disks-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-comms-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-video-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-nt4-emm-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-x86-emm-host-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-idle-lifecycle-objects>)
target_compile_definitions(ntdos64-opennt-original-selector50-idle-trace-fixture PRIVATE
    CCPU_SM2_USE_HISTORICAL_BIOS
    CCPU_SM2_USE_EFFECTIVE_ADDR_COMPLETION)
target_link_libraries(ntdos64-opennt-original-selector50-idle-trace-fixture PRIVATE
    ntdos64-opennt-ccpu-archive
    ntdos64-opennt-ccpu-platform-provider-archive
    ntdll.lib
    msvcrt.lib ucrt.lib legacy_stdio_definitions.lib)

# R1.2 split diagnostic: original DemDispatch plus its date service, but no
# guest instruction, BIOS selector, or BOP.  It identifies whether a bounded
# selector-50 trace is blocked before or inside the original DEM dispatcher.
add_executable(ntdos64-opennt-original-demdispatch-date-fixture EXCLUDE_FROM_ALL
    local/archived/legacy-adapters/ccpu_monitor_sm0_foundation.c
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-effective-addr-reconstruction>
    local/trace/fixtures/original_demdispatch_date_fixture.c
    $<TARGET_OBJECTS:ntdos64-opennt-register-bridge-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-dem-objects>)
target_compile_definitions(ntdos64-opennt-original-demdispatch-date-fixture PRIVATE
    CCPU_SM2_USE_EFFECTIVE_ADDR_COMPLETION)
target_link_libraries(ntdos64-opennt-original-demdispatch-date-fixture PRIVATE
    ntdos64-opennt-ccpu-archive
    ntdos64-opennt-ccpu-platform-provider-archive
    ntdll.lib
    msvcrt.lib ucrt.lib legacy_stdio_definitions.lib)

# R1.2 split diagnostic: original MS_bop_0's service-byte read and post-DEM
# IP increment. It has no guest instruction or BIOS-table dispatch and cannot
# substitute for the selector-50 trace target.
add_executable(ntdos64-opennt-original-msbop0-date-fixture EXCLUDE_FROM_ALL
    local/archived/legacy-adapters/ccpu_monitor_sm0_foundation.c
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-effective-addr-reconstruction>
    local/trace/fixtures/original_msbop0_date_fixture.c
    $<TARGET_OBJECTS:ntdos64-opennt-register-bridge-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-sim32-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-nt-bop-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-dem-objects>)
target_compile_definitions(ntdos64-opennt-original-msbop0-date-fixture PRIVATE
    CCPU_SM2_USE_EFFECTIVE_ADDR_COMPLETION)
target_link_libraries(ntdos64-opennt-original-msbop0-date-fixture PRIVATE
    ntdos64-opennt-ccpu-archive
    ntdos64-opennt-ccpu-platform-provider-archive
    ntdll.lib
    msvcrt.lib ucrt.lib legacy_stdio_definitions.lib)

# S1 directly invokes the original BIOS[50h] entry. It proves the historical
# host-side table -> MS_bop_0 -> DemDispatch boundary without invoking CCPU's
# instruction decoder or adding any local BIOS/BOP/DEM behavior.
add_executable(ntdos64-opennt-original-bios50-date-fixture EXCLUDE_FROM_ALL
    local/archived/legacy-adapters/ccpu_monitor_sm0_foundation.c
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-effective-addr-reconstruction>
    local/trace/fixtures/original_bios50_date_fixture.c
    $<TARGET_OBJECTS:ntdos64-opennt-register-bridge-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-ntstubs-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-sound-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-event-host-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-bios-table-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-bios-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-sim32-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-nt-bop-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-dem-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-xms-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-suballoc-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-umb-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-system-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-support-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-keymouse-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-disks-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-comms-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-video-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-nt4-emm-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-x86-emm-host-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-idle-lifecycle-objects>)
target_compile_definitions(ntdos64-opennt-original-bios50-date-fixture PRIVATE
    CCPU_SM2_USE_HISTORICAL_BIOS
    CCPU_SM2_USE_EFFECTIVE_ADDR_COMPLETION)
target_link_libraries(ntdos64-opennt-original-bios50-date-fixture PRIVATE
    ntdos64-opennt-ccpu-archive
    ntdos64-opennt-ccpu-platform-provider-archive
    ntdll.lib
    msvcrt.lib ucrt.lib legacy_stdio_definitions.lib)

# S2 v1 keeps the same proven historical table closure, but reaches it only
# through a fixed-width record adapter. Its allowed selector/service pair is
# deliberately limited to the S1 date trace; it is not a DOS dispatcher.
add_executable(ntdos64-opennt-historical-bios-bridge-v1-fixture EXCLUDE_FROM_ALL
    local/archived/legacy-adapters/ccpu_monitor_sm0_foundation.c
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-effective-addr-reconstruction>
    local/compat/host/historical_bios_bridge_v1.c
    local/trace/fixtures/historical_bios_bridge_v1_fixture.c
    $<TARGET_OBJECTS:ntdos64-opennt-register-bridge-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-ntstubs-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-sound-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-event-host-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-bios-table-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-bios-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-sim32-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-nt-bop-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-dem-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-xms-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-suballoc-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-umb-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-system-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-support-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-keymouse-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-disks-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-comms-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-video-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-nt4-emm-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-x86-emm-host-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-idle-lifecycle-objects>)
target_include_directories(ntdos64-opennt-historical-bios-bridge-v1-fixture PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/local/compat/host)
target_compile_definitions(ntdos64-opennt-historical-bios-bridge-v1-fixture PRIVATE
    CCPU_SM2_USE_HISTORICAL_BIOS
    CCPU_SM2_USE_EFFECTIVE_ADDR_COMPLETION)
target_link_libraries(ntdos64-opennt-historical-bios-bridge-v1-fixture PRIVATE
    ntdos64-opennt-ccpu-archive
    ntdos64-opennt-ccpu-platform-provider-archive
    ntdll.lib
    msvcrt.lib ucrt.lib legacy_stdio_definitions.lib)

# This default-disabled fixture reaches the original SVC_DEMLOADDOS path. The
# local profile supplies only the BYOB directory after original DemInit; the
# original BIOS/MS_bop_0/DemDispatch/demLoadDos owners perform the load.
add_executable(ntdos64-opennt-original-demload-dos-byob-fixture EXCLUDE_FROM_ALL
    local/archived/legacy-adapters/ccpu_monitor_sm0_foundation.c
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-effective-addr-reconstruction>
    local/compat/host/historical_bios_bridge_v1.c
    local/compat/host/historical_dem_byob_profile_v1.c
    local/compat/host/historical_vdm_session_context_v1.c
    local/compat/host/oem_facade_v1/oem_facade_v1_f1.c
    local/trace/fixtures/original_demload_dos_byob_fixture.c
    $<TARGET_OBJECTS:ntdos64-opennt-register-bridge-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-ntstubs-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-sound-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-event-host-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-bios-table-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-bios-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-sim32-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-nt-bop-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-dem-ccpu-sas-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-xms-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-suballoc-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-umb-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-system-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-support-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-keymouse-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-disks-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-comms-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-video-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-nt4-emm-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-x86-emm-host-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-idle-lifecycle-objects>)
target_include_directories(ntdos64-opennt-original-demload-dos-byob-fixture PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/local/compat/host
    ${CMAKE_CURRENT_SOURCE_DIR}/local/compat/host/oem_facade_v1)
target_compile_definitions(ntdos64-opennt-original-demload-dos-byob-fixture PRIVATE
    CCPU_SM2_USE_HISTORICAL_BIOS
    CCPU_SM2_USE_EFFECTIVE_ADDR_COMPLETION)
target_link_libraries(ntdos64-opennt-original-demload-dos-byob-fixture PRIVATE
    ntdos64-opennt-ccpu-archive
    ntdos64-opennt-ccpu-platform-provider-archive
    ntdll.lib
    msvcrt.lib ucrt.lib legacy_stdio_definitions.lib)

# Diagnostic companion: it invokes the original loader directly only to
# distinguish loader closure from the historical MS_bop_0 post-load idle call.
# It is trace-only and is not an alternate runtime dispatch path.
add_executable(ntdos64-opennt-original-demload-dos-direct-fixture EXCLUDE_FROM_ALL
    local/archived/legacy-adapters/ccpu_monitor_sm0_foundation.c
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-effective-addr-reconstruction>
    local/compat/host/historical_bios_bridge_v1.c
    local/compat/host/historical_dem_byob_profile_v1.c
    local/compat/host/historical_vdm_session_context_v1.c
    local/compat/host/oem_facade_v1/oem_facade_v1_f1.c
    local/trace/fixtures/original_demload_dos_direct_fixture.c
    $<TARGET_OBJECTS:ntdos64-opennt-register-bridge-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-ntstubs-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-sound-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-event-host-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-bios-table-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-bios-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-sim32-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-nt-bop-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-dem-ccpu-sas-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-xms-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-suballoc-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-umb-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-system-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-support-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-keymouse-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-disks-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-comms-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-video-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-nt4-emm-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-x86-emm-host-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-idle-lifecycle-objects>)
target_include_directories(ntdos64-opennt-original-demload-dos-direct-fixture PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/local/compat/host
    ${CMAKE_CURRENT_SOURCE_DIR}/local/compat/host/oem_facade_v1)
target_compile_definitions(ntdos64-opennt-original-demload-dos-direct-fixture PRIVATE
    CCPU_SM2_USE_HISTORICAL_BIOS
    CCPU_SM2_USE_EFFECTIVE_ADDR_COMPLETION)
target_link_libraries(ntdos64-opennt-original-demload-dos-direct-fixture PRIVATE
    ntdos64-opennt-ccpu-archive
    ntdos64-opennt-ccpu-platform-provider-archive
    ntdll.lib
    msvcrt.lib ucrt.lib legacy_stdio_definitions.lib)

# S5 caller-first drive-discovery trace. This follows the actual post-load
# service route through the original BIOS/BOP/DEM owners and links the original
# OEM conversion owner. It is intentionally a default-disabled trace: no
# drive-policy facade, DOS result replacement, or runtime admission is implied.
add_executable(ntdos64-opennt-original-demgetdrives-byob-fixture EXCLUDE_FROM_ALL
    local/archived/legacy-adapters/ccpu_monitor_sm0_foundation.c
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-effective-addr-reconstruction>
    local/compat/host/historical_bios_bridge_v1.c
    local/compat/host/historical_vdm_session_context_v1.c
    local/compat/host/historical_nt_process_heap_v1.c
    local/trace/fixtures/original_demgetdrives_byob_fixture.c
    $<TARGET_OBJECTS:ntdos64-opennt-register-bridge-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-ntstubs-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-sound-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-event-host-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-bios-table-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-bios-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-sim32-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-oemuni-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-nt-bop-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-dem-ccpu-sas-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-xms-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-suballoc-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-umb-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-system-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-support-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-keymouse-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-disks-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-comms-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-video-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-nt4-emm-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-x86-emm-host-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-idle-lifecycle-objects>)
target_include_directories(ntdos64-opennt-original-demgetdrives-byob-fixture PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/local/compat/host)
target_compile_definitions(ntdos64-opennt-original-demgetdrives-byob-fixture PRIVATE
    CCPU_SM2_USE_HISTORICAL_BIOS
    CCPU_SM2_USE_EFFECTIVE_ADDR_COMPLETION)
target_link_libraries(ntdos64-opennt-original-demgetdrives-byob-fixture PRIVATE
    ntdos64-opennt-ccpu-archive
    ntdos64-opennt-ccpu-platform-provider-archive
    ntdll.lib
    msvcrt.lib ucrt.lib legacy_stdio_definitions.lib)

# S5 caller-first DPB trace. The two services share the original DEM session:
# 0Fh populates PhysicalDriveTypes and 46h alone writes the guest DPB chain.
add_executable(ntdos64-opennt-original-demgetdpblist-trace-fixture EXCLUDE_FROM_ALL
    local/archived/legacy-adapters/ccpu_monitor_sm0_foundation.c
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-effective-addr-reconstruction>
    local/compat/host/historical_bios_bridge_v1.c
    local/compat/host/historical_vdm_session_context_v1.c
    local/compat/host/historical_nt_process_heap_v1.c
    local/trace/fixtures/original_demgetdpblist_trace_fixture.c
    $<TARGET_OBJECTS:ntdos64-opennt-register-bridge-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-ntstubs-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-sound-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-event-host-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-bios-table-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-bios-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-sim32-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-oemuni-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-nt-bop-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-dem-ccpu-sas-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-xms-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-suballoc-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-umb-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-system-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-support-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-keymouse-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-disks-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-comms-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-video-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-nt4-emm-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-x86-emm-host-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-idle-lifecycle-objects>)
target_include_directories(ntdos64-opennt-original-demgetdpblist-trace-fixture PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/local/compat/host
    ${CMAKE_CURRENT_SOURCE_DIR}/base/mvdm/dos/dem)
target_compile_definitions(ntdos64-opennt-original-demgetdpblist-trace-fixture PRIVATE
    CCPU_SM2_USE_HISTORICAL_BIOS
    CCPU_SM2_USE_EFFECTIVE_ADDR_COMPLETION)
target_link_libraries(ntdos64-opennt-original-demgetdpblist-trace-fixture PRIVATE
    ntdos64-opennt-ccpu-archive
    ntdos64-opennt-ccpu-platform-provider-archive
    ntdll.lib
    msvcrt.lib ucrt.lib legacy_stdio_definitions.lib)

# S5 caller-first guest-address trace for the immediately following DosInit
# service. It only proves that the original DEM owner retains NTDOS's pointers
# through the original CCPU/SAS route; it does not provide a DOS data layout.
add_executable(ntdos64-opennt-original-demsetdta-trace-fixture EXCLUDE_FROM_ALL
    local/archived/legacy-adapters/ccpu_monitor_sm0_foundation.c
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-effective-addr-reconstruction>
    local/compat/host/historical_bios_bridge_v1.c
    local/compat/host/historical_vdm_session_context_v1.c
    local/trace/fixtures/original_demsetdta_trace_fixture.c
    $<TARGET_OBJECTS:ntdos64-opennt-register-bridge-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-ntstubs-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-sound-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-event-host-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-bios-table-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-bios-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-sim32-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-oemuni-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-nt-bop-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-dem-ccpu-sas-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-xms-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-suballoc-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-umb-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-system-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-support-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-keymouse-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-disks-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-comms-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-video-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-nt4-emm-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-x86-emm-host-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-idle-lifecycle-objects>)
target_include_directories(ntdos64-opennt-original-demsetdta-trace-fixture PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/local/compat/host)
target_compile_definitions(ntdos64-opennt-original-demsetdta-trace-fixture PRIVATE
    CCPU_SM2_USE_HISTORICAL_BIOS
    CCPU_SM2_USE_EFFECTIVE_ADDR_COMPLETION)
target_link_libraries(ntdos64-opennt-original-demsetdta-trace-fixture PRIVATE
    ntdos64-opennt-ccpu-archive
    ntdos64-opennt-ccpu-platform-provider-archive
    ntdll.lib
    msvcrt.lib ucrt.lib legacy_stdio_definitions.lib)

# S5 caller-first guest-address trace for DosInit's original VHE/device-chain
# registration. It retains the original service and observes no error handling.
add_executable(ntdos64-opennt-original-demsetharderror-trace-fixture EXCLUDE_FROM_ALL
    local/archived/legacy-adapters/ccpu_monitor_sm0_foundation.c
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-effective-addr-reconstruction>
    local/compat/host/historical_bios_bridge_v1.c
    local/compat/host/historical_vdm_session_context_v1.c
    local/trace/fixtures/original_demsetharderror_trace_fixture.c
    $<TARGET_OBJECTS:ntdos64-opennt-register-bridge-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-ntstubs-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-sound-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-event-host-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-bios-table-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-bios-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-sim32-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-oemuni-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-nt-bop-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-dem-ccpu-sas-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-xms-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-suballoc-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-umb-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-system-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-support-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-keymouse-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-disks-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-comms-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-video-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-nt4-emm-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-x86-emm-host-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-idle-lifecycle-objects>)
target_include_directories(ntdos64-opennt-original-demsetharderror-trace-fixture PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/local/compat/host)
target_compile_definitions(ntdos64-opennt-original-demsetharderror-trace-fixture PRIVATE
    CCPU_SM2_USE_HISTORICAL_BIOS
    CCPU_SM2_USE_EFFECTIVE_ADDR_COMPLETION)
target_link_libraries(ntdos64-opennt-original-demsetharderror-trace-fixture PRIVATE
    ntdos64-opennt-ccpu-archive
    ntdos64-opennt-ccpu-platform-provider-archive
    ntdll.lib
    msvcrt.lib ucrt.lib legacy_stdio_definitions.lib)

# S5 COMMAND boundary trace. The complete historical command object group is
# retained so CmdDispatch is never reduced to service 05 by local code.
add_executable(ntdos64-opennt-original-cmdsetinfo-trace-fixture EXCLUDE_FROM_ALL
    local/archived/legacy-adapters/ccpu_monitor_sm0_foundation.c
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-effective-addr-reconstruction>
    local/compat/host/historical_bios_bridge_v1.c
    local/compat/host/historical_vdm_session_context_v1.c
    local/trace/fixtures/original_cmdsetinfo_trace_fixture.c
    $<TARGET_OBJECTS:ntdos64-opennt-register-bridge-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-ntstubs-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-sound-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-event-host-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-bios-table-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-bios-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-sim32-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-oemuni-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-nt-bop-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-dem-ccpu-sas-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-command-objects>
    # nt_fdisk.c is the original owner of pFDAccess. It supplies storage only
    # on the admitted cmdSetInfo path; none of its disk operations are called.
    $<TARGET_OBJECTS:ntdos64-opennt-disk-host-backend-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-xms-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-suballoc-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-umb-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-system-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-support-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-keymouse-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-disks-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-comms-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-video-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-nt4-emm-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-x86-emm-host-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-idle-lifecycle-objects>)
target_include_directories(ntdos64-opennt-original-cmdsetinfo-trace-fixture PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/local/compat/host
    ${CMAKE_CURRENT_SOURCE_DIR}/base/mvdm/dos/command
    ${CMAKE_CURRENT_SOURCE_DIR}/base/mvdm/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc)
target_compile_definitions(ntdos64-opennt-original-cmdsetinfo-trace-fixture PRIVATE
    CCPU_SM2_USE_HISTORICAL_BIOS
    CCPU_SM2_USE_EFFECTIVE_ADDR_COMPLETION)
target_link_libraries(ntdos64-opennt-original-cmdsetinfo-trace-fixture PRIVATE
    ntdos64-opennt-ccpu-archive
    ntdos64-opennt-ccpu-platform-provider-archive
    ntdll.lib
    msvcrt.lib ucrt.lib legacy_stdio_definitions.lib)

# S4 cross-check: the independently reconstructed monitor mechanics use only
# the S2 record transport to reach the original BIOS[50h] date trace. The
# monitor itself has no historical BIOS/BOP/DEM implementation.
add_executable(ntdos64-opennt-reconstructed-monitor-s4-bridge-fixture EXCLUDE_FROM_ALL
    local/archived/legacy-adapters/ccpu_monitor_sm0_foundation.c
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-effective-addr-reconstruction>
    ${CMAKE_CURRENT_SOURCE_DIR}/../reconstruction/monitor/reconstructed_monitor.c
    local/compat/host/historical_bios_bridge_v1.c
    local/trace/fixtures/historical_monitor_bridge_fixture.c
    $<TARGET_OBJECTS:ntdos64-opennt-register-bridge-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-ntstubs-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-sound-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-event-host-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-bios-table-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-bios-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-sim32-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-nt-bop-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-dem-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-xms-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-suballoc-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-umb-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-system-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-support-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-keymouse-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-disks-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-comms-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-video-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-nt4-emm-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-x86-emm-host-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-idle-lifecycle-objects>)
target_include_directories(ntdos64-opennt-reconstructed-monitor-s4-bridge-fixture PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/local/compat/host
    ${CMAKE_CURRENT_SOURCE_DIR}/../reconstruction/monitor)
target_compile_definitions(ntdos64-opennt-reconstructed-monitor-s4-bridge-fixture PRIVATE
    CCPU_SM2_USE_HISTORICAL_BIOS
    CCPU_SM2_USE_EFFECTIVE_ADDR_COMPLETION)
target_link_libraries(ntdos64-opennt-reconstructed-monitor-s4-bridge-fixture PRIVATE
    ntdos64-opennt-ccpu-archive
    ntdos64-opennt-ccpu-platform-provider-archive
    ntdll.lib
    msvcrt.lib ucrt.lib legacy_stdio_definitions.lib)

# S5 v2 record layout is compiled independently with the historical i386
# toolchain. It is a wire-format check only: no OpenNT object or runtime entry
# is linked here.
add_executable(ntdos64-opennt-historical-transport-v2-layout-fixture EXCLUDE_FROM_ALL
    ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/historical_transport_v2_layout_test.c)
target_include_directories(ntdos64-opennt-historical-transport-v2-layout-fixture PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../reconstruction/transport)
target_compile_features(ntdos64-opennt-historical-transport-v2-layout-fixture PRIVATE c_std_11)

# S5 T-S5.1 helper: one fixed-width pipe request reaches only the original
# BIOS[50h] date path. It is a default-disabled trace fixture, not a runtime.
add_executable(ntdos64-opennt-historical-transport-v2-helper EXCLUDE_FROM_ALL
    local/archived/legacy-adapters/ccpu_monitor_sm0_foundation.c
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-effective-addr-reconstruction>
    local/compat/host/historical_bios_bridge_v2.c
    local/trace/fixtures/historical_transport_v2_helper.c
    $<TARGET_OBJECTS:ntdos64-opennt-register-bridge-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-ntstubs-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-sound-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-event-host-objects>
    # Original owners required by the event/static BIOS closure. They remain
    # composition-only; the helper still invokes only BIOS[50h]/service 14h.
    $<TARGET_OBJECTS:ntdos64-opennt-error-host-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-reset-shell-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-host-timer-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-display-input-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-bios-table-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-bios-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-sim32-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-nt-bop-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-dem-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-xms-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-suballoc-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-umb-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-system-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-support-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-keymouse-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-disks-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-comms-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-video-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-nt4-emm-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-x86-emm-host-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-idle-lifecycle-objects>)
target_include_directories(ntdos64-opennt-historical-transport-v2-helper PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/local/compat/host
    ${CMAKE_CURRENT_SOURCE_DIR}/../reconstruction/transport)
target_compile_definitions(ntdos64-opennt-historical-transport-v2-helper PRIVATE
    CCPU_SM2_USE_HISTORICAL_BIOS
    CCPU_SM2_USE_EFFECTIVE_ADDR_COMPLETION)
target_link_libraries(ntdos64-opennt-historical-transport-v2-helper PRIVATE
    ntdos64-opennt-ccpu-archive
    # C_Video is the original generated C-VID vector owner. This is only a
    # source-closure input; it does not bind the unresolved generic Video vector.
    ntdos64-opennt-cvidc-archive
    ntdos64-opennt-ccpu-platform-provider-archive
    ntdll.lib
    msvcrt.lib ucrt.lib legacy_stdio_definitions.lib)
# Preserve the complete unresolved owner set for this clean-link diagnostic.
# This is intentionally not /FORCE:UNRESOLVED and cannot produce a runnable helper.
target_link_options(ntdos64-opennt-historical-transport-v2-helper PRIVATE /errorlimit:0)

# S5 owner-closure contrast: the same bounded selector-50/date trace, but
# using original SAS ownership and no archived SM0 adapter. It is link-only
# evidence, default-disabled, and is never a runtime target.
add_executable(ntdos64-opennt-historical-transport-v2-owner-closure EXCLUDE_FROM_ALL
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-effective-addr-reconstruction>
    local/trace/fixtures/historical_bios_owner_closure_bridge.c
    local/trace/fixtures/historical_transport_v2_helper.c
    $<TARGET_OBJECTS:ntdos64-opennt-register-bridge-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-ntstubs-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-sound-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-event-host-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-error-host-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-reset-shell-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-host-timer-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-display-input-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-bios-table-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-bios-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-sim32-owner-closure-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-host-intel-memory-owner-objects>
    # NTDOS post-load 50h/0Fh reaches demGetDrives, whose original OEM drive
    # conversion owner is file.c. Keep the full original owner in this
    # default-disabled no-SM0 closure; do not substitute a drive shim.
    $<TARGET_OBJECTS:ntdos64-opennt-oemuni-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-nt-bop-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-dem-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-xms-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-suballoc-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-umb-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-system-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-support-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-keymouse-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-disks-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-comms-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-video-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-nt4-emm-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-x86-emm-host-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-idle-lifecycle-objects>)
target_include_directories(ntdos64-opennt-historical-transport-v2-owner-closure PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/local/compat/host
    ${CMAKE_CURRENT_SOURCE_DIR}/../reconstruction/transport)
target_compile_definitions(ntdos64-opennt-historical-transport-v2-owner-closure PRIVATE
    CCPU_SM2_USE_HISTORICAL_BIOS
    CCPU_SM2_USE_EFFECTIVE_ADDR_COMPLETION)
target_link_libraries(ntdos64-opennt-historical-transport-v2-owner-closure PRIVATE
    ntdos64-opennt-ccpu-archive
    ntdos64-opennt-cvidc-archive
    ntdos64-opennt-ccpu-platform-provider-archive
    ntdll.lib
    msvcrt.lib ucrt.lib legacy_stdio_definitions.lib)
target_link_options(ntdos64-opennt-historical-transport-v2-owner-closure PRIVATE /errorlimit:0)

# S5 BOP/DEM contrast for the no-SM0 closure measurement. It deliberately
# invokes original MS_bop_0 directly to isolate BIOS[] static-table width;
# it is not a permitted runtime dispatch alternative.
add_executable(ntdos64-opennt-historical-transport-v2-msbop-closure EXCLUDE_FROM_ALL
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-effective-addr-reconstruction>
    local/trace/fixtures/historical_msbop_owner_closure_bridge.c
    local/trace/fixtures/historical_transport_v2_helper.c
    $<TARGET_OBJECTS:ntdos64-opennt-register-bridge-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-ntstubs-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-sim32-owner-closure-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-host-intel-memory-owner-objects>
    # Original owner for nt_bop.c's historical error path; source closure only.
    $<TARGET_OBJECTS:ntdos64-opennt-error-host-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-nt-bop-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-dem-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-xms-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-suballoc-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-umb-provider-objects>
    # Same dynamically evidenced OEM owner as the full BIOS[] diagnostic.
    $<TARGET_OBJECTS:ntdos64-opennt-oemuni-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-idle-lifecycle-objects>)
target_include_directories(ntdos64-opennt-historical-transport-v2-msbop-closure PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/local/compat/host
    ${CMAKE_CURRENT_SOURCE_DIR}/../reconstruction/transport)
target_compile_definitions(ntdos64-opennt-historical-transport-v2-msbop-closure PRIVATE
    CCPU_SM2_USE_EFFECTIVE_ADDR_COMPLETION)
target_link_libraries(ntdos64-opennt-historical-transport-v2-msbop-closure PRIVATE
    ntdos64-opennt-ccpu-archive
    ntdos64-opennt-cvidc-archive
    ntdos64-opennt-ccpu-platform-provider-archive
    ntdll.lib
    msvcrt.lib ucrt.lib legacy_stdio_definitions.lib)
target_link_options(ntdos64-opennt-historical-transport-v2-msbop-closure PRIVATE /errorlimit:0)

# R1.2 pointer-route observation. It calls the original Sim32 address mapper
# after CCPU/SAS initialization, but does not execute a BOP, BIOS, DEM, or
# guest instruction.
add_executable(ntdos64-opennt-original-sim32-pointer-fixture EXCLUDE_FROM_ALL
    local/archived/legacy-adapters/ccpu_monitor_sm0_foundation.c
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-effective-addr-reconstruction>
    local/trace/fixtures/original_sim32_pointer_fixture.c
    $<TARGET_OBJECTS:ntdos64-opennt-sim32-objects>)
target_compile_definitions(ntdos64-opennt-original-sim32-pointer-fixture PRIVATE
    CCPU_SM2_USE_EFFECTIVE_ADDR_COMPLETION)
target_link_libraries(ntdos64-opennt-original-sim32-pointer-fixture PRIVATE
    ntdos64-opennt-ccpu-archive
    ntdos64-opennt-ccpu-platform-provider-archive
    ntdll.lib
    msvcrt.lib ucrt.lib legacy_stdio_definitions.lib)

# R2 trace-only SAS lifecycle proof. Unlike the older pointer observation,
# this target lets the original sim32.c + nt_mem.c pair own allocation and
# teardown. It executes no CPU instruction, BIOS selector, BOP, DEM, device,
# or DOS code and is never part of a runtime target.
add_executable(ntdos64-opennt-original-sas-lifecycle-trace-fixture EXCLUDE_FROM_ALL
    local/archived/legacy-adapters/ccpu_monitor_sm0_foundation.c
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-effective-addr-reconstruction>
    local/trace/fixtures/original_sas_lifecycle_trace_fixture.c
    $<TARGET_OBJECTS:ntdos64-opennt-register-bridge-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-ntstubs-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-host-sas-objects>)
target_compile_definitions(ntdos64-opennt-original-sas-lifecycle-trace-fixture PRIVATE
    CCPU_SM0_USE_HISTORICAL_SAS
    CCPU_SM2_USE_EFFECTIVE_ADDR_COMPLETION)
target_link_libraries(ntdos64-opennt-original-sas-lifecycle-trace-fixture PRIVATE
    ntdos64-opennt-ccpu-archive
    ntdos64-opennt-ccpu-platform-provider-archive
    ntdll.lib
    msvcrt.lib ucrt.lib legacy_stdio_definitions.lib)

# R1 full-source diagnostic for the selected direct-CCPU profile. It retains
    # original source owners broadly enough to distinguish missing source modules
    # from actual modern-host seams, but is never run and includes no local BOP,
    # DEM, BIOS, or register replacement.
add_executable(ntdos64-opennt-direct-ccpu-full-source-closure-fixture EXCLUDE_FROM_ALL
    local/trace/fixtures/original_chain_closure_fixture.c
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-effective-addr-reconstruction>
    $<TARGET_OBJECTS:ntdos64-opennt-register-bridge-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-ntstubs-objects>
    # Existing OpenNT owners used only to reduce this link-diagnostic fixture
    # to genuine historical host seams and missing CCPU vectors. nt_cpu.c is
    # compiled in its CCPU wrapper profile without the distinct x86 MONITOR
    # or RISC CPU_40_STYLE register facade.
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-host-cpu-objects>
    # Original CCPU protected-mode interrupt/exception hook owner. It only
    # dispatches a handler previously installed through the historical VDM API;
    # without one it returns FALSE and leaves the CCPU path unchanged.
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-host-inthk-objects>
    # Original CPU_40_STYLE SAS allocation pair. sim32.c owns the historical
    # host_sas_init/term contract and nt_mem.c owns the Intel address-space
    # reservation/commit implementation; this remains link-diagnostic only.
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-host-sas-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-host-eoi-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-host-timer-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-sound-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-copy-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-error-host-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-keycode-nls-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-event-host-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-bios-table-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-bios-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-sim32-ntvdm-profile-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-nt-bop-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-dem-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-xms-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-suballoc-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-umb-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-system-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-support-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-keymouse-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-disks-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-comms-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-video-provider-objects>
    # This target exposes only the original LIM writeback block that the
    # CCPU SAS profile references. It remains a link diagnostic, not EMS
    # runtime admission.
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-lim-writeback-reconstruction-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-x86-emm-host-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-idle-lifecycle-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-display-input-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-graphics-render-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-config-resource-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-disk-host-backend-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-comms-host-backend-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-comms-native-host-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-command-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-dpmi32-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-dbg-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-session-host-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-pif-host-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-oemuni-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-text-display-host-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-vdd-host-objects>
    # These are source-owner diagnostics only. nt_reset remains excluded from
    # any direct CLI/runtime path by the historical process-shell rule.
    $<TARGET_OBJECTS:ntdos64-opennt-reset-shell-objects>)
target_compile_definitions(ntdos64-opennt-direct-ccpu-full-source-closure-fixture PRIVATE
    CCPU_SM2_USE_HISTORICAL_BIOS
    CCPU_SM2_USE_EFFECTIVE_ADDR_COMPLETION
    CCPU_SM2_USE_HISTORICAL_EOI
    CCPU_SM2_USE_HISTORICAL_TIMER)
target_link_libraries(ntdos64-opennt-direct-ccpu-full-source-closure-fixture PRIVATE
    ntdos64-opennt-ccpu-archive
    # The separately generated C_VIDC execution library is an original
    # historical CCPU link input, not a host video replacement.
    ntdos64-opennt-cvidc-archive
    ntdos64-opennt-ccpu-platform-provider-archive
    ntdll.lib
    msvcrt.lib ucrt.lib legacy_stdio_definitions.lib)

# Negative R3 selection diagnostic: omitting DPMI32 leaves the complete
# historical BIOS[] table's MS_bop_3 without its original DpmiDispatch owner.
# It executes no guest code and supplies no BOP/SVC/device behavior.
add_executable(ntdos64-opennt-direct-ccpu-no-dpmi-closure-fixture EXCLUDE_FROM_ALL
    local/trace/fixtures/original_chain_closure_fixture.c
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-effective-addr-reconstruction>
    $<TARGET_OBJECTS:ntdos64-opennt-register-bridge-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-ntstubs-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-host-eoi-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-host-timer-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-event-host-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-bios-table-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-bios-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-sim32-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-nt-bop-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-dem-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-xms-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-suballoc-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-umb-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-system-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-support-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-keymouse-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-disks-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-comms-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-video-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-nt4-emm-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-x86-emm-host-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-idle-lifecycle-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-display-input-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-graphics-render-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-config-resource-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-disk-host-backend-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-comms-host-backend-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-command-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-dbg-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-session-host-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-pif-host-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-oemuni-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-text-display-host-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-vdd-host-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-reset-shell-objects>)
target_compile_definitions(ntdos64-opennt-direct-ccpu-no-dpmi-closure-fixture PRIVATE
    CCPU_SM2_USE_HISTORICAL_BIOS
    CCPU_SM2_USE_EFFECTIVE_ADDR_COMPLETION
    CCPU_SM2_USE_HISTORICAL_EOI
    CCPU_SM2_USE_HISTORICAL_TIMER)
target_link_libraries(ntdos64-opennt-direct-ccpu-no-dpmi-closure-fixture PRIVATE
    ntdos64-opennt-ccpu-archive
    ntdos64-opennt-ccpu-platform-provider-archive
    ntdll.lib
    msvcrt.lib ucrt.lib legacy_stdio_definitions.lib)

# This diagnostic differs from the reset fixture only by the original CVIDC
# archive selected by historical CCPU builds. Its unrecovered SAS vector slots
# are linked to fail-closed trace guards: they record ABI arguments and stop,
# never return a guest-visible pointer or instruction result. It remains
# initialization-only: no CPU simulation, BOP, BIOS, or DEM code is entered.
add_executable(ntdos64-opennt-ccpu-cvidc-register-bridge-fixture EXCLUDE_FROM_ALL
    local/archived/legacy-adapters/ccpu_register_abi_bridge.c
    local/archived/legacy-adapters/ccpu_host_foundation.c
    local/archived/legacy-adapters/ccpu_host_simulate_bridge.c
    local/archived/legacy-adapters/ccpu_unresolved_vector_guard.c
    local/trace/fixtures/ccpu_register_abi_bridge_fixture.c)
target_link_libraries(ntdos64-opennt-ccpu-cvidc-register-bridge-fixture PRIVATE
    ntdos64-opennt-ccpu-archive
    ntdos64-opennt-cvidc-archive
    ntdos64-opennt-ccpu-platform-provider-archive)

# Bounded source-derived completion check for c_effective_addr. Its controlled
# monitor-ABI state prevents unrelated historical host dependencies from being
# silently admitted. It never enters guest execution or a runtime target.
add_executable(ntdos64-opennt-ccpu-effective-addr-fixture EXCLUDE_FROM_ALL
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-effective-addr-reconstruction>
    local/trace/fixtures/ccpu_effective_addr_fixture.c)

# R3.C ABI fixture for the one-shot direct-CLI command/PIF transport. It has
# no config.c, CCPU, BIOS, BOP, DEM, DOS, or runner dependency.
add_executable(ntdos64-opennt-direct-cli-command-transport-fixture EXCLUDE_FROM_ALL
    local/compat/host/direct_cli_command_transport.c
    local/trace/fixtures/direct_cli_command_transport_fixture.c)
target_include_directories(ntdos64-opennt-direct-cli-command-transport-fixture PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc)

# R3.C fixture for the obsolete PEB ConsoleHandle formatting field used only
# by config.c's temporary title string. It does not inspect or mutate a console.
add_executable(ntdos64-opennt-direct-cli-console-identity-fixture EXCLUDE_FROM_ALL
    local/compat/host/direct_cli_console_identity.c
    local/trace/fixtures/direct_cli_console_identity_fixture.c)

# Original CCPU execution dependencies, kept apart from the full PC/AT
# aggregate so the reset fixture can expose their real downstream closure.
add_library(ntdos64-opennt-ccpu-platform-provider-objects OBJECT EXCLUDE_FROM_ALL
    overlay/base/mvdm/softpc.new/base/support/ios.c
    base/mvdm/softpc.new/base/system/ica.c
    base/mvdm/softpc.new/base/system/qevnt.c
    base/mvdm/softpc.new/base/system/rom.c)
add_library(ntdos64-opennt-ccpu-platform-provider-archive STATIC EXCLUDE_FROM_ALL
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-platform-provider-objects>)

# The remaining members of the original BIOS library.  bios.c itself remains
# in the host-stack gate so the provider group can be audited independently.
add_library(ntdos64-opennt-bios-provider-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/softpc.new/base/bios/build_id.c
    base/mvdm/softpc.new/base/bios/cmos_bis.c
    base/mvdm/softpc.new/base/bios/cntlbop.c
    base/mvdm/softpc.new/base/bios/emm_fncs.c
    base/mvdm/softpc.new/base/bios/equipmnt.c
    base/mvdm/softpc.new/base/bios/ill_bop.c
    base/mvdm/softpc.new/base/bios/mem_size.c
    base/mvdm/softpc.new/base/bios/reset.c
    base/mvdm/softpc.new/base/bios/rom_basc.c
    base/mvdm/softpc.new/base/bios/rtc_bios.c
    base/mvdm/softpc.new/base/bios/slave_bs.c
    base/mvdm/softpc.new/base/bios/tape_io.c
    base/mvdm/softpc.new/base/bios/virtual.c)

# The remaining members of the original system library.  ICA and QEVNT stay
# in the host-stack gate, where their CCPU-facing dependencies are audited.
add_library(ntdos64-opennt-system-provider-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/softpc.new/base/system/at_dma.c
    base/mvdm/softpc.new/base/system/cmosnt.c
    base/mvdm/softpc.new/base/system/dummy_nt.c
    base/mvdm/softpc.new/base/system/idetect.c
    base/mvdm/softpc.new/base/system/timer.c
    base/mvdm/softpc.new/base/system/timestrb.c
    base/mvdm/softpc.new/base/system/unexp_nt.c
    overlay/base/mvdm/softpc.new/base/system/illegalp.c)

# The host-stack gate already carries the recorded historical ios.c overlay.
# Keep the other original support-library members together so their startup,
# clock, environment, XT configuration, and termination ownership can be
# audited without adding a replacement lifecycle implementation.
add_library(ntdos64-opennt-support-provider-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/softpc.new/base/support/terminat.c
    base/mvdm/softpc.new/base/support/time_day.c
    base/mvdm/softpc.new/base/support/main.c
    base/mvdm/softpc.new/base/support/get_env.c
    base/mvdm/softpc.new/base/support/xt.c)

# Keep the complete original keyboard/mouse/PPI library together. Its host
# callbacks remain unresolved for later provider/shim classification; this gate
# must not become a substitute input implementation.
add_library(ntdos64-opennt-keymouse-provider-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/softpc.new/base/keymouse/keyba.c
    base/mvdm/softpc.new/base/keymouse/keybd_io.c
    base/mvdm/softpc.new/base/keymouse/mouse.c
    base/mvdm/softpc.new/base/keymouse/ppi.c
    base/mvdm/softpc.new/base/keymouse/mouse_io.c)

# Complete original floppy/fixed-disk controller library.  Its historical NT
# backend remains a separate later gate; this target supplies no replacement
# media or INT 13h behavior.
add_library(ntdos64-opennt-disks-provider-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/softpc.new/base/disks/floppy.c
    base/mvdm/softpc.new/base/disks/floppy_i.c
    base/mvdm/softpc.new/base/disks/diskbios.c
    base/mvdm/softpc.new/base/disks/fdisk.c
    base/mvdm/softpc.new/base/disks/fla.c
    base/mvdm/softpc.new/base/disks/gfi.c
    base/mvdm/softpc.new/base/disks/gfi_ibm.c
    base/mvdm/softpc.new/base/disks/gfi_mpty.c
    base/mvdm/softpc.new/base/disks/gfi_sflp.c
    base/mvdm/softpc.new/base/disks/gfi_test.c)

# Narrow original disk BIOS owner for startup-trace compositions that already
# select GFI through separately owned objects.  This avoids duplicate GFI
# definitions while preserving diskbios.c unchanged.
add_library(ntdos64-opennt-diskbios-provider-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/softpc.new/base/disks/diskbios.c)

# Complete original serial/parallel controller library. The separate NT host
# serial/printer backends remain unselected; this target supplies no new port
# semantics and is object-analysis input only.
add_library(ntdos64-opennt-comms-provider-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/softpc.new/base/comms/com.c
    base/mvdm/softpc.new/base/comms/printer.c
    base/mvdm/softpc.new/base/comms/printer_.c
    base/mvdm/softpc.new/base/comms/prt_scrn.c
    base/mvdm/softpc.new/base/comms/rs232_io.c)

# Historical Windows disk/floppy backend, deliberately separate from the
# controller gate. It is object-analysis input only: its direct-drive policy
# is not admitted to a future BYOB runtime.
add_library(ntdos64-opennt-disk-host-backend-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/softpc.new/host/src/nt_fdisk.c
    base/mvdm/softpc.new/host/src/nt_rflop.c)

add_library(ntdos64-opennt-contained-media-fdisk-objects OBJECT EXCLUDE_FROM_ALL
    local/archived/legacy-adapters/contained_media_fdisk.c)

add_executable(ntdos64-opennt-contained-media-fdisk-test EXCLUDE_FROM_ALL
    local/archived/legacy-adapters/contained_media_fdisk.c
    local/trace/fixtures/contained_media_fdisk_test.c)

# Complete original OEM file/process facade. Its use of obsolete TEB/PEB and
# RTL state is assessed as one unit; this gate does not select it for runtime.
add_library(ntdos64-opennt-oemuni-provider-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/oemuni/file.c
    base/mvdm/oemuni/process.c)

add_library(ntdos64-opennt-contained-oemuni-objects OBJECT EXCLUDE_FROM_ALL
    local/archived/legacy-adapters/contained_oemuni.c)

add_executable(ntdos64-opennt-contained-oemuni-test EXCLUDE_FROM_ALL
    local/archived/legacy-adapters/contained_oemuni.c
    local/trace/fixtures/contained_oemuni_test.c)

# Bounded caller fixture: compile the original DEM directory SVC handlers
# against the contained facade. Its register/memory observations are test-only
# and this executable is never part of an engine or startup path.
add_executable(ntdos64-opennt-contained-oemuni-demdir-fixture EXCLUDE_FROM_ALL
    base/mvdm/dos/dem/demdir.c
    local/archived/legacy-adapters/contained_oemuni.c
    local/trace/fixtures/contained_oemuni_demdir_fixture.c)

# Complete original video library; host display callbacks remain outside this
# object gate for later historical-provider versus modern-facade classification.
add_library(ntdos64-opennt-video-provider-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/softpc.new/base/video/ega_mode.c
    base/mvdm/softpc.new/base/video/ega_prts.c
    base/mvdm/softpc.new/base/video/ega_vide.c
    base/mvdm/softpc.new/base/video/gfx_updt.c
    base/mvdm/softpc.new/base/video/video.c
    base/mvdm/softpc.new/base/video/gvi.c
    base/mvdm/softpc.new/base/video/video_io.c
    base/mvdm/softpc.new/base/video/v7_ports.c
    base/mvdm/softpc.new/base/video/v7_video.c
    base/mvdm/softpc.new/base/video/vga_mode.c
    base/mvdm/softpc.new/base/video/vga_prts.c
    base/mvdm/softpc.new/base/video/vga_vide.c
    base/mvdm/softpc.new/base/video/ega_read.c
    base/mvdm/softpc.new/base/video/ega_writ.c
    base/mvdm/softpc.new/base/video/ega_dmmy.c
    base/mvdm/softpc.new/base/video/ega_trcr.c
    base/mvdm/softpc.new/base/video/egawrtm0.c
    base/mvdm/softpc.new/base/video/egwrtm12.c
    base/mvdm/softpc.new/base/video/cga.c)

# The original CCPU FPU archive excludes these VIDC global-state units. Keep
# them as an explicit profile increment, rather than changing that archived
# baseline. The local storage overlay retains the localfm.c declarations absent
# from the selected archive with the generated-header closure evidenced by
# sascdef.c; vglob.c accesses VIDC state through CCPU's Gdp workspace layout.
add_library(ntdos64-opennt-ccpu-vidc-global-objects OBJECT EXCLUDE_FROM_ALL
    local/archived/legacy-adapters/ccpu_vidc_global_storage.c
    base/mvdm/softpc.new/base/ccpu386/vglob.c)

# Keep vglob.c separately selectable so that this live caller can be tested
# with C_VIDC's original ev_glue.c workspace owner, without admitting the
# archived localfm storage adapter.
add_library(ntdos64-opennt-ccpu-vglob-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/softpc.new/base/ccpu386/vglob.c)

# Link-only closure probe for the original video owner of read_pointers. This
# intentionally extends, rather than changes, the small CCPU/CVIDC baseline:
# it records the historical display, port, and initialization dependencies
# introduced by the complete video provider. It never executes guest code.
add_executable(ntdos64-opennt-ccpu-cvidc-video-closure-fixture EXCLUDE_FROM_ALL
    local/archived/legacy-adapters/ccpu_register_abi_bridge.c
    local/archived/legacy-adapters/ccpu_host_foundation.c
    local/archived/legacy-adapters/ccpu_host_simulate_bridge.c
    local/archived/legacy-adapters/ccpu_unresolved_vector_guard.c
    local/trace/fixtures/ccpu_register_abi_bridge_fixture.c
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-vidc-global-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-video-provider-objects>
    # Direct inclusion measures the just-proven original host objects without
    # changing the broader provider archive or admitting a runtime policy.
    $<TARGET_OBJECTS:ntdos64-opennt-host-display-input-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-graphics-render-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-config-resource-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-disk-host-backend-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-comms-host-backend-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-host-eoi-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-host-timer-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-command-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-dpmi32-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-dbg-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-register-bridge-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-event-host-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-session-host-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-nt4-emm-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-x86-emm-host-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-text-display-host-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-pif-host-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-oemuni-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-idle-lifecycle-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-vdd-host-objects>)
target_link_libraries(ntdos64-opennt-ccpu-cvidc-video-closure-fixture PRIVATE
    ntdos64-opennt-ccpu-archive
    ntdos64-opennt-cvidc-archive
    ntdos64-opennt-ccpu-platform-provider-archive
    ntdos64-opennt-historical-provider-archive
    # Closure-only import classification: never admitted to the runner or a
    # historical provider archive. This separates current ntdll exports from
    # the VDM-specific host seams that require an explicit modern shim.
    ntdll.lib)
target_compile_definitions(ntdos64-opennt-ccpu-cvidc-video-closure-fixture PRIVATE
    CCPU_USE_HISTORICAL_MOUSE_OWNER
    CCPU_USE_HISTORICAL_EOI_TIMER_OWNERS
    CCPU_USE_HISTORICAL_REGISTER_BRIDGE)

# Original CCPU-oriented host callback owners. Each remains an independent
# default-off compile gate: nt_cpu.c carries the generated VIDC/monitor ABI,
# while EOI and timer can be classified without that unrelated blocker. None
# is a modern replacement or is linked into a runtime target.
add_library(ntdos64-opennt-ccpu-host-cpu-objects OBJECT EXCLUDE_FROM_ALL
    overlay/base/mvdm/softpc.new/host/src/nt_cpu.c)
add_library(ntdos64-opennt-ccpu-host-inthk-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/softpc.new/host/src/nt_inthk.c)
add_library(ntdos64-opennt-ccpu-host-sas-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/softpc.new/host/src/sim32.c
    base/mvdm/softpc.new/host/src/nt_mem.c)
# The no-SM0 CCPU closure already owns sim32.c under its original names. Keep
# nt_mem.c independently selectable so its actual CPU_40_STYLE SAS allocation
# owner can be recovered without a second Sim32 definition.
add_library(ntdos64-opennt-ccpu-host-intel-memory-owner-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/softpc.new/host/src/nt_mem.c)
add_library(ntdos64-opennt-ccpu-host-eoi-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/softpc.new/host/src/nt_eoi.c)
add_library(ntdos64-opennt-ccpu-host-timer-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/softpc.new/host/src/nt_timer.c)

# Historical host/src coverage inventory.  These source units are listed by
# the NT4 host/src/sources file but are not a runtime closure or a replacement
# implementation.  They compile independently so profile/header collisions
# are recorded before any dynamically reached owner is admitted to a link.
add_library(ntdos64-opennt-historical-host-portable-inventory-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/softpc.new/host/src/cpucstbs.c
    base/mvdm/softpc.new/host/src/nt_emm.c
    base/mvdm/softpc.new/host/src/nt_sas.c
    base/mvdm/softpc.new/host/src/nt_yoda.c
    base/mvdm/softpc.new/host/src/stubs.c)
add_library(ntdos64-opennt-historical-host-native-inventory-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/softpc.new/host/src/nt_input.c
    base/mvdm/softpc.new/host/src/nt_munge.c
    base/mvdm/softpc.new/host/src/nt_sec.c
    base/mvdm/softpc.new/host/src/nt_thred.c)

# This probe calls only the original counter initialization/read pair. It
# deliberately does not start a heartbeat, event loop, or a local clock shim.
add_executable(ntdos64-opennt-original-perf-counter-fixture EXCLUDE_FROM_ALL
    local/trace/fixtures/original_perf_counter_fixture.c
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-host-timer-objects>)
target_link_libraries(ntdos64-opennt-original-perf-counter-fixture PRIVATE
    ntdll.lib
    msvcrt.lib ucrt.lib legacy_stdio_definitions.lib)

# The original host sound owner stays independent from the broader display
# group so timer closure measures only its waveform/alarm boundary.
add_library(ntdos64-opennt-host-sound-objects OBJECT EXCLUDE_FROM_ALL
    overlay/base/mvdm/softpc.new/host/src/nt_sound.c)

# copy_fnc.c and nt_sound.c establish the historical native declaration order
# themselves. Keep them separate from the Win32-first display/input units.
add_library(ntdos64-opennt-host-copy-objects OBJECT EXCLUDE_FROM_ALL
    overlay/base/mvdm/softpc.new/host/src/copy_fnc.c)

# Preserve the original host display/input owner boundary as one compile gate.
# It supplies no modern policy and is not linked into an executable here.
add_library(ntdos64-opennt-host-display-input-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/softpc.new/host/src/nt_graph.c
    base/mvdm/softpc.new/host/src/nt_fulsc.c
    base/mvdm/softpc.new/host/src/nt_mouse.c)

# Keep the text-section/console-VDM owner separate from generic display/input
# sources. Its closure identifies the historical console driver contract and
# is never admitted to a runtime target.
add_library(ntdos64-opennt-text-display-host-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/softpc.new/host/src/nt_det.c)

# The original idle/timeslice/reset host owner is separate from console and
# device policy. Keep it default-disabled so its obligations can be measured
# without turning it into a substitute monitor lifecycle.
add_library(ntdos64-opennt-host-idle-lifecycle-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/softpc.new/host/src/nt_unix.c)

# Preserve the original VDD BOP/adapter/IRQ/DMA bridge as a distinct host
# boundary. Its object is closure-only and cannot become a substitute BIOS or
# DOS service implementation.
add_library(ntdos64-opennt-vdd-host-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/softpc.new/host/src/nt_vdd.c)

# The original paint-table and palette owners are distinct from display mode
# transitions. Keep their source set together and default-disabled so the
# video closure can measure historical rendering dependencies without adding
# a console policy or a runtime path.
add_library(ntdos64-opennt-host-graphics-render-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/softpc.new/host/src/nt_hosts.c
    base/mvdm/softpc.new/host/src/nt_cga.c
    base/mvdm/softpc.new/host/src/nt_ega.c
    base/mvdm/softpc.new/host/src/nt_vga.c)

# The original serial, printer, and WOW-COM host owners form one native host
# backend. They are retained only as a default-disabled compile/closure input;
# direct device policy and asynchronous I/O are not admitted to a runtime.
add_library(ntdos64-opennt-comms-host-backend-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/softpc.new/host/src/nt_wcom.c)
add_library(ntdos64-opennt-comms-native-host-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/softpc.new/host/src/nt_com.c
    base/mvdm/softpc.new/host/src/nt_lpt.c
    base/mvdm/softpc.new/host/src/nt_ntfun.c)

# Preserve the NT4 COMMAND library as one default-disabled historical group.
# Its dispatcher is not useful without the original SVC handler/state units.
add_library(ntdos64-opennt-command-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/dos/command/cmd.c
    base/mvdm/dos/command/cmddata.c
    base/mvdm/dos/command/cmddisp.c
    base/mvdm/dos/command/cmdexec.c
    base/mvdm/dos/command/cmdexit.c
    base/mvdm/dos/command/cmdmisc.c
    base/mvdm/dos/command/cmdpif.c
    base/mvdm/dos/command/cmdredir.c
    base/mvdm/dos/command/cmdconf.c
    base/mvdm/dos/command/cmdkeyb.c
    base/mvdm/dos/command/cmdenv.c)

# Preserve the NT4 x86 DPMI32 monitor library as its original source group.
# The RISC-only units remain staged for provenance, but are not part of this
# x86 target. This group is never a default build or runner dependency.
add_library(ntdos64-opennt-dpmi32-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/dpmi32/debug.c
    base/mvdm/dpmi32/dpmi32.c
    base/mvdm/dpmi32/dpmiint.c
    base/mvdm/dpmi32/dpmiselr.c
    base/mvdm/dpmi32/buffer.c
    base/mvdm/dpmi32/data.c
    base/mvdm/dpmi32/int21map.c
    base/mvdm/dpmi32/modesw.c
    base/mvdm/dpmi32/register.c
    base/mvdm/dpmi32/savestat.c
    base/mvdm/dpmi32/stack.c
    base/mvdm/dpmi32/vxd.c
    base/mvdm/dpmi32/xmem.c
    base/mvdm/dpmi32/i386/dpmi386.c
    overlay/base/mvdm/dpmi32/i386/dpmimem.c
    base/mvdm/dpmi32/i386/dpmimisc.c)

# Preserve the NT4 DBG library as its own default-disabled historical product
# boundary. It owns DBGDispatch but is never a runner/debug-console dependency.
add_library(ntdos64-opennt-dbg-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/dbg/dbg.c)

# The historical CCPU register bridge translates the public monitor register
# entry points to generated c_get/c_set owners. It is a link-only recovery
# input, never a substitute register implementation.
add_library(ntdos64-opennt-register-bridge-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/softpc.new/host/src/nt_aorc.c)

# Preserve the original NT event/input host owner for closure measurement.
# It remains separate from the runner and from any modern event shim.
add_library(ntdos64-opennt-event-host-objects OBJECT EXCLUDE_FROM_ALL
    overlay/base/mvdm/softpc.new/host/src/nt_event.c)

# Preserve the original NTVDM initialization/session owner. It remains a
# default-disabled historical input until its PIF, VDD, and console seams are
# explicitly measured.
add_library(ntdos64-opennt-session-host-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/softpc.new/host/src/nt_msscs.c)

# BYOB variant of the same historical session owner. Only the NTIO source
# lookup and first-session C: write side effect are overlaid; it is not linked
# into a runtime until its VDD/DEM/XMS closure is measured.
add_library(ntdos64-opennt-session-byob-objects OBJECT EXCLUDE_FROM_ALL
    overlay/base/mvdm/softpc.new/host/src/nt_msscs.c)

# Trace-only BYOB session variant. It retains the same source and BYOB loader
# seam, but stops immediately before the original io_init owner.
add_library(ntdos64-opennt-session-byob-trace-objects OBJECT EXCLUDE_FROM_ALL
    overlay/base/mvdm/softpc.new/host/src/nt_msscs.c)

add_library(ntdos64-opennt-session-byob-io-trace-objects OBJECT EXCLUDE_FROM_ALL
    overlay/base/mvdm/softpc.new/host/src/nt_msscs.c)

add_library(ntdos64-opennt-session-byob-reset-trace-objects OBJECT EXCLUDE_FROM_ALL
    overlay/base/mvdm/softpc.new/host/src/nt_msscs.c)

add_executable(ntdos64-opennt-byob-ntio-path-fixture EXCLUDE_FROM_ALL
    local/compat/host/byob_ntio_path.c
    local/trace/fixtures/byob_ntio_path_fixture.c)

# Preserve historical process-shell and host-error source owners for closure
# diagnosis. Neither target is an admissible direct CLI/runtime component.
add_library(ntdos64-opennt-reset-shell-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/softpc.new/host/src/nt_reset.c)
add_library(ntdos64-opennt-error-host-objects OBJECT EXCLUDE_FROM_ALL
    overlay/base/mvdm/softpc.new/host/src/nt_error.c)

# Original keyboard-code and NLS/string owners, retained as one default-off
# closure input. They provide source-owned conversion and message data only.
add_library(ntdos64-opennt-host-keycode-nls-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/softpc.new/host/src/nt_keycd.c
    base/mvdm/softpc.new/host/src/nt_nls.c)

# Preserve the original PIF parser/configuration owner independently. It is
# linked only into the closure fixture to identify its file and environment
# dependencies; no replacement PIF policy is supplied here.
add_library(ntdos64-opennt-pif-host-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/softpc.new/host/src/nt_pif.c)

# R3.C parser-only proof. Its temporary input is generated and deleted by the
# fixture; no PIF bytes are retained in source, build inputs, or the runner.
add_executable(ntdos64-opennt-pif-default-parser-fixture EXCLUDE_FROM_ALL
    local/trace/fixtures/pif_parser_fixture_support.c
    local/trace/fixtures/pif_default_parser_fixture.c
    $<TARGET_OBJECTS:ntdos64-opennt-pif-host-objects>)
target_include_directories(ntdos64-opennt-pif-default-parser-fixture PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/windows/inc)

# Preserve the NT4 EMS/LIM middle-layer manager independently from the NT4.5
# reference probe. It is a default-disabled closure input only.
add_library(ntdos64-opennt-nt4-emm-objects OBJECT EXCLUDE_FROM_ALL
    overlay/base/mvdm/softpc.new/base/dos/emm_mngr.c)

# The NTVDM source profile suppresses the original LIM writeback helpers while
# CCPU's SAS source retains their LIM calls. Keep the recovered original block
# isolated to the full-source closure diagnostic until a real caller reaches it.
add_library(ntdos64-opennt-ccpu-lim-writeback-reconstruction-objects OBJECT EXCLUDE_FROM_ALL
    overlay/base/mvdm/softpc.new/base/dos/emm_mngr.c)

add_library(ntdos64-opennt-x86-emm-host-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/softpc.new/host/src/x86_emm.c)

# Original NT configuration and resource-file owners used by the CCPU ROM
# path. Compile them together only to expose their historical dependency graph;
# they retain their original profile/host-file semantics and are never linked
# into a CCPU fixture, the runner, or a default target.
add_library(ntdos64-opennt-config-resource-objects OBJECT EXCLUDE_FROM_ALL
    overlay/base/mvdm/softpc.new/host/src/config.c
    base/mvdm/softpc.new/host/src/nt_rez.c)

# The standalone config-prefix trace keeps the original config owner separate
# from nt_rez so it can stop at the first original SAS lifecycle call.
add_library(ntdos64-opennt-config-prefix-objects OBJECT EXCLUDE_FROM_ALL
    overlay/base/mvdm/softpc.new/host/src/config.c)

# This keeps the same original config.c input, but uses the CCPU research
# profile without CPU_40_STYLE. That macro selects a generated non-x86
# debugger register view; it is not part of the DOS startup contract.
add_library(ntdos64-opennt-config-x86-prefix-objects OBJECT EXCLUDE_FROM_ALL
    overlay/base/mvdm/softpc.new/host/src/config.c)

add_executable(ntdos64-opennt-config-prefix-trace-fixture EXCLUDE_FROM_ALL
    local/trace/fixtures/config_prefix_stop_support.c
    local/trace/fixtures/config_prefix_pif_guard.c
    local/trace/fixtures/config_prefix_trace_fixture.c
    local/compat/host/direct_cli_command_transport.c
    local/compat/host/direct_cli_console_identity.c
    $<TARGET_OBJECTS:ntdos64-opennt-config-prefix-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-pif-host-objects>)
target_include_directories(ntdos64-opennt-config-prefix-trace-fixture PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/windows/inc)

# R3.C next cut: let original config.c perform original sas_init, which in
# turn owns rom_init/copyROM. Stop at the following original InitNtCpuInfo
# caller boundary; this is still not CPU initialization or guest execution.
add_executable(ntdos64-opennt-config-sas-prefix-trace-fixture EXCLUDE_FROM_ALL
    local/archived/legacy-adapters/ccpu_monitor_sm0_foundation.c
    local/trace/fixtures/config_prefix_stop_support.c
    local/trace/fixtures/config_prefix_pif_guard.c
    local/trace/fixtures/config_prefix_trace_fixture.c
    local/compat/host/direct_cli_command_transport.c
    local/compat/host/direct_cli_console_identity.c
    $<TARGET_OBJECTS:ntdos64-opennt-config-prefix-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-pif-host-objects>)
target_compile_definitions(ntdos64-opennt-config-sas-prefix-trace-fixture PRIVATE
    CCPU_SM0_USE_ORIGINAL_CONFIG
    CONFIG_PREFIX_TRACE_SAS_INIT)
target_include_directories(ntdos64-opennt-config-sas-prefix-trace-fixture PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/windows/inc)
target_link_libraries(ntdos64-opennt-config-sas-prefix-trace-fixture PRIVATE
    ntdos64-opennt-ccpu-archive
    ntdos64-opennt-ccpu-platform-provider-archive
    msvcrt.lib ucrt.lib legacy_stdio_definitions.lib)

# Let original config.c pass original sas_init and demonstrate that the
# non-CPU_40 profile reaches the next original lifecycle owner. This is a
# bounded trace target, not a runtime component.
add_executable(ntdos64-opennt-config-x86-prefix-trace-fixture EXCLUDE_FROM_ALL
    local/archived/legacy-adapters/ccpu_monitor_sm0_foundation.c
    local/trace/fixtures/config_prefix_stop_support.c
    local/trace/fixtures/config_prefix_pif_guard.c
    local/trace/fixtures/config_prefix_trace_fixture.c
    local/compat/host/direct_cli_command_transport.c
    local/compat/host/direct_cli_console_identity.c
    $<TARGET_OBJECTS:ntdos64-opennt-config-x86-prefix-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-pif-host-objects>)
target_compile_definitions(ntdos64-opennt-config-x86-prefix-trace-fixture PRIVATE
    CCPU_SM0_USE_ORIGINAL_CONFIG
    CONFIG_PREFIX_TRACE_SAS_INIT
    CONFIG_PREFIX_TRACE_POST_CPU40)
target_include_directories(ntdos64-opennt-config-x86-prefix-trace-fixture PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/windows/inc)
target_link_libraries(ntdos64-opennt-config-x86-prefix-trace-fixture PRIVATE
    ntdos64-opennt-ccpu-archive
    ntdos64-opennt-ccpu-platform-provider-archive
    msvcrt.lib ucrt.lib legacy_stdio_definitions.lib)

# Continue the same bounded trace through the original CCPU-side UMB owner.
# It stops at the following original floppy configuration query.
add_executable(ntdos64-opennt-config-x86-umb-trace-fixture EXCLUDE_FROM_ALL
    local/archived/legacy-adapters/ccpu_monitor_sm0_foundation.c
    local/trace/fixtures/config_prefix_stop_support.c
    local/trace/fixtures/config_prefix_pif_guard.c
    local/trace/fixtures/config_prefix_trace_fixture.c
    local/compat/host/direct_cli_command_transport.c
    local/compat/host/direct_cli_console_identity.c
    $<TARGET_OBJECTS:ntdos64-opennt-config-x86-prefix-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-umb-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-pif-host-objects>)
target_compile_definitions(ntdos64-opennt-config-x86-umb-trace-fixture PRIVATE
    CCPU_SM0_USE_ORIGINAL_CONFIG
    CONFIG_PREFIX_TRACE_SAS_INIT
    CONFIG_PREFIX_TRACE_POST_CPU40
    CONFIG_PREFIX_TRACE_ORIGINAL_UMB
    CONFIG_PREFIX_USE_ORIGINAL_UMB)
target_include_directories(ntdos64-opennt-config-x86-umb-trace-fixture PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/windows/inc)
target_link_libraries(ntdos64-opennt-config-x86-umb-trace-fixture PRIVATE
    ntdos64-opennt-ccpu-archive
    ntdos64-opennt-ccpu-platform-provider-archive
    msvcrt.lib ucrt.lib legacy_stdio_definitions.lib)

# Admit original GFI routing, but stop before the historical direct A:/B:
# backend makes a host-device decision.
add_executable(ntdos64-opennt-config-x86-gfi-trace-fixture EXCLUDE_FROM_ALL
    local/archived/legacy-adapters/ccpu_monitor_sm0_foundation.c
    local/trace/fixtures/config_prefix_stop_support.c
    local/trace/fixtures/config_prefix_pif_guard.c
    local/trace/fixtures/config_prefix_trace_fixture.c
    local/compat/host/direct_cli_command_transport.c
    local/compat/host/direct_cli_console_identity.c
    $<TARGET_OBJECTS:ntdos64-opennt-config-x86-prefix-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-umb-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-gfi-config-trace-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-pif-host-objects>)
target_compile_definitions(ntdos64-opennt-config-x86-gfi-trace-fixture PRIVATE
    CCPU_SM0_USE_ORIGINAL_CONFIG
    CONFIG_PREFIX_TRACE_SAS_INIT
    CONFIG_PREFIX_TRACE_POST_CPU40
    CONFIG_PREFIX_TRACE_ORIGINAL_UMB
    CONFIG_PREFIX_TRACE_ORIGINAL_GFI
    CONFIG_PREFIX_USE_ORIGINAL_UMB
    CONFIG_PREFIX_USE_ORIGINAL_GFI)
target_include_directories(ntdos64-opennt-config-x86-gfi-trace-fixture PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/windows/inc)
target_link_libraries(ntdos64-opennt-config-x86-gfi-trace-fixture PRIVATE
    ntdos64-opennt-ccpu-archive
    ntdos64-opennt-ccpu-platform-provider-archive
    msvcrt.lib ucrt.lib legacy_stdio_definitions.lib)

# Trace-only no-media fixture: retain the original config/GFI flow, then stop
# at a fail-closed host boundary without probing A: or B:. This is not a
# modern-host policy implementation and must never be linked into a runtime
# or provider library.
add_executable(ntdos64-opennt-config-x86-media-absent-fixture EXCLUDE_FROM_ALL
    local/archived/legacy-adapters/ccpu_monitor_sm0_foundation.c
    local/trace/fixtures/config_prefix_stop_support.c
    local/trace/fixtures/config_prefix_pif_guard.c
    local/trace/fixtures/config_prefix_trace_fixture.c
    local/compat/host/direct_cli_command_transport.c
    local/compat/host/direct_cli_console_identity.c
    local/trace/fixtures/media_capability_absent.c
    $<TARGET_OBJECTS:ntdos64-opennt-config-x86-prefix-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-umb-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-gfi-config-trace-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-pif-host-objects>)
target_compile_definitions(ntdos64-opennt-config-x86-media-absent-fixture PRIVATE
    CCPU_SM0_USE_ORIGINAL_CONFIG
    CONFIG_PREFIX_TRACE_SAS_INIT
    CONFIG_PREFIX_EXPECT_CONFIG_RETURN
    CONFIG_PREFIX_USE_ORIGINAL_UMB
    CONFIG_PREFIX_USE_ORIGINAL_GFI
    CONFIG_PREFIX_USE_MODERN_MEDIA_CAPABILITY)
target_include_directories(ntdos64-opennt-config-x86-media-absent-fixture PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/windows/inc)
target_link_libraries(ntdos64-opennt-config-x86-media-absent-fixture PRIVATE
    ntdos64-opennt-ccpu-archive
    ntdos64-opennt-ccpu-platform-provider-archive
    msvcrt.lib ucrt.lib legacy_stdio_definitions.lib)

# Execute the original BYOB session loader only through its fixed-state SAS
# prelude. io_init is the first unadmitted lifecycle owner and stops the trace.
add_executable(ntdos64-opennt-session-byob-prefix-trace-fixture EXCLUDE_FROM_ALL
    local/archived/legacy-adapters/ccpu_monitor_sm0_foundation.c
    local/trace/fixtures/config_prefix_stop_support.c
    local/trace/fixtures/config_prefix_pif_guard.c
    local/trace/fixtures/config_prefix_trace_fixture.c
    local/compat/host/direct_cli_command_transport.c
    local/compat/host/direct_cli_console_identity.c
    local/compat/host/byob_ntio_path.c
    $<TARGET_OBJECTS:ntdos64-opennt-config-x86-prefix-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-umb-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-gfi-config-trace-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-session-byob-trace-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-pif-host-objects>)
target_compile_definitions(ntdos64-opennt-session-byob-prefix-trace-fixture PRIVATE
    CCPU_SM0_USE_ORIGINAL_CONFIG
    CONFIG_PREFIX_TRACE_SAS_INIT
    CONFIG_PREFIX_TRACE_SESSION_INIT
    CONFIG_PREFIX_USE_ORIGINAL_UMB
    CONFIG_PREFIX_USE_ORIGINAL_GFI
    CONFIG_PREFIX_USE_MODERN_MEDIA_CAPABILITY)
target_include_directories(ntdos64-opennt-session-byob-prefix-trace-fixture PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/windows/inc)
target_link_libraries(ntdos64-opennt-session-byob-prefix-trace-fixture PRIVATE
    ntdos64-opennt-ccpu-archive
    ntdos64-opennt-ccpu-platform-provider-archive
    msvcrt.lib ucrt.lib legacy_stdio_definitions.lib)

# Let the original SoftPC I/O routing tables initialize, then stop before the
# following historical TLS/reset/VDD sequence.
add_executable(ntdos64-opennt-session-byob-io-trace-fixture EXCLUDE_FROM_ALL
    local/archived/legacy-adapters/ccpu_monitor_sm0_foundation.c
    local/trace/fixtures/config_prefix_stop_support.c
    local/trace/fixtures/config_prefix_pif_guard.c
    local/trace/fixtures/config_prefix_trace_fixture.c
    local/compat/host/direct_cli_command_transport.c
    local/compat/host/direct_cli_console_identity.c
    local/trace/fixtures/media_capability_absent.c
    local/compat/host/byob_ntio_path.c
    $<TARGET_OBJECTS:ntdos64-opennt-config-x86-prefix-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-umb-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-gfi-config-trace-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-session-byob-io-trace-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-pif-host-objects>)
target_compile_definitions(ntdos64-opennt-session-byob-io-trace-fixture PRIVATE
    CCPU_SM0_USE_ORIGINAL_CONFIG
    CONFIG_PREFIX_TRACE_SAS_INIT
    CONFIG_PREFIX_TRACE_SESSION_INIT
    CONFIG_PREFIX_TRACE_SESSION_AFTER_IO
    CONFIG_PREFIX_USE_ORIGINAL_UMB
    CONFIG_PREFIX_USE_ORIGINAL_GFI
    CONFIG_PREFIX_USE_MODERN_MEDIA_CAPABILITY)
target_include_directories(ntdos64-opennt-session-byob-io-trace-fixture PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/windows/inc)
target_link_libraries(ntdos64-opennt-session-byob-io-trace-fixture PRIVATE
    ntdos64-opennt-ccpu-archive
    ntdos64-opennt-ccpu-platform-provider-archive
    msvcrt.lib ucrt.lib legacy_stdio_definitions.lib)

# Compile the OpenNT reset source from an overlay only for the bounded trace.
# The overlay adds checkpoints but no replacement device, BIOS, BOP, or DOS code.
set(NTDOS64_RESET_TRACE_STOP_STAGE 10)
set(NTDOS64_SESSION_TRACE_VINT_STOP_STAGE 4)
add_library(ntdos64-opennt-reset-trace-overlay-objects OBJECT EXCLUDE_FROM_ALL
    overlay/base/mvdm/softpc.new/base/bios/reset.c)
add_library(ntdos64-opennt-cpu-init-trace-overlay-objects OBJECT EXCLUDE_FROM_ALL
    overlay/base/mvdm/softpc.new/base/ccpu386/c_main.c)
# This overlay preserves the original host_applInit body and adds bounded
# checkpoints around its historical lifecycle. It is not a process shell and
# is never admitted to a runtime target.
add_library(ntdos64-opennt-host-applinit-trace-overlay-objects OBJECT EXCLUDE_FROM_ALL
    overlay/base/mvdm/softpc.new/host/src/nt_reset.c)
add_library(ntdos64-opennt-host-applinit-eoi-trace-overlay-objects OBJECT EXCLUDE_FROM_ALL
    overlay/base/mvdm/softpc.new/host/src/nt_eoi.c)
add_library(ntdos64-opennt-host-applinit-ui-trace-overlay-objects OBJECT EXCLUDE_FROM_ALL
    overlay/base/mvdm/softpc.new/host/src/nt_hosts.c)
# nt_graph.c is the original owner of SCREEN_DESCRIPTION `sc`, consumed by
# nt_hosts.c. The UI trace does not enter any nt_graph rendering path.
add_library(ntdos64-opennt-host-applinit-ui-state-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/softpc.new/host/src/nt_graph.c)
# This compiles the original event owner with stop-only checkpoints for the
# host_main trace. It must not be used as a runtime event-loop implementation.
add_library(ntdos64-opennt-host-applinit-event-trace-overlay-objects OBJECT EXCLUDE_FROM_ALL
    overlay/base/mvdm/softpc.new/host/src/nt_event.c)

# Preserve the original process entry separately from the broad support
# provider group so its first host boundary can be observed without manually
# replaying main.c's startup sequence.
add_library(ntdos64-opennt-host-main-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/softpc.new/base/support/main.c)
# This preserves the original main.c body and adds post-host_applInit stops.
add_library(ntdos64-opennt-host-main-trace-overlay-objects OBJECT EXCLUDE_FROM_ALL
    overlay/base/mvdm/softpc.new/base/support/main.c)
# xt.c is the historical owner of main.c's verbose global. Keep it separate
# from the broad support-provider group, which also owns another main.c.
add_library(ntdos64-opennt-host-main-xt-state-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/softpc.new/base/support/xt.c)
# Original mouse-driver initialization and its per-VM data allocator owner.
# These do not admit a host mouse/input backend.
add_library(ntdos64-opennt-host-main-mouse-init-objects OBJECT EXCLUDE_FROM_ALL
    overlay/base/mvdm/softpc.new/base/keymouse/mouse_io.c)
add_library(ntdos64-opennt-host-main-virtual-state-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/softpc.new/base/bios/virtual.c)
# Isolated original config.c build for the host_main composition trace only.
add_library(ntdos64-opennt-host-main-config-trace-objects OBJECT EXCLUDE_FROM_ALL
    overlay/base/mvdm/softpc.new/host/src/config.c)

add_executable(ntdos64-opennt-host-localalloc-abi-fixture EXCLUDE_FROM_ALL
    local/trace/fixtures/host_localalloc_abi_fixture.c)

add_executable(ntdos64-opennt-host-main-prefix-trace-fixture EXCLUDE_FROM_ALL
    local/trace/fixtures/host_main_prefix_trace_fixture.c
    $<TARGET_OBJECTS:ntdos64-opennt-host-main-objects>)
target_link_libraries(ntdos64-opennt-host-main-prefix-trace-fixture PRIVATE
    msvcrt.lib ucrt.lib legacy_stdio_definitions.lib)
# main.c's later historical startup owners remain deliberately unresolved in
# this prefix-only target. host_applInit longjmps at the first boundary, so no
# unresolved call is reached and no local replacement is admitted.
target_link_options(ntdos64-opennt-host-main-prefix-trace-fixture PRIVATE
    /FORCE:UNRESOLVED)

# This composes two original process functions. The checkpoint belongs to the
# trace overlay of the original host_applInit body; the local fixture does not
# supply a host_applInit replacement.
add_executable(ntdos64-opennt-host-main-applinit-trace-fixture EXCLUDE_FROM_ALL
    local/trace/fixtures/host_main_applinit_trace_fixture.c
    local/trace/fixtures/host_console_last_event_shim.c
    # Trace-only replacement for the obsolete PEB ConsoleHandle title token.
    local/compat/host/direct_cli_console_identity.c
    $<TARGET_OBJECTS:ntdos64-opennt-host-main-trace-overlay-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-main-xt-state-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-main-mouse-init-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-main-virtual-state-objects>
    # Reuse the separately configured original nt_unix.c owner of host_memset.
    $<TARGET_OBJECTS:ntdos64-opennt-host-idle-lifecycle-objects>
    # gfi_init's original callback and empty-drive owners; no media backend.
    $<TARGET_OBJECTS:ntdos64-opennt-host-main-config-trace-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-gfi-config-trace-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-gfi-empty-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-applinit-trace-overlay-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-reset-trace-overlay-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-applinit-eoi-trace-overlay-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-applinit-ui-trace-overlay-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-applinit-ui-state-objects>
    # nt_timer.c is the original owner of ThreadInfo. The trace does not call
    # host_timer_init or resume its heartbeat thread.
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-host-timer-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-applinit-event-trace-overlay-objects>)
target_compile_definitions(ntdos64-opennt-host-main-applinit-trace-fixture PRIVATE
    NTDOS64_HOST_APPLINIT_TRACE)
target_link_libraries(ntdos64-opennt-host-main-applinit-trace-fixture PRIVATE
    ntdll.lib msvcrt.lib ucrt.lib legacy_stdio_definitions.lib)
target_link_options(ntdos64-opennt-host-main-applinit-trace-fixture PRIVATE
    /FORCE:UNRESOLVED)

add_executable(ntdos64-opennt-host-applinit-prefix-trace-fixture EXCLUDE_FROM_ALL
    local/trace/fixtures/host_applinit_trace_fixture.c
    $<TARGET_OBJECTS:ntdos64-opennt-host-applinit-trace-overlay-objects>
    # reset.c is the original owner of the three SoftPC working-function
    # pointers assigned by host_applInit. Its reset() path is never called.
    $<TARGET_OBJECTS:ntdos64-opennt-reset-trace-overlay-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-applinit-eoi-trace-overlay-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-applinit-ui-trace-overlay-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-host-applinit-ui-state-objects>
    # Original owner of the suspended event-thread lifecycle. The trace exits
    # before any resume; unresolved late event-loop dependencies remain
    # diagnostics and receive no local substitute.
    $<TARGET_OBJECTS:ntdos64-opennt-event-host-objects>)
target_compile_definitions(ntdos64-opennt-host-applinit-prefix-trace-fixture PRIVATE
    NTDOS64_HOST_APPLINIT_TRACE)
target_link_libraries(ntdos64-opennt-host-applinit-prefix-trace-fixture PRIVATE
    # nt_eoi.c uses the original ntdll RTL critical-section entry point.
    # This is a historical host dependency, not a local lock replacement.
    ntdll.lib msvcrt.lib ucrt.lib legacy_stdio_definitions.lib)
# Any unresolved late-process dependencies are intentional diagnostics: the
# fixture longjmps at an explicit lifecycle checkpoint and must not grow local
# substitutes.
target_link_options(ntdos64-opennt-host-applinit-prefix-trace-fixture PRIVATE
    /FORCE:UNRESOLVED)

# Run original reset through the historical provider archive. The trace overlay
# owns reset(), so every other device action remains historically owned.
add_executable(ntdos64-opennt-session-byob-reset-trace-fixture EXCLUDE_FROM_ALL
    local/archived/legacy-adapters/ccpu_monitor_sm0_foundation.c
    local/trace/fixtures/config_prefix_stop_support.c
    local/trace/fixtures/config_prefix_pif_guard.c
    local/trace/fixtures/config_prefix_trace_fixture.c
    local/compat/host/direct_cli_command_transport.c
    local/compat/host/direct_cli_console_identity.c
    local/compat/host/byob_ntio_path.c
    $<TARGET_OBJECTS:ntdos64-opennt-config-x86-prefix-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-umb-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-gfi-config-trace-objects>
    # gfi_init() selects the original empty-floppy server for both drives.
    # Keep that historical server in this reset-only composition so an
    # unresolved function-table initializer cannot be mistaken for a SAS or
    # reset failure.
    $<TARGET_OBJECTS:ntdos64-opennt-gfi-empty-objects>
    # The original host startup sequence initializes the original ICA lock
    # before GFI/config. Retain nt_eoi.c as its sole owner; this trace does
    # not replace the lock or admit an event/IRQ policy.
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-host-eoi-objects>
    # cmosnt.c::rtc_init() obtains its timestamp through host_GetSysTime().
    # nt_timer.c is the original host owner of that call.  Select it here
    # rather than providing a trace clock or a replacement CMOS/RTC policy.
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-host-timer-objects>
    # com.c reaches the historical Windows serial owner while reset initializes
    # its controller table. Keep nt_com.c as that owner; do not substitute a
    # no-device COM shim for this trace.
    $<TARGET_OBJECTS:ntdos64-opennt-comms-native-host-objects>
    # diskbios.c reaches ccpusas4.c through wstatus() during the pre-reset
    # disk setup. The archive cannot be relied on to extract that owner from
    # this forced-unresolved trace link, so include the same original object
    # explicitly rather than adding an SAS forwarding wrapper.
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-fpu-link-objects>
    # vga_init() registers its direct-CPU output hook through the original
    # CCPU forwarding/stub translation unit.  Keep ntstubs.c as that source
    # owner instead of introducing a second video-port registration path.
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-ntstubs-objects>
    # init_vga_globals() reaches the original CCPU video-global accessor
    # owner. C_VIDC ev_glue.c below owns the live workspace; do not select
    # the archived localfm storage adapter.
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-vglob-objects>
    # Keep diskbios.c direct so its internal helpers remain with the original
    # source under /FORCE:UNRESOLVED, while GFI retains its separate owners.
    $<TARGET_OBJECTS:ntdos64-opennt-diskbios-provider-objects>
    # Retain the original Windows floppy/fixed-disk backend as the owner of
    # GFI and host_fdisk calls. It is linked only into this default-disabled
    # trace; no local media policy or disk implementation is substituted.
    $<TARGET_OBJECTS:ntdos64-opennt-disk-host-backend-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-session-byob-reset-trace-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-reset-trace-overlay-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-host-sas-x86-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-pif-host-objects>)
target_compile_definitions(ntdos64-opennt-session-byob-reset-trace-fixture PRIVATE
    CCPU_SM2_USE_HISTORICAL_BIOS
    CCPU_SM2_USE_HISTORICAL_EOI
    CCPU_SM2_USE_HISTORICAL_TIMER
    CCPU_SM0_USE_ORIGINAL_CONFIG
    NTDOS64_RESET_TRACE
    # Must match the separately compiled reset.c trace object below.
    NTDOS64_RESET_TRACE_STOP_STAGE=${NTDOS64_RESET_TRACE_STOP_STAGE}
    # Preserve obj.vdm/ntvdm.c's pre-host_main TimerInit() ordering.  This
    # creates timer synchronization state and initializes the original
    # performance-counter scale; it does not start the heartbeat thread.
    NTDOS64_SESSION_TRACE_TIMER_INIT
    NTDOS64_TRACE_PRINT_STOP_REASON
    NTDOS64_SESSION_TRACE_ICA_INIT
    NTDOS64_SESSION_TRACE_GFI_INIT
    NTDOS64_SESSION_TRACE_CPU_INIT
    NTDOS64_SESSION_TRACE_SAS_SNAPSHOT
    NTDOS64_SESSION_TRACE_CVIDC_WORKSPACE_INIT
    CONFIG_PREFIX_TRACE_SAS_INIT
    CONFIG_PREFIX_TRACE_SESSION_INIT
    NTDOS64_SESSION_TRACE_VINT
    NTDOS64_SESSION_TRACE_VINT_STOP_STAGE=${NTDOS64_SESSION_TRACE_VINT_STOP_STAGE}
    CONFIG_PREFIX_TRACE_SESSION_VINT
    CONFIG_PREFIX_TRACE_SESSION_AFTER_RESET
    CONFIG_PREFIX_USE_HISTORICAL_PIF_FASTPASTE
    CONFIG_PREFIX_USE_HISTORICAL_DEM
    CONFIG_PREFIX_USE_HISTORICAL_NT_BOP
    CONFIG_PREFIX_USE_ORIGINAL_UMB
    CONFIG_PREFIX_USE_ORIGINAL_XMS
    CONFIG_PREFIX_USE_ORIGINAL_GFI
    CONFIG_PREFIX_USE_HISTORICAL_GFI_EMPTY
    # Select original Sim32's x86 host-SAS aperture for this trace. The
    # archived foundation retains only its remaining fail-closed callbacks.
    host_sas_init=ntdos64_trace_aperture_host_sas_init_unused
    host_sas_term=ntdos64_trace_aperture_host_sas_term_unused
    # The support fixture retains these only for older bounded traces.  Rename
    # them here so original nt_rflop/nt_fdisk own the live session callbacks.
    FloppyTerminatePDB=ntdos64_trace_floppy_terminate_unused
    FdiskTerminatePDB=ntdos64_trace_fdisk_terminate_unused
    host_using_fdisk=ntdos64_trace_using_fdisk_unused
    host_fdisk_valid=ntdos64_trace_fdisk_valid_unused
    host_fdisk_change=ntdos64_trace_fdisk_change_unused
    host_fdisk_active=ntdos64_trace_fdisk_active_unused
    CONFIG_PREFIX_USE_MODERN_MEDIA_CAPABILITY)
target_include_directories(ntdos64-opennt-session-byob-reset-trace-fixture PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/windows/inc)
target_link_libraries(ntdos64-opennt-session-byob-reset-trace-fixture PRIVATE
    ntdos64-opennt-ccpu-archive
    # base/support/main.c establishes this original CCPU/CVIDC workspace
    # before host_main. The trace repeats only those two original calls.
    ntdos64-opennt-cvidc-archive
    ntdos64-opennt-ccpu-platform-provider-archive
    ntdos64-opennt-historical-provider-archive
    ntdll.lib
    msvcrt.lib ucrt.lib legacy_stdio_definitions.lib)
# Keep this bounded reset trace honest: unresolved later-session callbacks are
# emitted as linker traps instead of being replaced by local DOS/device logic.
target_link_options(ntdos64-opennt-session-byob-reset-trace-fixture PRIVATE
    /FORCE:UNRESOLVED
    # This fixture maps forced-unresolved traps through its PDB. Disable
    # incremental linking so section contributions remain authoritative.
    /INCREMENTAL:NO)

# Trace-only bisection of the original startup prefix. Set
# NTDOS64_STARTUP_PREFIX_TRACE_STOP_STAGE to 1..4 at launch to stop before
# gfi_init, after gfi_init, after config, or after c_cpu_init respectively.
# It supplies no startup/device/DOS result and is excluded from normal builds.
add_executable(ntdos64-opennt-session-startup-boundary-fixture EXCLUDE_FROM_ALL
    local/archived/legacy-adapters/ccpu_monitor_sm0_foundation.c
    local/trace/fixtures/config_prefix_stop_support.c
    local/trace/fixtures/config_prefix_pif_guard.c
    local/trace/fixtures/config_prefix_trace_fixture.c
    local/compat/host/direct_cli_command_transport.c
    local/compat/host/direct_cli_console_identity.c
    local/trace/fixtures/media_capability_absent.c
    $<TARGET_OBJECTS:ntdos64-opennt-config-x86-prefix-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-umb-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-gfi-config-trace-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-gfi-empty-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-pif-host-objects>)
target_compile_definitions(ntdos64-opennt-session-startup-boundary-fixture PRIVATE
    CCPU_SM0_USE_ORIGINAL_CONFIG
    NTDOS64_STARTUP_PREFIX_TRACE
    NTDOS64_SESSION_TRACE_ICA_INIT
    NTDOS64_SESSION_TRACE_GFI_INIT
    NTDOS64_SESSION_TRACE_CPU_INIT
    CONFIG_PREFIX_TRACE_SAS_INIT
    CONFIG_PREFIX_TRACE_SESSION_INIT
    CONFIG_PREFIX_USE_HISTORICAL_PIF_FASTPASTE
    CONFIG_PREFIX_USE_ORIGINAL_UMB
    CONFIG_PREFIX_USE_ORIGINAL_XMS
    CONFIG_PREFIX_USE_ORIGINAL_GFI
    CONFIG_PREFIX_USE_HISTORICAL_GFI_EMPTY
    CONFIG_PREFIX_USE_MODERN_MEDIA_CAPABILITY)
target_include_directories(ntdos64-opennt-session-startup-boundary-fixture PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/windows/inc)
target_link_libraries(ntdos64-opennt-session-startup-boundary-fixture PRIVATE
    ntdos64-opennt-ccpu-archive
    ntdos64-opennt-ccpu-platform-provider-archive
    msvcrt.lib ucrt.lib legacy_stdio_definitions.lib)
target_link_options(ntdos64-opennt-session-startup-boundary-fixture PRIVATE
    /FORCE:UNRESOLVED)

# This fixture never calls configuration code. Direct object inclusion makes
# every original external reference a link-time diagnostic instead of silently
# dropping config/resource ownership from the historical closure.
add_executable(ntdos64-opennt-config-resource-link-fixture EXCLUDE_FROM_ALL
    local/trace/fixtures/config_resource_link_fixture.c
    local/compat/host/direct_cli_command_transport.c
    local/compat/host/direct_cli_console_identity.c
    $<TARGET_OBJECTS:ntdos64-opennt-config-resource-objects>
    # `config.c` delegates PIF defaults/parsing to this original owner. Keep
    # it in the diagnostic so GetPIFData is not mistaken for a modern seam.
    $<TARGET_OBJECTS:ntdos64-opennt-pif-host-objects>)
target_include_directories(ntdos64-opennt-config-resource-link-fixture PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc)

add_library(ntdos64-opennt-xms-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/xms.486/xms.c
    base/mvdm/xms.486/xmsa20.c
    overlay/base/mvdm/xms.486/xmsblock.c
    base/mvdm/xms.486/i386/xmsmem86.c
    base/mvdm/xms.486/xmsdisp.c
    base/mvdm/xms.486/xmsmisc.c
    base/mvdm/xms.486/xmsumb.c)

# This fixture does not call XMS. Direct object inclusion lets the linker
# enumerate historical dependencies without admitting an XMS replacement.
add_executable(ntdos64-opennt-xms-link-fixture EXCLUDE_FROM_ALL
    local/trace/fixtures/xms_link_fixture.c
    $<TARGET_OBJECTS:ntdos64-opennt-xms-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-suballoc-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-umb-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-keymouse-provider-objects>)

# The historical suballocator owns XMS block accounting. Keep its original
# source unit independently auditable before any aggregate link experiment.
add_library(ntdos64-opennt-suballoc-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/suballoc/suballoc.c)

# ReserveUMB and the UMB ownership list remain in their historical NT host
# owner. Compile it independently before any measured aggregate link.
add_library(ntdos64-opennt-umb-provider-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/softpc.new/host/src/nt_umb.c)

# The generic floppy router is independent of the historical direct-drive
# backend. Keep it separately observable at the media-host boundary.
add_library(ntdos64-opennt-gfi-config-trace-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/softpc.new/base/disks/gfi.c)

# `gfi_init()` owns the initial empty-server table population. Keep that
# historical owner separate from the router and admit it only to the startup
# boundary trace that actually reaches gfi_init.
add_library(ntdos64-opennt-gfi-empty-objects OBJECT EXCLUDE_FROM_ALL
    base/mvdm/softpc.new/base/disks/gfi_mpty.c)

# Isolate the original pre-config `gfi_init()` call from config(), CCPU, BIOS,
# BOP and DOS. The fixture records the one historical host-runtime request;
# it never supplies media semantics or a runtime device policy.
add_executable(ntdos64-opennt-gfi-init-trace-fixture EXCLUDE_FROM_ALL
    local/trace/fixtures/gfi_init_trace_fixture.c
    $<TARGET_OBJECTS:ntdos64-opennt-gfi-config-trace-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-gfi-empty-objects>)
# `gfi.c` also owns inactive real-drive entry points. Preserve them as an
# unresolved diagnostic rather than fabricating a disk backend; this fixture
# invokes only `gfi_init()` and /OPT:REF discards those unreachable sections.
target_link_options(ntdos64-opennt-gfi-init-trace-fixture PRIVATE
    /OPT:REF /FORCE:UNRESOLVED)

# Same bounded GFI call, but resolve its one historical host callback through
# the NT config owner. This distinguishes an original-owner problem from the
# wider startup fixture without admitting config() or a disk backend.
add_executable(ntdos64-opennt-gfi-init-config-host-trace-fixture EXCLUDE_FROM_ALL
    local/trace/fixtures/gfi_init_trace_fixture.c
    $<TARGET_OBJECTS:ntdos64-opennt-config-x86-prefix-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-gfi-config-trace-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-gfi-empty-objects>)
target_compile_definitions(ntdos64-opennt-gfi-init-config-host-trace-fixture PRIVATE
    NTDOS64_GFI_USE_HISTORICAL_HOST_RUNTIME)
target_link_options(ntdos64-opennt-gfi-init-config-host-trace-fixture PRIVATE
    /OPT:REF /FORCE:UNRESOLVED)

# Minimal control-flow counterpart to the startup stage-2 probe. It isolates
# original gfi_init followed by the trace checkpoint/longjmp only; it has no
# PIF, command transport, title, config(), CPU, BIOS, BOP, DEM or DOS path.
add_executable(ntdos64-opennt-gfi-checkpoint-trace-fixture EXCLUDE_FROM_ALL
    local/trace/fixtures/config_prefix_stop_support.c
    local/trace/fixtures/gfi_checkpoint_trace_fixture.c
    $<TARGET_OBJECTS:ntdos64-opennt-config-x86-prefix-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-gfi-config-trace-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-gfi-empty-objects>)
target_compile_definitions(ntdos64-opennt-gfi-checkpoint-trace-fixture PRIVATE
    NTDOS64_STARTUP_PREFIX_TRACE
    CONFIG_PREFIX_TRACE_SAS_INIT
    CONFIG_PREFIX_USE_ORIGINAL_GFI
    CONFIG_PREFIX_USE_HISTORICAL_GFI_EMPTY)
target_link_options(ntdos64-opennt-gfi-checkpoint-trace-fixture PRIVATE
    /OPT:REF /FORCE:UNRESOLVED)

# Hold the complete startup fixture's non-config object set constant while
# calling only original gfi_init(). If this returns, the remaining stage-2
# problem is in the trace checkpoint/setjmp harness rather than GFI linkage.
add_executable(ntdos64-opennt-gfi-init-startup-set-trace-fixture EXCLUDE_FROM_ALL
    local/archived/legacy-adapters/ccpu_monitor_sm0_foundation.c
    local/trace/fixtures/config_prefix_stop_support.c
    local/trace/fixtures/config_prefix_pif_guard.c
    local/trace/fixtures/gfi_init_trace_fixture.c
    local/compat/host/direct_cli_command_transport.c
    local/compat/host/direct_cli_console_identity.c
    local/trace/fixtures/media_capability_absent.c
    $<TARGET_OBJECTS:ntdos64-opennt-config-x86-prefix-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-umb-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-gfi-config-trace-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-gfi-empty-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-pif-host-objects>)
target_compile_definitions(ntdos64-opennt-gfi-init-startup-set-trace-fixture PRIVATE
    CCPU_SM0_USE_ORIGINAL_CONFIG
    NTDOS64_GFI_USE_HISTORICAL_HOST_RUNTIME
    CONFIG_PREFIX_TRACE_SAS_INIT
    CONFIG_PREFIX_TRACE_SESSION_INIT
    CONFIG_PREFIX_USE_HISTORICAL_PIF_FASTPASTE
    CONFIG_PREFIX_USE_ORIGINAL_UMB
    CONFIG_PREFIX_USE_ORIGINAL_XMS
    CONFIG_PREFIX_USE_ORIGINAL_GFI
    CONFIG_PREFIX_USE_HISTORICAL_GFI_EMPTY
    CONFIG_PREFIX_USE_MODERN_MEDIA_CAPABILITY)
target_include_directories(ntdos64-opennt-gfi-init-startup-set-trace-fixture PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/windows/inc)
target_link_libraries(ntdos64-opennt-gfi-init-startup-set-trace-fixture PRIVATE
    ntdos64-opennt-ccpu-archive
    ntdos64-opennt-ccpu-platform-provider-archive
    msvcrt.lib ucrt.lib legacy_stdio_definitions.lib)
target_link_options(ntdos64-opennt-gfi-init-startup-set-trace-fixture PRIVATE
    /OPT:REF /FORCE:UNRESOLVED)

# Link-selected aggregation of historical monitor/BIOS/PC-AT providers. It
# deliberately excludes local contained facades, disk host backends, OEM file
# facades, frozen trace bridges, and every future modern-host shim. Archive
# extraction preserves historical ownership boundaries while exposing the
# actual remaining host dependencies to the SH-10 fixture.
add_library(ntdos64-opennt-historical-provider-archive STATIC EXCLUDE_FROM_ALL
    $<TARGET_OBJECTS:ntdos64-opennt-host-stack-objects>
    # nt_bop.c begins with OpenNT native headers and is compiled in its
    # independent native-first object target; retain the same link content
    # without forcing it through the DEM-oriented host-stack preamble.
    $<TARGET_OBJECTS:ntdos64-opennt-nt-bop-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-ccpu-platform-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-bios-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-system-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-support-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-keymouse-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-disks-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-comms-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-video-provider-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-xms-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-suballoc-objects>
    $<TARGET_OBJECTS:ntdos64-opennt-umb-provider-objects>)

function(ntdos64_configure_opennt_target target)
    set(options WITHOUT_CPU_40_STYLE)
    cmake_parse_arguments(NTDOS64_OPENNT "${options}" "" "" ${ARGN})

    set(ntdos64_opennt_profile_definitions
        NTVDM CCPU PROD DEVL=1 SPC386 NEW_CPU ANSI i386)
    if(NOT NTDOS64_OPENNT_WITHOUT_CPU_40_STYLE)
        list(APPEND ntdos64_opennt_profile_definitions CPU_40_STYLE)
    endif()

    target_compile_definitions(${target} PRIVATE
        # This compile-only closure probes the portable CCPU source form. It
        # is not the original NT4 x86 product profile, which CDEFINE.INC
        # selects as MONITOR/C_VID/X86GFX. In particular, do not define
        # GISP_CPU: it redirects BIOS selector FB to hg_bop_handler and would
        # confound this non-V86 source-closure measurement.
        ${ntdos64_opennt_profile_definitions})
    target_include_directories(${target} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/overlay/base/mvdm/softpc.new/base/inc
        ${CMAKE_CURRENT_SOURCE_DIR}/local/include
        ${CMAKE_CURRENT_SOURCE_DIR}/base/mvdm/dos/dem
        ${CMAKE_CURRENT_SOURCE_DIR}/overlay/base/mvdm/inc
        ${CMAKE_CURRENT_SOURCE_DIR}/base/mvdm/inc
        ${CMAKE_CURRENT_SOURCE_DIR}/base/mvdm/softpc.new/base/ccpu386
        ${CMAKE_CURRENT_SOURCE_DIR}/base/mvdm/softpc.new/base/inc
        ${CMAKE_CURRENT_SOURCE_DIR}/base/mvdm/softpc.new/host/inc
        ${CMAKE_CURRENT_SOURCE_DIR}/base/mvdm/softpc.new/host/inc/mips/prod
        ${CMAKE_CURRENT_SOURCE_DIR}/public/sdk/inc)

    if(target STREQUAL "ntdos64-opennt-cvidc-objects" OR
       target STREQUAL "ntdos64-opennt-cvidc-generic-video-vector-objects" OR
       target STREQUAL "ntdos64-opennt-reconstructed-cvidc-video-tail-objects")
        target_include_directories(${target} PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/base/mvdm/softpc.new/base/cvidc)
    endif()

    # The x86 SAS header is a separately manifested, patch-derived input for
    # CCPU and the original BIOS provider units that share CCPU's SAS layout.
    # The host-stack gate keeps its already verified header route.
    if(target STREQUAL "ntdos64-opennt-ccpu-objects" OR
       target STREQUAL "ntdos64-opennt-ccpu-fpu-link-objects" OR
       target STREQUAL "ntdos64-opennt-bios-provider-objects" OR
       target STREQUAL "ntdos64-opennt-system-provider-objects" OR
       target STREQUAL "ntdos64-opennt-keymouse-provider-objects" OR
       target STREQUAL "ntdos64-opennt-disks-provider-objects" OR
       target STREQUAL "ntdos64-opennt-diskbios-provider-objects" OR
       target STREQUAL "ntdos64-opennt-comms-provider-objects")
        target_include_directories(${target} BEFORE PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/overlay/base/mvdm/softpc.new/host/inc/x86/prod
            # ica.h is a fixed historical CCPU input and includes vdm.h.
            # Keep that declaration-only MVDM include visible to the same
            # CCPU/provider compile island; it supplies no runtime behavior.
            ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc)
    endif()

    # nt_fdisk/nt_rflop include the historical NT umbrella header directly.
    # Select the fixed OpenNT public declaration set before the minimal staging
    # compatibility headers, but only in their isolated object gate.
    if(target STREQUAL "ntdos64-opennt-disk-host-backend-objects")
        target_include_directories(${target} BEFORE PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/public/ddk/inc
            ${CMAKE_CURRENT_SOURCE_DIR}/public/sdk/inc)
        target_compile_options(${target} PRIVATE
            /FI${CMAKE_CURRENT_SOURCE_DIR}/local/compat/compiler/opennt_disk_ntddstor_compat.h)
    endif()

    if(MSVC)
        target_compile_options(${target} PRIVATE
        # OpenNT CCPU predates C99 and relies on its historical declaration
        # ordering.  This is a parser-compatibility profile only; it does not
        # supply substitute CCPU behavior.
        /clang:-std=gnu89
        /clang:-Wno-error=implicit-function-declaration
        /clang:-Wno-error=incompatible-pointer-types
        /clang:-Wno-error=return-type
        /FI${CMAKE_CURRENT_SOURCE_DIR}/local/compat/compiler/opennt_stage_crt_preamble.h)

        # nt_fdisk/nt_rflop must establish the fixed historical native API
        # declarations before their own later windows.h include.
        if(NOT target STREQUAL "ntdos64-opennt-disk-host-backend-objects" AND
           NOT target STREQUAL "ntdos64-opennt-dpmi32-objects" AND
           NOT target STREQUAL "ntdos64-opennt-dbg-objects" AND
           NOT target STREQUAL "ntdos64-opennt-oemuni-provider-objects" AND
           NOT target STREQUAL "ntdos64-opennt-host-applinit-trace-overlay-objects" AND
           NOT target STREQUAL "ntdos64-opennt-host-applinit-eoi-trace-overlay-objects" AND
           NOT target STREQUAL "ntdos64-opennt-host-applinit-event-trace-overlay-objects" AND
            NOT target STREQUAL "ntdos64-opennt-host-main-config-trace-objects" AND
            NOT target STREQUAL "ntdos64-opennt-host-copy-objects" AND
            NOT target STREQUAL "ntdos64-opennt-host-sound-objects" AND
            NOT target STREQUAL "ntdos64-opennt-comms-native-host-objects" AND
            # COMMAND's cmd.h establishes nt.h/ntrtl/nturtl before windows.h
            # when WIN_32 is selected.
            NOT target STREQUAL "ntdos64-opennt-command-objects" AND
            NOT target STREQUAL "ntdos64-opennt-session-host-objects" AND
            NOT target STREQUAL "ntdos64-opennt-event-host-objects" AND
            NOT target STREQUAL "ntdos64-opennt-reset-shell-objects" AND
            NOT target STREQUAL "ntdos64-opennt-error-host-objects" AND
            NOT target STREQUAL "ntdos64-opennt-host-keycode-nls-objects" AND
            NOT target STREQUAL "ntdos64-opennt-x86-emm-host-objects" AND
            NOT target STREQUAL "ntdos64-opennt-xms-objects" AND
            NOT target STREQUAL "ntdos64-opennt-suballoc-objects" AND
           # qevnt.c in the CCPU platform-provider group establishes the
           # native NT declaration order before windows.h itself.
           NOT target STREQUAL "ntdos64-opennt-ccpu-platform-provider-objects" AND
           # nt_umb.c likewise establishes the fixed native declaration set
           # before including windows.h.
           NOT target STREQUAL "ntdos64-opennt-umb-provider-objects" AND
           # nt_bop.c begins with the retained native NT headers, then
           # includes windows.h and vdm.h in its original order.
           NOT target STREQUAL "ntdos64-opennt-nt-bop-objects" AND
           # nt_eoi.c establishes the native NT declaration order before
           # including windows.h; its CCPU-specific object gate must match.
           NOT target STREQUAL "ntdos64-opennt-ccpu-host-eoi-objects" AND
            # nt_cpu.c has the same native nt.h -> ntrtl.h -> nturtl.h
            # preamble.  Its CCPU wrapper branch must not receive the generic
            # Win32-first staging header before that original declaration order.
            NOT target STREQUAL "ntdos64-opennt-ccpu-host-cpu-objects" AND
            # nt_inthk.c likewise establishes the native headers itself; the
            # original CCPU hook owner must retain that declaration order.
            NOT target STREQUAL "ntdos64-opennt-ccpu-host-inthk-objects" AND
            # The original SAS allocation pair has the same native header
            # ordering and must not receive a Win32-first forced include.
            NOT target STREQUAL "ntdos64-opennt-ccpu-host-sas-objects" AND
            # This owner-only nt_mem.c variant preserves the same native
            # declaration order while the no-SM0 closure supplies sim32.c.
            NOT target STREQUAL "ntdos64-opennt-ccpu-host-intel-memory-owner-objects" AND
           # cmosnt.c is the native NT CMOS owner and establishes this order
           # before including windows.h.
           NOT target STREQUAL "ntdos64-opennt-system-provider-objects" AND
           # config.c establishes the historical native-NT declaration order
           # itself. Like the host-main copy, it must not be preceded by the
           # Win32-first staging preamble.
           NOT target STREQUAL "ntdos64-opennt-config-resource-objects" AND
           NOT target STREQUAL "ntdos64-opennt-config-prefix-objects" AND
           NOT target STREQUAL "ntdos64-opennt-config-x86-prefix-objects" AND
           # nt_msscs.c also begins with the native OpenNT declaration chain.
           # Its session variants must not receive a Win32-first forced header.
           NOT target STREQUAL "ntdos64-opennt-session-host-objects" AND
           NOT target STREQUAL "ntdos64-opennt-session-byob-objects" AND
           NOT target STREQUAL "ntdos64-opennt-session-byob-trace-objects" AND
           NOT target STREQUAL "ntdos64-opennt-session-byob-io-trace-objects" AND
           NOT target STREQUAL "ntdos64-opennt-session-byob-reset-trace-objects" AND
           # nt_timer.c establishes the historical native-NT declaration
           # order itself before including windows.h.  Pre-including
           # windows.h would make OpenNT's ntdef.h collide with winnt.h.
           NOT target STREQUAL "ntdos64-opennt-ccpu-host-timer-objects" AND
           # Both historical host/src inventory targets must retain each
           # source unit's own native-NT or Win32 declaration order.
           NOT target STREQUAL "ntdos64-opennt-historical-host-portable-inventory-objects" AND
           NOT target STREQUAL "ntdos64-opennt-historical-host-native-inventory-objects" AND
           # SIM32 + CPU_40_STYLE starts with the same native NT headers,
           # then includes windows.h itself. Keep that original order rather
           # than forcing the generic Win32-first staging preamble.
           NOT target STREQUAL "ntdos64-opennt-sim32-objects" AND
           NOT target STREQUAL "ntdos64-opennt-sim32-owner-closure-objects" AND
           NOT target STREQUAL "ntdos64-opennt-ccpu-host-sas-objects")
            target_compile_options(${target} PRIVATE
                /FI${CMAKE_CURRENT_SOURCE_DIR}/local/compat/compiler/opennt_stage_preamble.h)
        endif()

        if(target STREQUAL "ntdos64-opennt-host-stack-objects" OR
           target STREQUAL "ntdos64-opennt-dem-objects" OR
           target STREQUAL "ntdos64-opennt-dem-ccpu-sas-objects" OR
           target STREQUAL "ntdos64-opennt-demerror-register-objects" OR
           target STREQUAL "ntdos64-opennt-demlock-register-objects")
            target_compile_options(${target} PRIVATE
                /FI${CMAKE_CURRENT_SOURCE_DIR}/local/compat/compiler/opennt_dem_ntdecl_compat.h
                # DEM keeps VDMREDIR dispatch pointers as C tentative globals;
                # nt_bop.c supplies their initialized owner definitions. Use
                # the historical common-symbol behavior rather than treating
                # those declarations as modern strong duplicate definitions.
                /clang:-fcommon)
        endif()

    endif()

    set_target_properties(${target} PROPERTIES
        EXCLUDE_FROM_ALL TRUE
        FOLDER historical-inputs)
endfunction()

ntdos64_configure_opennt_target(ntdos64-opennt-ccpu-objects)
ntdos64_configure_opennt_target(ntdos64-opennt-ccpu-fpu-link-objects)
ntdos64_configure_opennt_target(ntdos64-opennt-ccpu-ntstubs-objects)
# `ccpu386/ccpudefs.inc` is the authoritative CCPU compilation profile.  It
# is separate from obj.vdm's native x86 MONITOR selection: the software CCPU
# and its generated C_VIDC companion require this shared interface layout.
target_compile_definitions(ntdos64-opennt-ccpu-objects PRIVATE
    CPU_30_STYLE)
target_compile_definitions(ntdos64-opennt-ccpu-fpu-link-objects PRIVATE
    CPU_30_STYLE)
target_compile_definitions(ntdos64-opennt-ccpu-ntstubs-objects PRIVATE
    CPU_30_STYLE)
# ntstubs.c is an original whole translation unit. Its three no-op fallback
# exports collide with the selected software-FPU profile; rename only those
# unused fallbacks in this closure-only object so all SAS forwarding remains
# original and normal calls still resolve to their FPU/archive owners.
target_compile_definitions(ntdos64-opennt-ccpu-ntstubs-objects PRIVATE
    copyROM=ntdos64_ntstubs_copyROM_unused
    initialise_npx=ntdos64_ntstubs_initialise_npx_unused
    npx_reset=ntdos64_ntstubs_npx_reset_unused
    # C_VIDC ev_glue.c is the selected historical owner of the live Gdp
    # workspace. ntstubs.c never reads its own storage; rename only that
    # duplicate definition while retaining its original Cpu_define_outb body.
    Gdp=ntdos64_ntstubs_Gdp_unused)
ntdos64_configure_opennt_target(ntdos64-opennt-cvidc-objects)
# C_VIDC is compiled with the same historical ccpudefs.inc profile as CCPU.
target_compile_definitions(ntdos64-opennt-cvidc-objects PRIVATE
    CPU_30_STYLE
    C_VID)
ntdos64_configure_opennt_target(ntdos64-opennt-cvidc-generic-video-vector-objects)
target_compile_definitions(ntdos64-opennt-cvidc-generic-video-vector-objects PRIVATE
    CPU_30_STYLE
    C_VID
    C_Video=Video)
set_target_properties(ntdos64-opennt-cvidc-generic-video-vector-objects PROPERTIES
    EXCLUDE_FROM_ALL TRUE
    FOLDER historical-inputs)
ntdos64_configure_opennt_target(ntdos64-opennt-reconstructed-cvidc-video-tail-objects)
target_compile_definitions(ntdos64-opennt-reconstructed-cvidc-video-tail-objects PRIVATE
    CPU_30_STYLE
    C_VID)
target_include_directories(ntdos64-opennt-reconstructed-cvidc-video-tail-objects BEFORE PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/base/mvdm/softpc.new/base/cvidc)
set_target_properties(ntdos64-opennt-reconstructed-cvidc-video-tail-objects PROPERTIES
    EXCLUDE_FROM_ALL TRUE
    FOLDER reconstruction)
set_target_properties(ntdos64-opennt-ccpu-archive PROPERTIES
    EXCLUDE_FROM_ALL TRUE
    FOLDER historical-inputs)
set_target_properties(ntdos64-opennt-cvidc-archive PROPERTIES
    EXCLUDE_FROM_ALL TRUE
    FOLDER historical-inputs)
ntdos64_configure_opennt_target(ntdos64-opennt-host-stack-objects)
target_include_directories(ntdos64-opennt-host-stack-objects PRIVATE
    # demsrch.c includes this already manifest-tracked, declaration-empty
    # stand-in for a historical private PCH header.
    ${CMAKE_CURRENT_SOURCE_DIR}/local/compat/compiler)
ntdos64_configure_opennt_target(ntdos64-opennt-ccpu-platform-provider-objects)
target_compile_definitions(ntdos64-opennt-ccpu-platform-provider-objects PRIVATE _X86_)
target_include_directories(ntdos64-opennt-ccpu-platform-provider-objects PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc)
set_target_properties(ntdos64-opennt-ccpu-platform-provider-archive PROPERTIES
    EXCLUDE_FROM_ALL TRUE
    FOLDER historical-inputs)
ntdos64_configure_opennt_target(ntdos64-opennt-ccpu-register-bridge-fixture)
target_link_options(ntdos64-opennt-ccpu-register-bridge-fixture PRIVATE /OPT:REF)
ntdos64_configure_opennt_target(ntdos64-opennt-original-register-bridge-fixture)
target_compile_definitions(ntdos64-opennt-original-register-bridge-fixture PRIVATE
    CCPU_SM2_USE_EFFECTIVE_ADDR_COMPLETION)
target_link_options(ntdos64-opennt-original-register-bridge-fixture PRIVATE /OPT:REF)
ntdos64_configure_opennt_target(ntdos64-opennt-original-demerror-register-fixture)
target_compile_definitions(ntdos64-opennt-original-demerror-register-fixture PRIVATE
    CCPU_SM2_USE_EFFECTIVE_ADDR_COMPLETION)
target_link_options(ntdos64-opennt-original-demerror-register-fixture PRIVATE /OPT:REF)
ntdos64_configure_opennt_target(ntdos64-opennt-original-demlock-register-fixture)
target_compile_definitions(ntdos64-opennt-original-demlock-register-fixture PRIVATE
    CCPU_SM2_USE_EFFECTIVE_ADDR_COMPLETION)
target_link_options(ntdos64-opennt-original-demlock-register-fixture PRIVATE /OPT:REF)
ntdos64_configure_opennt_target(ntdos64-opennt-ccpu-software-monitor-sm0-fixture)
target_link_options(ntdos64-opennt-ccpu-software-monitor-sm0-fixture PRIVATE /OPT:REF)
ntdos64_configure_opennt_target(ntdos64-opennt-ccpu-software-monitor-sm1-fixture)
target_link_options(ntdos64-opennt-ccpu-software-monitor-sm1-fixture PRIVATE /OPT:REF)
ntdos64_configure_opennt_target(ntdos64-opennt-ccpu-software-monitor-sm2-fixture)
target_link_options(ntdos64-opennt-ccpu-software-monitor-sm2-fixture PRIVATE /OPT:REF /errorlimit:0)
ntdos64_configure_opennt_target(ntdos64-opennt-original-chain-closure-fixture)
target_link_options(ntdos64-opennt-original-chain-closure-fixture PRIVATE /OPT:REF /errorlimit:0)
ntdos64_configure_opennt_target(ntdos64-opennt-original-selector50-trace-fixture)
target_link_options(ntdos64-opennt-original-selector50-trace-fixture PRIVATE
    /OPT:REF
    /FORCE:UNRESOLVED
    /errorlimit:0)
ntdos64_configure_opennt_target(ntdos64-opennt-original-bios50-date-fixture)
# BIOS[] retains the original complete selector table. /OPT:REF retains only
# the called selector-50 closure; /FORCE keeps unrelated, uncalled table
# entries as link diagnostics and must never be used as runtime closure.
target_link_options(ntdos64-opennt-original-bios50-date-fixture PRIVATE
    /OPT:REF
    /FORCE:UNRESOLVED
    /errorlimit:0)
ntdos64_configure_opennt_target(ntdos64-opennt-historical-bios-bridge-v1-fixture)
target_link_options(ntdos64-opennt-historical-bios-bridge-v1-fixture PRIVATE
    /OPT:REF
    /FORCE:UNRESOLVED
    /errorlimit:0)
ntdos64_configure_opennt_target(ntdos64-opennt-original-demload-dos-byob-fixture)
# The original BIOS table retains unrelated selectors. This fixture only
# admits its selector-50/DEMLOADDOS trace closure and is never runtime proof.
target_link_options(ntdos64-opennt-original-demload-dos-byob-fixture PRIVATE
    /OPT:REF
    /FORCE:UNRESOLVED
    /errorlimit:0)
ntdos64_configure_opennt_target(ntdos64-opennt-original-demload-dos-direct-fixture)
target_link_options(ntdos64-opennt-original-demload-dos-direct-fixture PRIVATE
    /OPT:REF
    /FORCE:UNRESOLVED
    /errorlimit:0)
ntdos64_configure_opennt_target(ntdos64-opennt-original-demgetdrives-byob-fixture)
# Keep unrelated BIOS[] table entries as link diagnostics. The called 0Fh
# closure must resolve through original source owners; this is never runtime
# closure proof.
target_link_options(ntdos64-opennt-original-demgetdrives-byob-fixture PRIVATE
    /OPT:REF
    /FORCE:UNRESOLVED
    /errorlimit:0)
ntdos64_configure_opennt_target(ntdos64-opennt-original-demgetdpblist-trace-fixture)
# Keep unrelated BIOS[] entries diagnostic-only. The admitted dynamic chain is
# the original 0Fh discovery followed by original 46h DPB writing.
target_link_options(ntdos64-opennt-original-demgetdpblist-trace-fixture PRIVATE
    /OPT:REF
    /FORCE:UNRESOLVED
    /errorlimit:0)
ntdos64_configure_opennt_target(ntdos64-opennt-original-demsetdta-trace-fixture)
# This retains the same full historical BIOS table only as an uncalled-entry
# diagnostic. The admitted 1Bh dynamic closure remains entirely original.
target_link_options(ntdos64-opennt-original-demsetdta-trace-fixture PRIVATE
    /OPT:REF
    /FORCE:UNRESOLVED
    /errorlimit:0)
ntdos64_configure_opennt_target(ntdos64-opennt-original-demsetharderror-trace-fixture)
# Same bounded full-table diagnostic policy as the preceding DosInit services.
target_link_options(ntdos64-opennt-original-demsetharderror-trace-fixture PRIVATE
    /OPT:REF
    /FORCE:UNRESOLVED
    /errorlimit:0)
ntdos64_configure_opennt_target(ntdos64-opennt-original-cmdsetinfo-trace-fixture)
# The full BIOS and COMMAND dispatch tables remain diagnostics for uncalled
# entries; the dynamic 54h/05h path uses only original owners.
target_link_options(ntdos64-opennt-original-cmdsetinfo-trace-fixture PRIVATE
    /OPT:REF
    /FORCE:UNRESOLVED
    /errorlimit:0)
ntdos64_configure_opennt_target(ntdos64-opennt-reconstructed-monitor-s4-bridge-fixture)
target_link_options(ntdos64-opennt-reconstructed-monitor-s4-bridge-fixture PRIVATE
    /OPT:REF
    /FORCE:UNRESOLVED
    /errorlimit:0)
ntdos64_configure_opennt_target(ntdos64-opennt-original-selector50-idle-trace-fixture)
target_link_options(ntdos64-opennt-original-selector50-idle-trace-fixture PRIVATE
    /OPT:REF
    /FORCE:UNRESOLVED
    /errorlimit:0)
ntdos64_configure_opennt_target(ntdos64-opennt-original-demdispatch-date-fixture)
target_link_options(ntdos64-opennt-original-demdispatch-date-fixture PRIVATE
    /OPT:REF
    /FORCE:UNRESOLVED
    /errorlimit:0)
ntdos64_configure_opennt_target(ntdos64-opennt-original-msbop0-date-fixture)
target_link_options(ntdos64-opennt-original-msbop0-date-fixture PRIVATE
    /OPT:REF
    /FORCE:UNRESOLVED
    /errorlimit:0)
ntdos64_configure_opennt_target(ntdos64-opennt-original-sim32-pointer-fixture)
target_link_options(ntdos64-opennt-original-sim32-pointer-fixture PRIVATE
    /OPT:REF
    /FORCE:UNRESOLVED
    /errorlimit:0)
ntdos64_configure_opennt_target(ntdos64-opennt-original-sas-lifecycle-trace-fixture)
target_link_options(ntdos64-opennt-original-sas-lifecycle-trace-fixture PRIVATE
    /OPT:REF
    # sim32.c/nt_mem.c retain uncalled descriptor and private-VDM imports in
    # their object closure. Keep them as link diagnostics; this bounded
    # lifecycle probe does not execute either path.
    /FORCE:UNRESOLVED
    /errorlimit:0)
ntdos64_configure_opennt_target(ntdos64-opennt-direct-ccpu-full-source-closure-fixture)
target_link_options(ntdos64-opennt-direct-ccpu-full-source-closure-fixture PRIVATE /OPT:REF /errorlimit:0)
ntdos64_configure_opennt_target(ntdos64-opennt-direct-ccpu-no-dpmi-closure-fixture)
target_link_options(ntdos64-opennt-direct-ccpu-no-dpmi-closure-fixture PRIVATE /OPT:REF /errorlimit:0)
ntdos64_configure_opennt_target(ntdos64-opennt-ccpu-cvidc-register-bridge-fixture)
target_link_options(ntdos64-opennt-ccpu-cvidc-register-bridge-fixture PRIVATE /OPT:REF)
ntdos64_configure_opennt_target(ntdos64-opennt-ccpu-effective-addr-fixture)
ntdos64_configure_opennt_target(ntdos64-opennt-direct-cli-command-transport-fixture)
ntdos64_configure_opennt_target(ntdos64-opennt-direct-cli-console-identity-fixture)
ntdos64_configure_opennt_target(ntdos64-opennt-pif-default-parser-fixture)
ntdos64_configure_opennt_target(ntdos64-opennt-config-prefix-trace-fixture)
ntdos64_configure_opennt_target(ntdos64-opennt-config-sas-prefix-trace-fixture)
target_link_options(ntdos64-opennt-ccpu-effective-addr-fixture PRIVATE /OPT:REF)
ntdos64_configure_opennt_target(ntdos64-opennt-bios-provider-objects)
ntdos64_configure_opennt_target(ntdos64-opennt-reset-trace-overlay-objects)
target_compile_definitions(ntdos64-opennt-reset-trace-overlay-objects PRIVATE
    NTDOS64_RESET_TRACE
    NTDOS64_RESET_TRACE_STOP_STAGE=${NTDOS64_RESET_TRACE_STOP_STAGE}
    _X86_)
target_include_directories(ntdos64-opennt-reset-trace-overlay-objects PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc)
ntdos64_configure_opennt_target(ntdos64-opennt-cpu-init-trace-overlay-objects)
target_compile_definitions(ntdos64-opennt-cpu-init-trace-overlay-objects PRIVATE
    NTDOS64_CPU_INIT_TRACE)
ntdos64_configure_opennt_target(ntdos64-opennt-host-applinit-trace-overlay-objects)
target_compile_definitions(ntdos64-opennt-host-applinit-trace-overlay-objects PRIVATE
    NTDOS64_HOST_APPLINIT_TRACE _X86_)
target_include_directories(ntdos64-opennt-host-applinit-trace-overlay-objects PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/windows/inc)
ntdos64_configure_opennt_target(ntdos64-opennt-host-applinit-eoi-trace-overlay-objects)
target_compile_definitions(ntdos64-opennt-host-applinit-eoi-trace-overlay-objects PRIVATE
    NTDOS64_HOST_APPLINIT_TRACE _X86_)
target_include_directories(ntdos64-opennt-host-applinit-eoi-trace-overlay-objects PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc)
ntdos64_configure_opennt_target(ntdos64-opennt-host-applinit-ui-trace-overlay-objects)
target_compile_definitions(ntdos64-opennt-host-applinit-ui-trace-overlay-objects PRIVATE
    NTDOS64_HOST_APPLINIT_TRACE)
target_include_directories(ntdos64-opennt-host-applinit-ui-trace-overlay-objects PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/windows/inc)
target_compile_options(ntdos64-opennt-host-applinit-ui-trace-overlay-objects PRIVATE
    /FI${CMAKE_CURRENT_SOURCE_DIR}/local/compat/compiler/opennt_legacy_sdk_compat.h
    /FI${CMAKE_CURRENT_SOURCE_DIR}/local/compat/compiler/opennt_host_native_io_compat.h)
ntdos64_configure_opennt_target(ntdos64-opennt-host-applinit-ui-state-objects)
target_compile_definitions(ntdos64-opennt-host-applinit-ui-state-objects PRIVATE _X86_)
target_include_directories(ntdos64-opennt-host-applinit-ui-state-objects PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/windows/inc)
target_compile_options(ntdos64-opennt-host-applinit-ui-state-objects PRIVATE
    /FI${CMAKE_CURRENT_SOURCE_DIR}/local/compat/compiler/opennt_legacy_sdk_compat.h)
ntdos64_configure_opennt_target(ntdos64-opennt-host-applinit-event-trace-overlay-objects)
target_compile_definitions(ntdos64-opennt-host-applinit-event-trace-overlay-objects PRIVATE
    NTDOS64_HOST_APPLINIT_TRACE _X86_)
target_include_directories(ntdos64-opennt-host-applinit-event-trace-overlay-objects PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/windows/inc)
target_compile_options(ntdos64-opennt-host-applinit-event-trace-overlay-objects PRIVATE
    /FI${CMAKE_CURRENT_SOURCE_DIR}/local/compat/compiler/opennt_legacy_seh_compat.h)
ntdos64_configure_opennt_target(ntdos64-opennt-host-main-objects)
ntdos64_configure_opennt_target(ntdos64-opennt-host-main-trace-overlay-objects)
target_compile_definitions(ntdos64-opennt-host-main-trace-overlay-objects PRIVATE
    NTDOS64_HOST_MAIN_TRACE)
ntdos64_configure_opennt_target(ntdos64-opennt-host-main-xt-state-objects)
ntdos64_configure_opennt_target(ntdos64-opennt-host-main-mouse-init-objects)
target_compile_definitions(ntdos64-opennt-host-main-mouse-init-objects PRIVATE
    NTDOS64_MOUSE_INIT_TRACE)
target_include_directories(ntdos64-opennt-host-main-mouse-init-objects PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc)
ntdos64_configure_opennt_target(ntdos64-opennt-host-main-virtual-state-objects)
ntdos64_configure_opennt_target(ntdos64-opennt-host-main-config-trace-objects)
target_compile_definitions(ntdos64-opennt-host-main-config-trace-objects PRIVATE
    NTDOS64_HOST_MAIN_CONFIG_TRACE _X86_)
target_include_directories(ntdos64-opennt-host-main-config-trace-objects PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/windows/inc)
ntdos64_configure_opennt_target(ntdos64-opennt-host-localalloc-abi-fixture)
ntdos64_configure_opennt_target(ntdos64-opennt-host-main-prefix-trace-fixture)
ntdos64_configure_opennt_target(ntdos64-opennt-host-main-applinit-trace-fixture)
ntdos64_configure_opennt_target(ntdos64-opennt-host-applinit-prefix-trace-fixture)
ntdos64_configure_opennt_target(ntdos64-opennt-bios-table-objects)
ntdos64_configure_opennt_target(ntdos64-opennt-nt-bop-objects)
target_compile_definitions(ntdos64-opennt-nt-bop-objects PRIVATE _X86_)
target_include_directories(ntdos64-opennt-nt-bop-objects PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc)
ntdos64_configure_opennt_target(ntdos64-opennt-sim32-objects)
ntdos64_configure_opennt_target(ntdos64-opennt-sim32-owner-closure-objects)
target_compile_definitions(ntdos64-opennt-sim32-owner-closure-objects PRIVATE
    SIM32
    _X86_)
ntdos64_configure_opennt_target(ntdos64-opennt-sim32-ntvdm-profile-objects)
# sim32.c encloses its original implementation in SIM32.  Without this
# historical role macro the object is empty and /FORCE:UNRESOLVED masks the
# missing entry point as a null call in trace diagnostics.
# Its unused host-SAS allocation owner is deliberately renamed in this
# bounded fixture island: CCPU continues to own the one trace aperture via
# ccpu_monitor_sm0_foundation.c, while Sim32's original pointer conversion is
# retained unchanged. This is link ownership isolation, not a replacement.
target_compile_definitions(ntdos64-opennt-sim32-objects PRIVATE
    SIM32
    _X86_
    host_sas_init=ntdos64_sim32_host_sas_init_unused
    host_sas_term=ntdos64_sim32_host_sas_term_unused)
ntdos64_configure_opennt_target(ntdos64-opennt-dem-objects)
ntdos64_configure_opennt_target(ntdos64-opennt-dem-ccpu-sas-objects)
ntdos64_configure_opennt_target(ntdos64-opennt-demerror-register-objects)
target_include_directories(ntdos64-opennt-dem-objects PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc)
target_include_directories(ntdos64-opennt-dem-ccpu-sas-objects PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc)
target_compile_definitions(ntdos64-opennt-dem-ccpu-sas-objects PRIVATE
    NTDOS64_CCPU_SAS_GETVDMADDR)
target_include_directories(ntdos64-opennt-demerror-register-objects PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc)
# Preserve the original source while asking LLVM to determine the one fixture
# call's reachable closure before COFF symbol resolution. This is a build
# diagnostic only; it cannot add a replacement DEM/global stub or alter DEM.
target_compile_options(ntdos64-opennt-demerror-register-objects PRIVATE
    /clang:-flto)
ntdos64_configure_opennt_target(ntdos64-opennt-demlock-register-objects)
target_include_directories(ntdos64-opennt-demlock-register-objects PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc)
ntdos64_configure_opennt_target(ntdos64-opennt-system-provider-objects)
target_compile_definitions(ntdos64-opennt-system-provider-objects PRIVATE _X86_)
ntdos64_configure_opennt_target(ntdos64-opennt-support-provider-objects)
target_include_directories(ntdos64-opennt-support-provider-objects PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc)
ntdos64_configure_opennt_target(ntdos64-opennt-keymouse-provider-objects)
ntdos64_configure_opennt_target(ntdos64-opennt-disks-provider-objects)
ntdos64_configure_opennt_target(ntdos64-opennt-diskbios-provider-objects)
ntdos64_configure_opennt_target(ntdos64-opennt-comms-provider-objects)
target_compile_definitions(ntdos64-opennt-comms-provider-objects PRIVATE PRINTER)
ntdos64_configure_opennt_target(ntdos64-opennt-disk-host-backend-objects)
target_compile_definitions(ntdos64-opennt-disk-host-backend-objects PRIVATE _X86_)
ntdos64_configure_opennt_target(ntdos64-opennt-contained-media-fdisk-objects)
ntdos64_configure_opennt_target(ntdos64-opennt-contained-media-fdisk-test)
ntdos64_configure_opennt_target(ntdos64-opennt-oemuni-provider-objects)
# The original sources establish their own `nt.h -> ntrtl.h -> nturtl.h ->
# windows.h` order and inspect original process layouts. Select the matching
# OpenNT SDK umbrella before the local minimal compatibility header; do not
# force the generic Windows-first staging preamble ahead of it.
target_include_directories(ntdos64-opennt-oemuni-provider-objects BEFORE PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/sdk/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc)
target_compile_definitions(ntdos64-opennt-oemuni-provider-objects PRIVATE _X86_)
target_compile_options(ntdos64-opennt-oemuni-provider-objects PRIVATE
    /FI${CMAKE_CURRENT_SOURCE_DIR}/local/compat/compiler/opennt_dbg_seh_spelling.h)
ntdos64_configure_opennt_target(ntdos64-opennt-contained-oemuni-objects)
ntdos64_configure_opennt_target(ntdos64-opennt-contained-oemuni-test)
ntdos64_configure_opennt_target(ntdos64-opennt-video-provider-objects)
# `ccpu386/ccpudefs.inc`, rather than obj.vdm's native x86 V86 monitor
# configuration, owns this software-CCPU composition.  Defining MONITOR here
# causes ega_read.c to compile to an empty object under NTVDM and leaves the
# CCPU-required read_pointers symbol as a forced-unresolved zero reference.
target_compile_definitions(ntdos64-opennt-video-provider-objects PRIVATE
    CPU_30_STYLE
    C_VID
    )
target_include_directories(ntdos64-opennt-video-provider-objects PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc)
ntdos64_configure_opennt_target(ntdos64-opennt-ccpu-vidc-global-objects)
ntdos64_configure_opennt_target(ntdos64-opennt-ccpu-vglob-objects)
target_compile_definitions(ntdos64-opennt-ccpu-vidc-global-objects PRIVATE
    CPU_30_STYLE C_VID)
target_compile_definitions(ntdos64-opennt-ccpu-vglob-objects PRIVATE
    CPU_30_STYLE)
ntdos64_configure_opennt_target(ntdos64-opennt-ccpu-cvidc-video-closure-fixture)
target_link_options(ntdos64-opennt-ccpu-cvidc-video-closure-fixture PRIVATE /OPT:REF /errorlimit:0)
# `nt_cpu.c` carries two mutually exclusive historical roles.  CPU_40_STYLE
# selects the RISC generated monitor-register facade; the direct CCPU probe
# needs only its ordinary cpu_simulate()/cpu_interrupt() wrappers.  Keep this
# source gate in the CCPU profile without the RISC layout contract.
ntdos64_configure_opennt_target(ntdos64-opennt-ccpu-host-cpu-objects WITHOUT_CPU_40_STYLE)
target_compile_definitions(ntdos64-opennt-ccpu-host-cpu-objects PRIVATE _X86_)
target_include_directories(ntdos64-opennt-ccpu-host-cpu-objects PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc)
ntdos64_configure_opennt_target(ntdos64-opennt-ccpu-host-inthk-objects)
target_compile_definitions(ntdos64-opennt-ccpu-host-inthk-objects PRIVATE _X86_)
target_include_directories(ntdos64-opennt-ccpu-host-inthk-objects PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc)
ntdos64_configure_opennt_target(ntdos64-opennt-ccpu-host-sas-objects)
target_compile_definitions(ntdos64-opennt-ccpu-host-sas-objects PRIVATE
    SIM32
    _X86_
    # DPMI32's i386 mapper is already the selected owner of these two public
    # pointer bridges in this closure. Keep Sim32's independent SAS allocator
    # without admitting a second provider for the same bridge ABI.
    Sim32pGetVDMPointer=ntdos64_sim32_pointer_bridge_unused
    ExpSim32GetVDMPointer=ntdos64_sim32_exp_pointer_bridge_unused)
target_include_directories(ntdos64-opennt-ccpu-host-sas-objects PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc)
# The no-SM0 CCPU closure already owns sim32.c under its original names. Keep
# nt_mem.c independently selectable so its actual CPU_40_STYLE SAS allocation
# owner can be recovered without a second Sim32 definition.
ntdos64_configure_opennt_target(ntdos64-opennt-ccpu-host-intel-memory-owner-objects)
target_compile_definitions(ntdos64-opennt-ccpu-host-intel-memory-owner-objects PRIVATE
    SIM32
    _X86_)
target_include_directories(ntdos64-opennt-ccpu-host-intel-memory-owner-objects PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc)
# `sim32.c` has two historical host-SAS implementations. The CPU_40_STYLE
# branch delegates to nt_mem.c; the CCPU x86 branch reserves/commits its own
# linear aperture. Keep their object profiles distinct and preserve DPMI32 as
# the selected owner of Sim32's unrelated public pointer bridges.
ntdos64_configure_opennt_target(ntdos64-opennt-ccpu-host-sas-x86-objects
    WITHOUT_CPU_40_STYLE)
target_compile_definitions(ntdos64-opennt-ccpu-host-sas-x86-objects PRIVATE
    SIM32
    _X86_
    Sim32pGetVDMPointer=ntdos64_sim32_x86_pointer_bridge_unused
    ExpSim32GetVDMPointer=ntdos64_sim32_x86_exp_pointer_bridge_unused)
target_include_directories(ntdos64-opennt-ccpu-host-sas-x86-objects PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc)
target_compile_options(ntdos64-opennt-ccpu-host-sas-x86-objects PRIVATE
    # Non-CPU_40 Sim32 names the historical V86 monitor linear-pointer
    # import. The CCPU source owner supplies the same contract as c_GetLinAdd
    # (and its x86 generated header exposes that mapping), so select the
    # existing CCPU owner rather than introduce a pointer-conversion shim.
    /FI${CMAKE_CURRENT_SOURCE_DIR}/local/compat/compiler/opennt_ccpu_sim32_x86_abi_compat.h)
ntdos64_configure_opennt_target(ntdos64-opennt-ccpu-host-eoi-objects)
# nt_eoi.c starts with nt.h, whose original include chain owns ntexapi.h and
# the RTL_ATOM declarations required by ntrtl.h. Do not predefine _NTEXAPI_.
target_compile_definitions(ntdos64-opennt-ccpu-host-eoi-objects PRIVATE _X86_)
target_include_directories(ntdos64-opennt-ccpu-host-eoi-objects PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc)
ntdos64_configure_opennt_target(ntdos64-opennt-ccpu-host-timer-objects)
target_compile_definitions(ntdos64-opennt-ccpu-host-timer-objects PRIVATE _X86_)
target_include_directories(ntdos64-opennt-ccpu-host-timer-objects PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/windows/inc)
target_compile_options(ntdos64-opennt-ccpu-host-timer-objects PRIVATE
    /FI${CMAKE_CURRENT_SOURCE_DIR}/local/compat/compiler/opennt_legacy_seh_compat.h)
ntdos64_configure_opennt_target(
    ntdos64-opennt-historical-host-portable-inventory-objects WITHOUT_CPU_40_STYLE)
target_compile_definitions(ntdos64-opennt-historical-host-portable-inventory-objects PRIVATE
    _X86_
    # fprt.c uses the MSVC 1990s CRT calling-convention spellings but does not
    # include the header that defined them in the current SDK.
    _CRTAPI1=__cdecl
    _CRTAPI2=__cdecl)
ntdos64_configure_opennt_target(
    ntdos64-opennt-historical-host-native-inventory-objects WITHOUT_CPU_40_STYLE)
target_compile_definitions(ntdos64-opennt-historical-host-native-inventory-objects PRIVATE _X86_)
target_include_directories(ntdos64-opennt-historical-host-native-inventory-objects BEFORE PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/ddk/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/windows/inc)
target_compile_options(ntdos64-opennt-historical-host-native-inventory-objects PRIVATE
    /FI${CMAKE_CURRENT_SOURCE_DIR}/local/compat/compiler/opennt_legacy_seh_compat.h)
ntdos64_configure_opennt_target(ntdos64-opennt-original-perf-counter-fixture)
target_link_options(ntdos64-opennt-original-perf-counter-fixture PRIVATE /OPT:REF /errorlimit:0)
ntdos64_configure_opennt_target(ntdos64-opennt-host-sound-objects)
target_compile_definitions(ntdos64-opennt-host-sound-objects PRIVATE _X86_)
target_include_directories(ntdos64-opennt-host-sound-objects PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/windows/inc)
ntdos64_configure_opennt_target(ntdos64-opennt-host-copy-objects)
target_compile_definitions(ntdos64-opennt-host-copy-objects PRIVATE _X86_)
target_include_directories(ntdos64-opennt-host-copy-objects PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/windows/inc)
ntdos64_configure_opennt_target(ntdos64-opennt-host-display-input-objects)
target_compile_options(ntdos64-opennt-host-display-input-objects PRIVATE
    /FI${CMAKE_CURRENT_SOURCE_DIR}/local/compat/compiler/opennt_legacy_sdk_compat.h
    /FI${CMAKE_CURRENT_SOURCE_DIR}/local/compat/compiler/opennt_host_native_io_compat.h)
target_include_directories(ntdos64-opennt-host-display-input-objects PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/windows/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc)
ntdos64_configure_opennt_target(ntdos64-opennt-text-display-host-objects)
target_compile_options(ntdos64-opennt-text-display-host-objects PRIVATE
    /FI${CMAKE_CURRENT_SOURCE_DIR}/local/compat/compiler/opennt_legacy_sdk_compat.h
    /FI${CMAKE_CURRENT_SOURCE_DIR}/local/compat/compiler/opennt_host_native_io_compat.h)
target_include_directories(ntdos64-opennt-text-display-host-objects PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/windows/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc)
ntdos64_configure_opennt_target(ntdos64-opennt-host-idle-lifecycle-objects)
target_compile_definitions(ntdos64-opennt-host-idle-lifecycle-objects PRIVATE
    _USE_32BIT_TIME_T)
target_compile_options(ntdos64-opennt-host-idle-lifecycle-objects PRIVATE
    /FI${CMAKE_CURRENT_SOURCE_DIR}/local/compat/compiler/opennt_nt_unix_crt_compat.h)
ntdos64_configure_opennt_target(ntdos64-opennt-vdd-host-objects)
target_compile_options(ntdos64-opennt-vdd-host-objects PRIVATE
    /FI${CMAKE_CURRENT_SOURCE_DIR}/local/compat/compiler/opennt_dbg_seh_spelling.h)
ntdos64_configure_opennt_target(ntdos64-opennt-host-graphics-render-objects)
target_compile_options(ntdos64-opennt-host-graphics-render-objects PRIVATE
    /FI${CMAKE_CURRENT_SOURCE_DIR}/local/compat/compiler/opennt_legacy_sdk_compat.h)
target_include_directories(ntdos64-opennt-host-graphics-render-objects BEFORE PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/local/graphics-render
    ${CMAKE_CURRENT_SOURCE_DIR}/local/compat/compiler
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/windows/inc)
ntdos64_configure_opennt_target(ntdos64-opennt-comms-host-backend-objects)
target_include_directories(ntdos64-opennt-comms-host-backend-objects PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/windows/inc)
target_compile_options(ntdos64-opennt-comms-host-backend-objects PRIVATE
    /FI${CMAKE_CURRENT_SOURCE_DIR}/local/compat/compiler/opennt_legacy_seh_compat.h
    /FI${CMAKE_CURRENT_SOURCE_DIR}/local/compat/compiler/opennt_host_native_io_compat.h)
ntdos64_configure_opennt_target(ntdos64-opennt-comms-native-host-objects)
target_compile_definitions(ntdos64-opennt-comms-native-host-objects PRIVATE _X86_)
target_include_directories(ntdos64-opennt-comms-native-host-objects PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/windows/inc)
target_compile_options(ntdos64-opennt-comms-native-host-objects PRIVATE
    /FI${CMAKE_CURRENT_SOURCE_DIR}/local/compat/compiler/opennt_legacy_seh_compat.h)
ntdos64_configure_opennt_target(ntdos64-opennt-command-objects)
target_compile_definitions(ntdos64-opennt-command-objects PRIVATE WIN_32 _X86_)
target_include_directories(ntdos64-opennt-command-objects PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/base/mvdm/dos/command
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/windows/inc)
ntdos64_configure_opennt_target(ntdos64-opennt-dpmi32-objects)
# The original private VDM header exposes VDM_TIB only with the native NT
# declaration profile selected. This affects declarations only; no NT VDM API
# implementation is supplied by this default-disabled object gate. The fixed
# OpenNT DPMI source still spells the old PmStackInfo member while its adjacent
# private header renamed that prefix-compatible field DpmiInfo. This alias is
# solely a compile/closure aid; it makes no runtime VDM_TIB ABI claim.
target_compile_definitions(ntdos64-opennt-dpmi32-objects PRIVATE
    LINKED_INTO_MONITOR
    _X86_
    PmStackInfo=DpmiInfo)
target_include_directories(ntdos64-opennt-dpmi32-objects BEFORE PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/local/compat/compiler
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc)
target_include_directories(ntdos64-opennt-dpmi32-objects PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/base/mvdm/dpmi32
    ${CMAKE_CURRENT_SOURCE_DIR}/base/mvdm/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/windows/inc)
target_compile_options(ntdos64-opennt-dpmi32-objects PRIVATE
    /FI${CMAKE_CURRENT_SOURCE_DIR}/local/compat/compiler/opennt_dpmi32_native_preamble.h
    /clang:-fcommon)
ntdos64_configure_opennt_target(ntdos64-opennt-dbg-objects)
target_compile_definitions(ntdos64-opennt-dbg-objects PRIVATE WIN_32 DEBUG _X86_)
target_include_directories(ntdos64-opennt-dbg-objects PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/base/mvdm/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/windows/inc)
target_compile_options(ntdos64-opennt-dbg-objects PRIVATE
    /FI${CMAKE_CURRENT_SOURCE_DIR}/local/compat/compiler/opennt_dpmi32_native_preamble.h
    /FI${CMAKE_CURRENT_SOURCE_DIR}/local/compat/compiler/opennt_dbg_seh_spelling.h)
ntdos64_configure_opennt_target(ntdos64-opennt-register-bridge-objects)
ntdos64_configure_opennt_target(ntdos64-opennt-event-host-objects)
target_compile_definitions(ntdos64-opennt-event-host-objects PRIVATE _X86_)
target_include_directories(ntdos64-opennt-event-host-objects PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/windows/inc)
target_compile_options(ntdos64-opennt-event-host-objects PRIVATE
    /FI${CMAKE_CURRENT_SOURCE_DIR}/local/compat/compiler/opennt_legacy_seh_compat.h)
ntdos64_configure_opennt_target(ntdos64-opennt-session-host-objects)
target_compile_definitions(ntdos64-opennt-session-host-objects PRIVATE _X86_)
target_include_directories(ntdos64-opennt-session-host-objects PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/windows/inc)
ntdos64_configure_opennt_target(ntdos64-opennt-reset-shell-objects)
target_compile_definitions(ntdos64-opennt-reset-shell-objects PRIVATE _X86_)
target_include_directories(ntdos64-opennt-reset-shell-objects PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/windows/inc)
ntdos64_configure_opennt_target(ntdos64-opennt-error-host-objects)
target_compile_definitions(ntdos64-opennt-error-host-objects PRIVATE _X86_)
target_include_directories(ntdos64-opennt-error-host-objects PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/windows/inc)
ntdos64_configure_opennt_target(ntdos64-opennt-host-keycode-nls-objects)
target_compile_definitions(ntdos64-opennt-host-keycode-nls-objects PRIVATE _X86_)
target_include_directories(ntdos64-opennt-host-keycode-nls-objects PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/windows/inc)
ntdos64_configure_opennt_target(ntdos64-opennt-pif-host-objects)
target_compile_definitions(ntdos64-opennt-pif-host-objects PRIVATE WINNT)
target_include_directories(ntdos64-opennt-pif-host-objects PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/windows/inc)
ntdos64_configure_opennt_target(ntdos64-opennt-nt4-emm-objects)
target_compile_definitions(ntdos64-opennt-nt4-emm-objects PRIVATE LIM MONITOR)
ntdos64_configure_opennt_target(ntdos64-opennt-ccpu-lim-writeback-reconstruction-objects)
target_compile_definitions(ntdos64-opennt-ccpu-lim-writeback-reconstruction-objects PRIVATE
    LIM MONITOR NTDOS64_CCPU_LIM_WRITEBACK_RECONSTRUCTION)
ntdos64_configure_opennt_target(ntdos64-opennt-x86-emm-host-objects)
target_compile_definitions(ntdos64-opennt-x86-emm-host-objects PRIVATE LIM MONITOR _X86_)
ntdos64_configure_opennt_target(ntdos64-opennt-config-resource-objects)
ntdos64_configure_opennt_target(ntdos64-opennt-config-prefix-objects)
target_compile_definitions(ntdos64-opennt-config-resource-objects PRIVATE _X86_)
target_compile_definitions(ntdos64-opennt-config-prefix-objects PRIVATE _X86_)
ntdos64_configure_opennt_target(ntdos64-opennt-config-x86-prefix-objects
    WITHOUT_CPU_40_STYLE)
target_compile_definitions(ntdos64-opennt-config-x86-prefix-objects PRIVATE _X86_)
ntdos64_configure_opennt_target(ntdos64-opennt-config-x86-prefix-trace-fixture
    WITHOUT_CPU_40_STYLE)
ntdos64_configure_opennt_target(ntdos64-opennt-config-x86-umb-trace-fixture
    WITHOUT_CPU_40_STYLE)
ntdos64_configure_opennt_target(ntdos64-opennt-config-resource-link-fixture)
target_link_options(ntdos64-opennt-config-resource-link-fixture PRIVATE /errorlimit:0)
ntdos64_configure_opennt_target(ntdos64-opennt-xms-objects)
target_compile_definitions(ntdos64-opennt-xms-objects PRIVATE WIN_32 _X86_)
target_include_directories(ntdos64-opennt-xms-objects PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/base/mvdm/xms.486)
ntdos64_configure_opennt_target(ntdos64-opennt-xms-link-fixture)
target_link_options(ntdos64-opennt-xms-link-fixture PRIVATE /errorlimit:0)
ntdos64_configure_opennt_target(ntdos64-opennt-suballoc-objects)
target_compile_definitions(ntdos64-opennt-suballoc-objects PRIVATE DEV_DBG _X86_)
target_include_directories(ntdos64-opennt-suballoc-objects PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/base/mvdm/suballoc)
ntdos64_configure_opennt_target(ntdos64-opennt-umb-provider-objects)
target_compile_definitions(ntdos64-opennt-umb-provider-objects PRIVATE _X86_)
ntdos64_configure_opennt_target(ntdos64-opennt-gfi-config-trace-objects
    WITHOUT_CPU_40_STYLE)
ntdos64_configure_opennt_target(ntdos64-opennt-gfi-empty-objects
    WITHOUT_CPU_40_STYLE)
ntdos64_configure_opennt_target(ntdos64-opennt-gfi-init-trace-fixture
    WITHOUT_CPU_40_STYLE)
ntdos64_configure_opennt_target(ntdos64-opennt-gfi-init-config-host-trace-fixture
    WITHOUT_CPU_40_STYLE)
ntdos64_configure_opennt_target(ntdos64-opennt-gfi-checkpoint-trace-fixture
    WITHOUT_CPU_40_STYLE)
ntdos64_configure_opennt_target(ntdos64-opennt-gfi-init-startup-set-trace-fixture
    WITHOUT_CPU_40_STYLE)
ntdos64_configure_opennt_target(ntdos64-opennt-config-x86-media-absent-fixture
    WITHOUT_CPU_40_STYLE)
ntdos64_configure_opennt_target(ntdos64-opennt-session-byob-objects)
target_compile_definitions(ntdos64-opennt-session-byob-objects PRIVATE
    NTDOS64_BYOB_NTIO
    _X86_)
target_include_directories(ntdos64-opennt-session-byob-objects PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/windows/inc)
ntdos64_configure_opennt_target(ntdos64-opennt-session-byob-trace-objects)
target_compile_definitions(ntdos64-opennt-session-byob-trace-objects PRIVATE
    NTDOS64_BYOB_NTIO
    NTDOS64_SESSION_TRACE_IO_INIT
    _X86_)
target_include_directories(ntdos64-opennt-session-byob-trace-objects PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/windows/inc)
ntdos64_configure_opennt_target(ntdos64-opennt-session-byob-io-trace-objects)
target_compile_definitions(ntdos64-opennt-session-byob-io-trace-objects PRIVATE
    NTDOS64_BYOB_NTIO
    NTDOS64_SESSION_TRACE_AFTER_IO_INIT
    _X86_)
target_include_directories(ntdos64-opennt-session-byob-io-trace-objects PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/windows/inc)
ntdos64_configure_opennt_target(ntdos64-opennt-session-byob-reset-trace-objects)
target_compile_definitions(ntdos64-opennt-session-byob-reset-trace-objects PRIVATE
    NTDOS64_BYOB_NTIO
    NTDOS64_SESSION_TRACE_AFTER_RESET
    NTDOS64_SESSION_TRACE_VINT
    NTDOS64_SESSION_TRACE_VINT_STOP_STAGE=${NTDOS64_SESSION_TRACE_VINT_STOP_STAGE}
    _X86_)
target_include_directories(ntdos64-opennt-session-byob-reset-trace-objects PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/windows/inc)
ntdos64_configure_opennt_target(ntdos64-opennt-byob-ntio-path-fixture)
ntdos64_configure_opennt_target(ntdos64-opennt-session-byob-prefix-trace-fixture
    WITHOUT_CPU_40_STYLE)
ntdos64_configure_opennt_target(ntdos64-opennt-session-byob-io-trace-fixture
    WITHOUT_CPU_40_STYLE)
ntdos64_configure_opennt_target(ntdos64-opennt-session-byob-reset-trace-fixture
    WITHOUT_CPU_40_STYLE)
ntdos64_configure_opennt_target(ntdos64-opennt-session-startup-boundary-fixture
    WITHOUT_CPU_40_STYLE)
target_include_directories(ntdos64-opennt-config-resource-objects PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/windows/inc)
target_include_directories(ntdos64-opennt-config-prefix-objects PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/windows/inc)
target_include_directories(ntdos64-opennt-config-x86-prefix-objects PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/base/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/public/internal/windows/inc)
ntdos64_configure_opennt_target(ntdos64-opennt-contained-oemuni-demdir-fixture)
target_compile_options(ntdos64-opennt-contained-oemuni-demdir-fixture PRIVATE /Ui386)
set_target_properties(ntdos64-opennt-historical-provider-archive PROPERTIES
    EXCLUDE_FROM_ALL TRUE
    FOLDER historical-inputs)
# Aggregate admission target only; it contains no link action and is never
# referenced by the modern runner or default build.
add_custom_target(ntdos64-opennt-historical-objects
    DEPENDS ntdos64-opennt-ccpu-objects ntdos64-opennt-host-stack-objects)
set_target_properties(ntdos64-opennt-historical-objects PROPERTIES
    EXCLUDE_FROM_ALL TRUE
    FOLDER historical-inputs)
