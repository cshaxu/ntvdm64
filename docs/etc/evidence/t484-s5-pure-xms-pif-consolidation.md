# T484 S5 P2 Pure DOS and XMS PIF consolidation

The existing CMD memory preparation and published PIF editor now generate
pure DOS and XMS as well as EMS profiles. They use the same authored
memory-profile template. Two duplicate native executables, two fixture
targets and 137 C producer lines are removed. The memory fixture build
still produces the independently authored DOS test programs. The archived
OEM path verifier now uses the same preparation command.

All seven previous writer profiles were replayed with the same paths.
Five profiles produced identical PIF and NT configuration bytes. The
other two profile triples retained every active field and every byte
outside the original string tails. PIF.EXE clears 46 bytes of old
trailing path storage in the OEM variant and 55 in the literal startup
variant. Both retain the original expansion strings, layout, flags,
memory values and checksum convention. No editor binary changes were
required for this batch.

PID 9060 passed the actual original pure DOS startup marker and the
default XMS boot UMB, INT15 and allocation/move/reallocation markers.
Both ran through isolated package staging and returned exit zero.
The XMS startup profile preserves the guest-side CONFIG.NT and AUTOEXEC.NT
paths and emits the original profile-loaded witness. Build and test
receipts are under build/t484-s5-pure-xms. The archived independent
OEM runtime replay remains separate from this byte-level verification.

PID 19664 completed the cached full build, matched the eleven raw host
images and their twenty-two published hashes, and matched all fourteen
addon files against the release manifest. Neither release directory
needed new images.
