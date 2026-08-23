/////////////////////////////////////////////////////////////////////////
//
// Minimal native-machine composition for the registered BX-MACH-026
// reset-window fixture boundary.
//
/////////////////////////////////////////////////////////////////////////

#ifndef BX_NTVDM_MINIMAL_MACHINE_H
#define BX_NTVDM_MINIMAL_MACHINE_H

class bx_pic_c;

enum bx_ntvdm_minimal_machine_status {
  BX_NTVDM_MINIMAL_MACHINE_OK = 0,
  BX_NTVDM_MINIMAL_MACHINE_ALREADY_ATTEMPTED,
  BX_NTVDM_MINIMAL_MACHINE_SIM_FAILED,
  BX_NTVDM_MINIMAL_MACHINE_MEMORY_FAILED,
  BX_NTVDM_MINIMAL_MACHINE_PORT_SPACE_FAILED,
  BX_NTVDM_MINIMAL_MACHINE_PIC_FAILED,
  BX_NTVDM_MINIMAL_MACHINE_KEYBOARD_FAILED,
  BX_NTVDM_MINIMAL_MACHINE_KEYBOARD_CLEANUP_FAILED,
  BX_NTVDM_MINIMAL_MACHINE_PIC_CLEANUP_FAILED,
  BX_NTVDM_MINIMAL_MACHINE_PORT_SPACE_CLEANUP_FAILED,
  BX_NTVDM_MINIMAL_MACHINE_CPU_CONFIGURATION_FAILED
};

// This C++-only owner exposes neither original-object pointers nor an ABI.
// SIM lifetime is intentionally the fixture process lifetime.
class bx_ntvdm_minimal_machine_c {
public:
  bx_ntvdm_minimal_machine_c();

  bx_ntvdm_minimal_machine_status initialize(Bit64u guest, Bit64u host);
  // Compose the selector-blind 8042 only after the caller has initialized
  // the native PC timer base for its selected IPS.
  bx_ntvdm_minimal_machine_status compose_headless_8042(void);
  bx_ntvdm_minimal_machine_status cleanup(void);

  // Private mantle lifecycle control for the CPU compatibility gate.
  // It is valid only after initialize and before cleanup.
  bx_bool set_realmode_segment_limit_compatibility(bx_bool enabled);
  bx_bool realmode_segment_limit_compatibility_active(void);

private:
  bx_bool attempted;
  bx_bool memory_owned;
  bx_bool port_space_owned;
  bx_bool pic_owned;
  bx_bool keyboard_owned;
  bx_bool cpu_initialized;
  bx_pic_c *pic;
};

#endif
