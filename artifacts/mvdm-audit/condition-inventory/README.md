# Compile-condition inventory (T473 S11)

Snapshot: commit `64840052bebf7386c0c91608ab1d8b4799177e29`. This is an inventory only: no condition was accepted, excluded, or changed by it.

- [tags.csv](tags.csv): identifiers appearing in C/H `#if`, `#ifdef`, `#ifndef` and `#elif`, ordered by occurrence. The build-profile column means at least one command in that build tree explicitly mentions a `/D` or `-D`; it does not mean every translation unit or occurrence uses that definition.
- [sites.csv](sites.csv): every conditional directive, full line-spliced expression, path, starting line, identifiers and source SHA-256. Conditional `#else` alternatives are represented by their enclosing conditional, not additional rows.
- [build-defines.csv](build-defines.csv): explicit compiler `/D` or `-D` flags found in Ninja command dependency closures for the ten named product targets. Counts are command occurrences, not live objects, linked functions, or behavioral coverage. Generated commands may contain fixture or generator dependencies.
- [summary.json](summary.json): scope and limitations.

The lexical C/H scan includes all tracked `src` C/H, including unselected sources and guest-target files; it does not authorize modifying guest or make those files product inputs. It includes include guards, local and compiler macros, numeric-comparison identifiers and inactive branches. Identifiers are not individually resolved macro definitions. Multi-line directives are joined; C comments and string literals are excluded from token extraction. Assembler, resources, generated source and other non-C/H formats are not enumerated here; they remain separate dependencies where relevant. Build flags hidden in headers, compiler predefined macros, `/U`, forced includes, macro redefinitions and transitive execution paths need actual preprocessor/source analysis. A missing explicit build flag is not proof that a macro is undefined.

Examples requiring source- and translation-unit-level review: `CPU_40_STYLE`/`CCPU` select the software executor; `CPU_30_STYLE` has no C/H conditional site in this snapshot; `MONITOR` appears in both excluded historical code and selected printer compilation; `X86GFX` also covers selected software video; `i386` and `_X86_` are distinct identifiers; `V7VGA` selects a video profile. A macro name by itself is never a removal decision.
