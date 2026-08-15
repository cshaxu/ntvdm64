# Research-only standalone reference gate. SOURCE-MANIFEST.json defines this
# independent source set. It must be configured separately from the modern
# MinGW-w64 runtime graph so its selected historical compiler/CRT cannot leak
# into ntdos64-run.
cmake_minimum_required(VERSION 3.20)
project(ntdos64_ntvdmx64_reference_inputs LANGUAGES C)

# Research-only reference gate. SOURCE-MANIFEST.json defines this independent
# source set. It deliberately has no OpenNT include directory. The recorded
# monitor is HAXM-specific and prohibited from the CCPU runtime profile.
add_library(ntdos64-ntvdmx64-haxm-reference OBJECT EXCLUDE_FROM_ALL
    ntvdmpatch/experimental/haxm/v86/haxm/i386/monitor.c
    ntvdmpatch/experimental/haxm/v86/haxm/i386/int.c
    ntvdmpatch/experimental/haxm/v86/haxm/i386/sas.c
    ntvdmpatch/experimental/haxm/v86/haxm/i386/thread.c
    ntvdmpatch/experimental/haxm/v86/haxm/i386/debug.c
    ntvdmpatch/experimental/haxm/v86/haxm/i386/spcstubs.c)

set_target_properties(ntdos64-ntvdmx64-haxm-reference PROPERTIES
    EXCLUDE_FROM_ALL TRUE
    FOLDER historical-inputs)
