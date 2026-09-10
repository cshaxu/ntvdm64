# T95 S6 MSVC configuration projection attempt 001 (implementation-blocked)

## Result

The projection package is **implementation-blocked before output creation**.
No projected Bochs `config.h`, report, analysis root, compiler object, link, or
runtime result exists.  The intended root
`artifacts/analysis/t95-s6-msvc-x86-config-projection-20260811-001` was checked
after the failures and does not exist.

This is not evidence for a projected configuration or for the subsequent
syntax route.  The coordinator stopped retries after two tool-level validation
failures; no further tool edit, analysis-root write, or compiler invocation is
admitted in this packet.

## Exact observations

1. Invocation of `tools/build/Project-BochsConfig.ps1` with the pinned manifest and
   the intended fresh root failed at original tool line 53 with
   `You cannot call a method on a null-valued expression.`  The fault was
   `$Matches[2].Trim()` while indexing every `#define`; it occurred before the
   tool's `New-Item` output step and produced no root or report.
2. The first focused test invocation of
   `tests/bx-ntvdm-adapter/Test-BochsConfigProjection.ps1` failed before its
   synthetic fixture setup with `New-Item : A parameter cannot be found that
   matches parameter name 'LiteralPath'.`  It likewise produced no projection
   output.

Two narrow corrections were applied before the stop arrived: null-safe parsing
now appears at `Project-BochsConfig.ps1:53`, and the tool/test use `New-Item
-Path` at lines 117 and 20 respectively.  A later test attempt then exposed
the collection-shape issue `Replacement 'BX_CPU_LEVEL' requires exactly one
source definition; found .`; the subsequent local code state contains an
unverified collection normalization and absent-addition check.  It must not be
treated as repaired: no post-correction successful test or actual projection
exists.

## Current scope and next gate

The manifest and source remain a proposed auditable projection design only.
The next and only action is **coordinator review of the tool design and exact
failure handling**, including whether the parser's macro representation and
test harness are acceptable.  A new execution admission is required before
any retry, source/tool change, fresh analysis root, or `/Zs` syntax observation.
