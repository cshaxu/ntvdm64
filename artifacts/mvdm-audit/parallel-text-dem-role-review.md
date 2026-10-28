# Original tabbed text and DEM error owners

At base `d56754f49`, the independent shard observers were replayed before
classifying `wow32/wutext.c` and `dos/dem/demerror.c` as partial. Both retain
100% original source text; restoration of behavior is not numerically
measurable from code identity and linked dispatch alone.

The [text review](shard-1/wow-tabbed-review.md) binds two original USER
ordinals and selected conversion calls. Guest tab arrays, encoding, mapping
and allocation failures, handle identity and callback lifetime remain open.
The mapping-failure case is a source-level hypothesis, not a reproduced fault.

The [DEM error review](shard-3/dem-error-role-review.md) binds five original
bodies, two service slots and four selected file-open error calls. Guest error
and carry mapping, hard-error packet identity, retry, nesting and teardown
still require behavior evidence. No guest or product image changed.
