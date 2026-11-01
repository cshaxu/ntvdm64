# SoftPC conditional-branch rules comparison

Compared the read-only neighboring `O:/repos.hobby/softpc/docs/rules/CODING.md`
selected-mirror-conditional-branches rule (SHA-256
`F7755B9F2EC6FAFC00BD44220C425957255D69A24EA7BDF76B14BF3FC5030D6D`)
with this repository's `docs/rules/CODING.md` and source policy. The neighboring
project is reference material only, not a build or runtime dependency.

Both already require the outer excluded branch boundary and `#error`, removal
of nested directives within that excluded body, selected alternatives and
necessary diagnostic line control. The adopted additions make explicit:

- Classification uses pinned source, effective branch conditions, production
  and supported test configurations, and the patch ledger; unknown and
  unreachable observations are not proof of falsehood.
- A false required conjunct can establish a false AND for that configuration;
  one false OR operand cannot. Exclusion requires proof across every applicable
  supported configuration, including the surrounding and sibling conditions.
- Proven unreachable bodies are excluded even if they contain local patches;
  registrations are retired only when their actual patches are removed.
- Supported selections need build and behavioral verification, deliberately
  excluded selections need a negative `#error` check, and changed diagnostic
  line mappings need verification across affected alternatives.
- Mirror-source transformations must not become build inputs for this cleanup;
  whole-file exclusions, ordinary comments and include guards are separate.

The adapted rule retains this project's immutable-guest, provenance,
architecture, original-source and existing supported-profile boundaries. It
does not assert that existing source already satisfies every added rule, does
not revive excluded CPU30/hardware V86 paths, and does not authorize guest
changes. This is a documentation and planning update, not a product build or a
claim that the source inventory has been re-audited under the new wording.
