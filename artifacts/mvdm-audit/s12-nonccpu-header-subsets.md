# S12 — unselected non-CCPU host header alternatives

DIV-506 marks 217 original non-CCPU branch bodies in three selected host
headers, keeping original conditionals and selected CCPU alternatives. Most
are non-CCPU `cpu4gen.h` alternatives; the selected CCPU functions/macros
and software V86 are unchanged. The old DIV-146 note is updated to describe
the now-marked non-CCPU definitions. Each body matches OpenNT and is recorded
with original location, SHA-256 and selected consumer-graph counts in the
[ledger](s12-nonccpu-header-subsets.jsonl).

Current valid product Ninja dependencies were joined to actual compiler
commands: every observed selected object consuming these three headers has
CCPU defined. Six graph outputs with no dependency rows are assembler or
resource outputs, not C preprocessor inputs. This does not claim support for
other external profiles. Independent reconstruction verified three files
and 217 replacements, plus the explicit DIV-146 comment update, with zero
unrelated byte changes against `b1dff76c3`.

Cached x86/x64 ten-component builds passed. After published identity was
applied, all ten images matched the published set exactly or only in PE link
timestamps. No repeat runtime suite or republish was necessary for unchanged
code. No guest was changed. Other mixed selectors and headers remain in scope.
