# T484 S3 P170 build hygiene consolidation

B0019 Test-BuildHygiene.ps1 mixed current cleanup protection with a private
configure/compiler runner. The latter tried native-launcher in both architecture
trees, although the current root defines that target only in its x64 branch.
Do not reinstate old build composition to satisfy the test.

Remove the private Run-Dev batch writer and architecture rebuild loop. The
existing verify-generated-input-stability.ps1 owns the real two-pass CMD build,
11 PE architectures, generated inputs, intermediate hashes/timestamps and eight
production graph no-op assertions. No second production build recipe is added.
The old aggregate ccache hit counter did not attribute hits to this test. Its
absence is not proof of a cache regression; final clean/changed-input acceptance
still belongs to S5. The blanket no-C++ graph assertion is not a current product
contract and is removed, rather than banning valid native C++ tool producers.

Keep every cleanup safety assertion. Test the actual BuildTree.cmake policy
in script mode: outside-cache refusal, accepted unconfigured test prefix,
fixed cache settings under hostile caller variables, unchanged output identity,
cache-off launcher removal and exact owned-prefix disposal. This is a native
policy test, not compilation or proof that every graph's optional cache-off
configuration was exercised. There is no generated compiler batch or fixture
binary. Clear-BuildOutput.ps1 remains the existing cleanup owner; its independent
S4 disposition is not closed by this change.

Baseline cleanup-only PID6588 passes22 assertions. Final candidate PID15004,
build/t484-s3-hygiene-final, passes28 assertions, then runs the existing two-pass
CMD cache gate and11 raw/22 published hash checks. Full runtime regression is not
claimed for this tooling-only change. The test source shrinks by33 lines.

Verification completion and delivery are recorded with the final result below.

PID15004 completed successfully in129.1 seconds. Both CMD builds, all eight
graph no-op assertions and all11 raw/22 publication hashes pass. Publication
bytes are unchanged; no redundant copy or new release is claimed. The temporary
policy prefix was removed by the tested cleaner.309/355 disposed,46 remain.
