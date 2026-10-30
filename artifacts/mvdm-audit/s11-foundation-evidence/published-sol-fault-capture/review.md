# Published SOL fault-capture repeat

The package staged from the diagnostic guest-read release has ten host hashes
matching `assets/release/release-manifest.json`. Its unchanged SOL input hash
was `68B196C6FD496BAC0E2F30F162EE0B4B4D6EB5BFEC1AF086C8EA759222211F31`.
The x86 probe attached to the owned worker, posted the normal Options command,
and watched debug exceptions for a bounded interval in each of eight rounds.
All eight dialogs opened, closed and returned to the parent; the observer
reported launcher exit zero. The probe reported no attach failure, access
violation or assertion. Host manifest, probe output and identities are here.

This does not identify or disprove the earlier intermittent 1400. The debugger
changes timing and only observes its per-command attachment intervals, not the
entire lifetime; absence of a captured exception is not proof of correctness.
The roughly 10-second reported open times include the probe's own 10-second
fault-observation wait and must not be used as performance measurements.
The first attempt used an x64 compiler and failed during probe compilation;
it did not launch SOL and is not a product result. No guest was modified.
