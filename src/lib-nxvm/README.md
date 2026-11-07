# NXVM shared libraries

The `lib/types`, `lib/base`, `lib/kvm-base`, and `lib/kvm-window` trees are
byte-identical imports from SoftPC src/lib. Their original `lib/` include
namespace is preserved by adding this directory to the compiler search path.
Source provenance and selected production units are owned by
`../ntcon-exe/nxvm-import.json`; corresponding upstream tests are under
`test/lib-nxvm/lib`. Product-specific bindings remain outside these trees.

The libvterm archive and notices remain unchanged; their manifest and bindings
remain with NTCON. Neither sibling repository is a build/runtime dependency.
