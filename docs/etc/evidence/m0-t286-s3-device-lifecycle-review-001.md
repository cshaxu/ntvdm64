# M0 T286 S3 — minimal device-lifecycle review

Reviewed the current overlay bodies against pinned Bochs 2.6
`iodev/devices.cc`, `pic.cc` and `keyboard.cc`, together with the adjacent
`adapter-bochs` minimal machine, PIC and headless-8042 assembly.

- `minimal_port_space.cc` owns the omitted upstream `bx_devices_c` constructor,
  stub initialization, default handlers, port maps, registration and dispatch
  bodies. Every body writes private `bx_devices_c` arrays or linked handlers.
  The upstream `init()` would instead start SIM/plugin/product-device
  composition. The overlay is therefore irreducible private state access.
- `pic_lifecycle.cc` is the single private counterpart of the minimal PIC
  registration in adapter assembly. Its eight unregister calls use private
  handler identities. There is no duplicate teardown body and no public
  adapter endpoint.
- `keyboard_lifecycle.cc` is the private counterpart of upstream keyboard
  plugin entry points. The source-shaped `keyboard.cc` calls it; adapter code
  only selects the headless plugin lifecycle. The retained keyboard crops
  remove GUI, mouse, paste, CMOS and parameter-tree product behavior rather
  than adding an alternate controller.

No upstream no-product path initializes these objects without bringing in the
full SIM/plugin/device shell. Consequently D1--D3 remain required-private.
No code moved or behavior changed in S3. The next reduction target is the
separate default-off observer group, which has no current named consumer.
