# Architecture Rules

1. Preserve the historical owner chain: `CCPU -> BIOS[] -> MS_bop_0 -> DemDispatch -> DEM -> NTDOS`.
2. A local recovery requires a reached OpenNT caller, owner analysis, data-layout and failure-behavior evidence, and a bounded fixture.
3. An overlay cannot become a general CPU, memory, device, BOP/SVC, or DOS implementation.
4. A modern host seam is admitted only for a retired host dependency reached by an original caller; it does not define guest behavior.
5. Comparison repositories and external emulators are read-only evidence, never build, link, runtime, or acceptance inputs.
6. Default product targets cannot depend on research fixtures or unreviewed historical artifacts.
