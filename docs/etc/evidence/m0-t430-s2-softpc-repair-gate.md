# T430 S2 SoftPC repair gate

## Question and authority

Owner requires original guest defects retained; original MVDM/OpenNT host
defects may use existing SoftPC fixes only, otherwise TODO; attributable
project-code defects require designed corrections. S1 remains closed.
S2 admits the conditional stack profile, not all subsequent stages at once.

## Inputs and procedure

Baseline S1 ee57f6d64 and production5b9931b8e/T429 unchanged. Read-only commands:
`git -C O:/repos.hobby/softpc show --stat --oneline
ce5f53515d3e6ce0a64e66a5465aa7a8fbca00f7`, then `git show` of that revision
limited to call.c, ret.c, iret.c and keyba.c. No sibling writes/build, binary
import, product launch, process termination or publication.

## Observations and conclusion

The commit replaces operand-sized SP/ESP selection after loading new SS with
existing set_current_SP(new_sp) in CALLF, outer RETF and outer IRET. It also
assigns code_to_send=input_port_val for 8042 C0 and removes JOKER restriction
around output-buffer-read IRQ1 deassertion. These are actual existing patch
bodies, not conclusions drawn only from a commit title. Other changes in that
commit are not admitted for copying.

S2 may adopt the same stack correction with minimal registered diff and
actual-instruction tests. S3 has a matching candidate but must verify this
product's PIC ordering. Neither observation proves product runtime behavior.

## Mixed-origin obligations

- K02: CX-1 underflow predates our adaptation. Without a matching SoftPC fix
  it stays deferred; the added Unicode/OEM conversion and bounded copy need
  their own caller-capacity contract tests. Do not fix originals incidentally.
- K03: original ReadFile writes guest data before completion result words;
  our staging defers data copy until after them. This project-added boundary
  requires failure/callback design and tests, not an original-defect exemption
  or an assumed new atomic-transaction contract.
- K04: retain original valid-buffer/no-error-return ABI; verify added copy/
  conversion, do not invent CF/AX.
- K01: missing project service binding is an integration gap; recover the
  original service if required, not a rewritten fallback algorithm.

Known original VGA/guest limitations remain in TODO. Unknown WRITE cause
stays with the WOW diagnostic owner, not classified as an original defect.

Current sibling src filename search found no vrnetapi.c/vrnmpipe.c/demsrch.c
counterparts. This establishes no available matching fix in the inspected
tree, not proof that none ever existed. The inherited CX0 issue is therefore
recorded as deferred TODO. Current sibling VGA do_new_cursor still bases
visibility on scan-line geometry; the previously recorded bit5 limitation is
not newly fixed here. No unrelated VGA-history patch is adopted.

## Delivery boundary

This initial policy/planning P changes no production source and does not close
S2. Actual-entrypoint fixtures, source integration, profile tests, x86/product
regression and publication remain open. Documentation governance/links/diff
checks precede commit/push. Unrelated side-session proposal remains excluded.
