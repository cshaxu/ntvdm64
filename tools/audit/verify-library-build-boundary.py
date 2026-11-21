"""Verify A-library and B-compat inputs in the generated, compiled production Ninja graphs."""
import re
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
GRAPHS = {
    "x86/worker": ["ntvdm.exe", "VCDEX.DLL", "VDMREDIR.dll", "nthook32.dll"],
    "x86/wow32": ["wow32.dll"],
    "x64/worker-reference-x86": ["ntvdm.exe"],
    "x64/frontend": ["ntcon.exe"],
    "x64/service": ["ntsrv.exe"],
    "x64/launcher": ["run16.exe"],
    "x64/monitor": ["ntmon.exe"],
    "x64/worker": ["ntvwm.exe"],
    "x64/hook": ["nthook64.dll"],
}


def ninja(directory, tool, *args):
    return subprocess.check_output(
        ["ninja", "-C", directory, "-t", tool, *args],
        text=True, errors="replace",
    ).replace("\\", "/")


# These two original USER units use mirror-owned data layouts. The exception
# is limited to the WOW32 build of the named source, not to the component.
WOW_USER_EXCEPTIONS = {"wow_dialog_lookup.c", "wow_object_ownership.c"}


# S19 removed C2 search paths; original NT ABI remains an S20 obligation.
C1_LAYOUT_ADAPTERS = {"mvdm_monitor_printer.c", "mvdm_vdm_tib.c"}

ORIGINAL_LAYOUT_ADAPTERS = {
    "base_process.c",
    'wow_user_private_access.c',
    'wow_common_dialog_calls.c',

    "wow_common_dialog_callback.c",
    'wow_ole_descriptor_binding.c',
    'wow_bitmap_bindings.c',
    'wow_button_lifetime.c',
    'wow_button_resources.c',
    'wow_callback_frame_lease.c',
    'wow_class_client.c',
    'wow_class_client_bindings.c',
    'wow_class_words_binding.c',
    'wow_cleanup_bindings.c',
    'wow_client_dispatch.c',
    'wow_control_brush.c',
    'wow_control_brush_bindings.c',
    'wow_control_draw_state.c',
    'wow_control_extent_bindings.c',
    'wow_control_fnid.c',
    'wow_control_state.c',
    'wow_default_message_bindings.c',
    'wow_default_native_bindings.c',
    'wow_default_state_bindings.c',
    'wow_default_window_bindings.c',
    'wow_default_window_bits.c',
    'wow_dialog_call_bindings.c',
    'wow_dialog_context_bindings.c',
    'wow_dialog_creation_binding.c',
    'wow_dialog_finalization_binding.c',
    'wow_dialog_keyboard_binding.c',
    'wow_shell_callback.c',
    'wow_timer_callback.c',
    'wow_user_thunk_scope.c',
    'wow_hook_callback.c',
    'wow_dialog_memory.c',
    'wow_dialog_native_bindings.c',
    'wow_dialog_pointer.c',
    'wow_dialog_procedure_bindings.c',
    'wow_dialog_state_bindings.c',
    'wow_edit_call_bindings.c',
    'wow_edit_conversion_bindings.c',
    'wow_edit_entry_bindings.c',
    'wow_edit_lookaside_bindings.c',
    'wow_edit_native_bindings.c',
    'wow_edit_state_bindings.c',
    'wow_edit_thread_bindings.c',
    'wow_idle_hook_binding.c',
    'wow_input_group_lifetime.c',
    'wow_input_membership.c',
    'wow_list_native_bindings.c',
    'wow_menu_client_bindings.c',
    'wow_menu_native_bindings.c',
    'wow_private_user_compat.c',
    'wow_standard_class_bindings.c',
    'wow_task_cleanup_entry.c',
    'wow_task_profile_binding.c',
    'wow_user_borrow_scope.c',
    'wow_user_discovery.c',
    'wow_user_message_bridge.c',
    'wow_user_native_extension.c',
    'wow_user_object_bindings.c',
    'wow_user_registration_state.c',
    'wow_user_registration_bridge.c',
    'wow_task_start_notification.c',
    'wow_user_task_lifecycle.c',
    'wow_window_client_access_bindings.c',
    'wow_window_client_state_bindings.c',
    'wow_window_creation_binding.c',
    'wow_window_procedure_entry.c',
    'wow_window_words_binding.c',
}


BASE_DISPATCH_HEADERS = {
    ROOT / "src/non-mvdm" / name for name in (
        "base/win32/inc/basemsg.h", "base/win32/inc/basevdm.h",
        "public/internal/base/inc/conroute.h", "public/internal/base/inc/vdmapi.h",
        "public/internal/base/inc/winss.h", "public/sdk/inc/devioctl.h",
        "public/sdk/inc/nt.h", "public/sdk/inc/ntcsrmsg.h",
        "public/sdk/inc/ntlpcapi.h", "public/sdk/inc/ntpsapi.h",
        "public/sdk/inc/ntregapi.h", "public/sdk/inc/ntrtl.h",
        "public/sdk/inc/nturtl.h")
}


def allowed_roots(owner, source=""):
    roots = {ROOT / "src" / owner}
    if owner == "compat":
        roots.update(ROOT / "src" / library for library in ("lib-nxvm", "lib-win32"))
    if owner == "compat" and source == "base_dispatch.c":
        roots.add(ROOT / "src/compat/include")
        roots.update(BASE_DISPATCH_HEADERS)
    if owner == "compat" and source in ORIGINAL_LAYOUT_ADAPTERS:
        roots.update(ROOT / "src" / layer for layer in ("mvdm-host", "non-mvdm"))
    if owner == "compat" and source in C1_LAYOUT_ADAPTERS:
        roots.add(ROOT / "src/non-mvdm")
    return roots


# No A/B unit may consume generated C-private layout declarations.
def generated_inputs(directory, owner, source):
    return set()


def verify(graph, products):
    arch, component = graph.split("/")
    directory = ROOT / "build/.ninja" / f"msvc-{arch}-release" / component
    commands = ninja(directory, "commands", *products)
    units = {}
    for line in commands.splitlines():
        match = re.search(r' /Fo("?)([^"]+?\.obj)\1\s+"?[^"]*/src/(lib-(?:nxvm|win32)|compat)/([^"]+\.c)"?$', line, re.I)
        if not match:
            continue
        obj, owner, source = match.group(2, 3, 4)
        if graph == "x86/wow32" and owner == "compat" and source in WOW_USER_EXCEPTIONS:
            continue
        units[obj] = (owner, source)
        roots = {ROOT / "src"} | allowed_roots(owner, source)
        for path in re.finditer(r'(?:^| )/I\s*(?:"([^"]+)"|(\S+))', line, re.I):
            if not any((directory / (path[1] or path[2])).resolve() == r or
                       (owner == "compat" and source in (ORIGINAL_LAYOUT_ADAPTERS | C1_LAYOUT_ADAPTERS) and
                        (directory / (path[1] or path[2])).resolve().is_relative_to(r) and
                        r != ROOT / "src") for r in roots) and not any(
                    (directory / (path[1] or path[2])).resolve() == h.parent
                    for h in generated_inputs(directory, owner, source) |
                    (BASE_DISPATCH_HEADERS if owner == "compat" and source == "base_dispatch.c" else set())):
                raise ValueError(f"{graph}/{obj}: unexpected include root {path[0]}")
        if re.search(r'(?:^| )/FI', line):
            raise ValueError(f"{graph}/{obj}: forced header")
    if not units:
        raise ValueError(f"{graph}: no A objects found")
    records = ninja(directory, "deps", *units)
    current = None
    valid = set()
    for line in records.splitlines():
        if not line.startswith(" "):
            current, _, state = line.partition(": #deps")
            if current in units and "(VALID)" in state:
                valid.add(current)
        elif current in units:
            header = (directory / line.strip()).resolve()
            if (header.is_relative_to(ROOT) and header not in generated_inputs(directory, *units[current])
                    and not any(header.is_relative_to(root) for root in allowed_roots(*units[current]))):
                raise ValueError(f"{graph}/{current}: reads {header}")
    if valid != set(units):
        raise ValueError(f"{graph}: missing current compiler evidence for {set(units) - valid}")
    # Collect actual explicit and implicit prerequisites without following product links.
    for roots in {frozenset(allowed_roots(*identity) | generated_inputs(directory, *identity)) for identity in units.values()}:
        objects = [obj for obj, identity in units.items()
                   if frozenset(allowed_roots(*identity) | generated_inputs(directory, *identity)) == roots]
        for line in ninja(directory, "inputs", *objects).splitlines():
            path = (directory / line.strip().strip('"')).resolve()
            if path.is_relative_to(ROOT) and not any(path.is_relative_to(root) for root in roots):
                raise ValueError(f"{graph}: unexpected build prerequisite {path}")
    print(f"{graph}: {len(units)} A/B objects checked")


if __name__ == "__main__":
    for name, targets in GRAPHS.items():
        verify(name, targets)
