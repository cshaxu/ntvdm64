# Retired project DEM ingress shim

These files are pre-T271/S2 project composition evidence, not production
inputs. `src/opennt-bop/ingress/dem_ingress_shim.{c,h}` selected a `50:xx`
service from a copied instruction window before calling the dispatcher.

T271/S2 replaces that path with the imported OpenNT `nt_bop.c:MS_bop_0` body:
it reads the service byte at staged `CS:IP`, invokes `DemDispatch`, and keeps
the original one-byte IP advance. The paired former fixture is retained only
as historical evidence; the active source-route regression is
`tests/component-integration/fixtures/t230_s10_dem_native_session_direct_fixture.c`.

`50:42` is separately retained as the registered BOP-DIV-098 source-derived
fast-read exception, because the original guest calls it while its visible
OpenNT table slot is a no-op.
