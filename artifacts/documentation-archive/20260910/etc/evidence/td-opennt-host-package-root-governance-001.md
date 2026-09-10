# Td — OpenNT-host package-root governance

## Question

Does `opennt-host` mean only the current BaseSrv/BaseClient VDM source slice,
or the project root for accepted non-MVDM OpenNT host packages generally?

## Decision

It is the single mirror root for every separately accepted non-MVDM OpenNT
host package used by `mvdm-host`. Base VDM is its first accepted slice only.
Each future package preserves its original relative path below this root and
must independently satisfy the existing package-boundary audit.

`adapter-opennt-host` remains package-private. Its subfamilies are named by
their accepted `opennt-host` owner package and may provide only that package's
private historical bindings. It is not a generic compatibility root.

## Applied governance

The architecture, source-layout rules, coding rules, source policy and both
component READMEs now use this plural package boundary. The production-root
count is corrected to eighteen, matching the declared directory topology.

No OpenNT source package, build input or runtime behavior was admitted by this
documentation decision.
