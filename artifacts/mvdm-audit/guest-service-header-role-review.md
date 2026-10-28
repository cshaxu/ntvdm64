# Selected DOS, DPMI and XMS service headers

Three original C headers are selected compiler dependencies and retain matching
simple numeric values with corresponding guest assembly declarations. They
remain partial: the [source review](shard-1/guest-service-constants-review.md)
does not prove actual guest binary identity, expression-valued definitions,
selector-to-provider dispatch, service behavior, failures or task cleanup.

The corresponding guest `.INC` paths remain pending. `softpc.h` was already
partial and is not counted again. No product or guest was changed or executed.
