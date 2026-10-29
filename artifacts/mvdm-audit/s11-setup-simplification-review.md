# T473 S11 owner-requested setup simplification

The owner explicitly directed retaining the simplified UNAPPLY behavior: these
scripts restore the applicable backups and delete the target PATCH directory,
including unrelated contents, as documented. The tests exercise only disposable
copies. No live guest/media was modified by this review. Existing interactive
prompts are retained; the command tests feed their fixture paths to stdin.

Checks: win101_setup_cmd_test.ps1 passed, including media prepare, generated
profile, restoration and missing-backup handling; win31_setup_cmd_roundtrip_test
passed against a copied retail fixture; the Win95 command-safety, failed-prepare
and unapply scripts passed. These do not establish a complete Win95 installation
or runtime acceptance. No add-on source changed in this request. Build-generator
changes in tools/build are independent S11 CPU/WOW candidates with additional
uncommitted source and fixtures; they are not part of this setup-only commit.

During the concurrent broker handoff investigation, the visible NTCON fixture
is explicitly configured for a five-row Console viewport. This accounts for its
short height; it is test configuration, not proof of default product geometry.
