# T484 S4 P36: reuse and rename the guest INI preparation owner

## Owner direction and decision

The owner rejected an independent guest_driver_profile tool and required
extending an existing SYSTEM.INI implementation, then renaming that owner to
match its broader responsibility. The uncommitted independent source, target
and wrapper were withdrawn. The existing wow_network_profile owner is extended
and renamed guest-ini; its original network modes remain supported.

There is one selected executable/target, one source file and one CMD entry:

- test/observation/guest_ini.c and fixture target guest-ini;
- tools/prepare-guest-ini.cmd;
- test/observation/verify-guest-ini.ps1, extending the original assertions.

All live build and test consumers use these names. Old names in historical
receipts and the planned S5 consolidation inventory identify provenance, not
aliases. Old executable/map outputs and the withdrawn independent candidate
outputs are removed from the fixture cache after checking no process uses them.

## Implementation and limits

File loading, NUL validation and section recognition are shared. The existing
network write still uses its private sibling filename to avoid SYSTEM.INI host
registry mappings. Repeated driver entries use byte-preserving output in the
same owner; driver candidates never write through a host profile mapping.
The CMD entry retains distinct package/guest ownership markers, validates
inputs, creates a fresh INI candidate before copying drivers, propagates errors,
and installs that candidate last. The two integration preparers retain hash
assertions, source identity and owned cleanup, not their old editing loops.

The utility retains31/95 preparation selection for exact existing caller
behavior. S5's planned general literal-operation interface and broader producer
consolidation remain separate work; this delivery is not a claim that all INI
writers or native preparation tools have completed that broader audit.
No original guest binary, mirror source, winnt.zip or installed guest is changed.

## Actual evidence

Root: build/t484-s4-guest-drivers-reuse. Baseline sources are retained under
build/t484-s4-guest-drivers for comparison only. The separate candidate's earlier
build succeeded before its withdrawal; it was never published or admitted.

- PID32304: extended existing target cached full build/fixture build passes,
  with22 unchanged host publication hashes.
- PID16272,21:41:38Z–21:42:54Z,exit0: original network tests plus driver ordering,
  duplicate/missing sections, invalid mouse setting and existing candidate
  assertions pass. Actual preparers match all224 Win31 and1166 Win95 files;
  source installations remain unchanged and owned copies are removed.
- PID11568,21:43:31Z–21:44:41Z,exit0: final guest-ini target rebuilds after
  renaming;11 raw/22 published host identities remain unchanged. Existing
  coherent publication is reused because no product/addon image changes.
- Final-name PID24204 completed21:45:26Z–21:46:36Z exit0:224/1166 files
  match; source identity and owned cleanup pass (renamed-test-result.json).
- PID15528 completed21:47:03Z–21:48:16Z exit0: real Empty and WfwNet
  message consumers pass, including native peers and normal task retirement;
  all22 host publication hashes match (network-result.json).

This is targeted delivery, not Full or clean interpreter-free acceptance.
Frozen census remains348/355 disposed,7 pending, plus five overlapping chains.
Shared driver preparation does not close unconverted caller configuration.
S4 remains active; the latest plan separately schedules S5 consolidation,
S6 final acceptance and S7 mouse-dialog work. Concurrent plan changes are
preserved, not overwritten by this implementation.
