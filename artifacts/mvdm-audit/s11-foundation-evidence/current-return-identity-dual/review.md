# Current diagnostic: shared WOW task retirement

The isolated diagnostic package from `current-return-identity` ran Winmine
and the unchanged hash-verified prebuilt common-dialog witness in one worker.
The witness created 17 dialogs, explicitly closed 16 and left one for normal
task retirement. The runner verified its guest records, one retirement,
callback ordering, Winmine flag/counter, reset, open, close and launcher exit;
all passed. No `IdentityFailure`, `publish-native-return`, or split-return
failure marker appeared in the retained lifetime trace. The trace and its
SHA-256 are identified in `inputs-hashes.csv`; short results and package input
manifest are retained. The isolated stage has a fresh package identity, so
its recorded runtime hashes differ from the source package's sealed images.

This is one overlapping-task run using diagnostic-only WOW32; it is not a
forced interleaving, production release or proof that every race is absent.
It did not reproduce or explain historical SOL 1400. No product or guest
binary was modified, rebuilt or published.
