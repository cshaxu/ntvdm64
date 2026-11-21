"""Boundary gate must resolve compiler paths, not grant blanket B exceptions."""
import importlib.util
from pathlib import Path
import unittest
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[3]
spec = importlib.util.spec_from_file_location(
    "boundary", ROOT / "tools/audit/verify-library-build-boundary.py")
boundary = importlib.util.module_from_spec(spec)
spec.loader.exec_module(boundary)


class BoundaryTest(unittest.TestCase):
    def check_unit(self, source, header, include=None, prerequisite=None):
        obj = "obj/compat/probe.obj"
        command = f'cl /I"{ROOT / "src"}" /Fo{obj} {ROOT / "src/compat" / source}'
        if include:
            command = command.replace(" /Fo", f' /I"{ROOT / include}" /Fo')
        outputs = {
            "commands": command,
            "deps": f"{obj}: #deps 1, deps mtime 1 (VALID)\n    ../../../../{header}\n",
            "inputs": f"../../../../src/compat/{source}\n" +
                      (f"../../../../{prerequisite}\n" if prerequisite else ""),
        }
        with patch.object(boundary, "ninja", side_effect=lambda _, tool, *args: outputs[tool].replace(chr(92), "/")):
            boundary.verify("x86/worker", ["ntvdm.exe"])

    def test_named_adapter_can_read_C(self):
        self.check_unit("mvdm_softpc_wow_page_domain.c",
                        "src/mvdm-host/softpc.new/base/ccpu386/c_addr.h")

    def test_ordinary_B_cannot_read_C(self):
        with self.assertRaises(ValueError):
            self.check_unit("host_context.c", "src/mvdm-host/softpc.new/base/inc/xt.h")

    def test_debug_callback_consumer_has_no_C_exception(self):
        self.check_unit("dbg_registers.c", "src/compat/include/mvdm_cpu_access.h")
        with self.assertRaises(ValueError):
            self.check_unit("dbg_registers.c", "src/mvdm-host/softpc.new/base/ccpu386/c_main.h")

    def test_cpu_callback_consumers_have_no_C_exception(self):
        for source in ("mvdm_keyboard_reset.c", "mvdm_softpc_descriptor_fields.c",
                       "mvdm_softpc_effective_address.c", "mvdm_redirector_async.c", "mvdm_softpc_mouse_bridge.c"):
            with self.subTest(source=source):
                self.check_unit(source, "src/compat/include/mvdm_cpu_access.h")
                with self.assertRaises(ValueError):
                    self.check_unit(source, "src/mvdm-host/softpc.new/base/ccpu386/c_main.h")

    def test_termination_callback_has_no_C_exception(self):
        self.check_unit("mvdm_softpc_termination.c", "src/compat/include/host_context.h")
        with self.assertRaises(ValueError):
            self.check_unit("mvdm_softpc_termination.c", "src/mvdm-host/softpc.new/host/inc/host_def.h")

    def test_printer_keeps_C1_but_rejects_C2(self):
        self.check_unit("mvdm_monitor_printer.c", "src/non-mvdm/public/internal/base/inc/vdm.h",
                        include="src/non-mvdm/public/internal/base/inc")
        with self.assertRaises(ValueError):
            self.check_unit("mvdm_monitor_printer.c", "src/mvdm-host/inc/doswow.h")
        with self.assertRaises(ValueError):
            self.check_unit("mvdm_monitor_printer.c", "src/compat/include/monitor_context.h",
                            include="src/mvdm-host/inc")

    def test_named_adapter_cannot_read_outer_layers(self):
        for layer in ("common", "frontend-base", "ntvdm-exe"):
            with self.subTest(layer=layer), self.assertRaises(ValueError):
                self.check_unit("mvdm_softpc_effective_address.c", f"src/{layer}/private.h")

    def test_outer_include_and_prerequisite_rejected(self):
        for field in ("include", "prerequisite"):
            with self.subTest(field=field), self.assertRaises(ValueError):
                self.check_unit("mvdm_softpc_effective_address.c", "src/compat/include/host_context.h",
                                **{field: "src/ntvdm-exe"})


if __name__ == "__main__":
    unittest.main()
