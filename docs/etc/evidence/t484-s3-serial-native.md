# T484 S3 P177 serial discovery native oracle and isolation

B0041's NASM producer was already native. Its remaining test host compiled C#
at runtime, copied old flat-layout images into a runtime directory, and killed
all processes with product names. Replace those mechanisms with the shared CMD
fixture graph, the existing owned physical-stage/observer/cleanup helpers, and
an explicit build-owned source package. RuntimeRoot is now required; the test
cannot default to modifying the installed O:/winnt package.

The small native serial-port-mask oracle preserves QueryDosDeviceA enumeration,
32768-to1048576 buffer growth, error propagation and case-insensitive COM1-COM9
selection. It returns a decimal mask to the existing independently authored DOS
probe's EXPECT_PORT_MASK. The guest test source and product sources are unchanged.
Native build/query PID29652 passes in19.8seconds, actual host mask0.

Initial isolated run build/t484-s3-serial-runtime, PID7308, fails after the direct
DOS probe returns0 because historical SERIAL_DISCOVERY_OK/SERIAL_ENUM_OK markers
are absent. These were implementation assertions for the old readonly shadow
registry path. Current ntvdm_shadow_reg_open_key_ex calls open_section; the
historical readonly helper's trace is not its contract. The old unconditional
SERIAL_ENUM_OK also presupposed a nonempty port mask. Remove these obsolete
markers, retaining the actual guest DPMI/VCD port-array comparison and normal
exit checks for both direct and twice-nested COMMAND launches. No production
trace or product behavior is changed to satisfy the test.

Final run build/t484-s3-serial-runtime-final, supervisor21752, runner24084,
2026-11-24T10:24:06Z to10:24:34Z, exit0. Both observer reports say exited with
exit=0x00000000; result.json records routes2, mask0, pass. This proves absent-port
behavior on this machine, not physical serial I/O or nonzero-port runtime
coverage. Existing mask construction tests and serial-medium tests remain.
The runner uses a private desktop, a fresh package identity and scoped cleanup;
it never stops unrelated sessions. Published/original guest bytes are unchanged.

Cached full/11 raw/22 publication identity check runs as PID30252. B0041 remains
pending only for shared copy/manifest staging under S4, matching the DDEML row;
314/355 disposed,41 remain. Native producer and isolated runtime cleanup are
implemented, not evidence that whole T484 is complete.

PID30252 completed in60.9seconds with exit0;11 raw and22 published hashes
match. No redundant publication. Both owned source-package copies were removed
after exact-path/live-process checks. Small failure/success logs remain.
Governance evidence: closure-result.json in the final batch directory.
