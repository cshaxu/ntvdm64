# COMMAND guest build evidence, shard 0

`command-guest-build-evidence.csv` records the 28 pending shard-0 files in
`dos/v86/cmd/command`, their current hashes, direct object names in the
historical link response, same-basename source candidates, and lexical
assembly includes. The response file SHA-256 is `516B5E9B4C836B9820CB0869D7FD56062812D9607C81382BA39A93FE814FCE23`; the adjacent
historical map SHA-256 is `7B5747267BC3F323844A81CD48ACBA5BF9CD62F1538B414F7E35AC0F1D516C53`. The map contains COMMAND public
labels and addresses, but has not been shown to correspond to the deployed
COMMAND.COM.

The link response lists ten of the shard's prebuilt objects, and ten source
files have a matching object basename. Neither proves that source built that
object, that the object was linked into a deployed guest, nor that a specific
command works. Other files include source-only segments, message resources
and build inputs: absence from the response does not imply irrelevance.
All 28 remain pending. No guest was modified or built. To establish behavior,
correlate the immutable shipped executable and original artifacts, then use
existing real guest tests for each feature; preserve source roles even where
the historical build process cannot be reproduced.
