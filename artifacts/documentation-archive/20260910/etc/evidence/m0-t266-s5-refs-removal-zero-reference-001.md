# M0 T266 S5 — repository-local refs removal evidence

- Pre-delete inventory: 1,887 tracked `refs/` files, 30,364,329 bytes.
- `git rm -r refs` removed every tracked snapshot. A checked physical cleanup then removed the remaining untracked external-checkout residue under the exact repository-local root.
- Post-delete: `refs/` is absent and `git ls-files refs` is empty.
- The fresh `build/M0-T266-S5/r001` formal Ninja graph generated from named owned source roots. `ninja -n` enumerated its complete 485-edge local build without a missing/external `refs/` input.
- Remaining literal `refs/...` text is limited to root historical research documentation and the S4 archive provenance manifest. It is neither executable nor a build/runtime/include input.
- The S4 global CMake and retained historical-fixture failures remain documented limitations. They name removed old component symbols/paths, not `refs/`, and do not invalidate the formal Ninja closure.
