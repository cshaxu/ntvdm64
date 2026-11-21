#include "compat/include/mvdm_vm_notification.h"
#include "mvdm_vm_bridge.h"
#include "compat/include/mvdm_vm_wake.h"
#include "compat/include/mvdm_vm_continuation_store.h"
#include "compat/include/mvdm_vm_guest_io.h"
#include "mvdm_vm_ingress.h"
#include "compat/include/mvdm_vm_command_entry.h"
#include "mvdm_vm_executor.h"
#include "mvdm_standalone_worker.h"
#include "compat/include/mvdm_command_guest_directory.h"

extern void host_ica_lock(void);
extern void host_ica_unlock(void);

int mvdm_vm_bridge_bop(uint32_t signature, uint32_t operation,
    uint32_t low, uint32_t high, uint32_t vm, int protected_ring0,
    mvdm_vm_bridge_result *result)
{
    uint64_t serial = ((uint64_t)high << 32) | low;
    WORD irq = 0xffff;
    mvdm_vm_wake_identity wake;
    if (signature != MVDM_VM_BRIDGE_SIGNATURE) return 0;
    result->value = 0xffff;
    result->serial_low = result->serial_high = 0;
    result->status = ERROR_ACCESS_DENIED;
    if (!protected_ring0) return 1;
    switch (operation) {
    case 0: result->status = mvdm_vm_notification_open(&irq); break;
    case 1: result->status = mvdm_vm_notification_ready(); break;
    case 2: result->status = mvdm_vm_notification_post(&serial); break;
    case 3: result->status = mvdm_vm_notification_snapshot(&serial); break;
    case 4: result->status = mvdm_vm_notification_acknowledge(serial); break;
    case 5:
        mvdm_vm_ingress_reset();
        host_ica_lock();
        result->status = mvdm_vm_notification_close();
        if (!result->status) {
            mvdm_vm_continuation_reset();
            mvdm_vm_wake_reset();
        }
        host_ica_unlock();
        break;
    case 6:
        result->status = mvdm_command_guest_state_directory_instance(
            &result->serial_low, &result->serial_high) ? ERROR_SUCCESS : ERROR_NOT_READY;
        return 1;
    case 7: result->status = mvdm_vm_wake_register(vm, serial); break;
    case 8:
        host_ica_lock();
        result->status = mvdm_vm_wake_retire(vm, serial);
        if (!result->status) mvdm_vm_continuation_retire(vm, serial);
        host_ica_unlock();
        break;
    case 9: result->status = mvdm_vm_wake_publish(vm, serial, &serial); break;
    case 10:
        result->status = mvdm_vm_wake_take(&wake);
        if (!result->status) {
            result->value = wake.vm;
            result->serial_low = (uint32_t)wake.generation;
            result->serial_high = (uint32_t)(wake.generation >> 32);
        }
        return 1;
    case 11:
        result->status = mvdm_vm_command_admit(vm, low, high, &serial);
        result->value = (uint32_t)serial;
        result->serial_low = (uint32_t)(serial >> 32);
        return 1;
    case 15:
        result->status = high ? ERROR_INVALID_PARAMETER : mvdm_vm_ingress_register(low);
        break;
    case 16:
        result->status = mvdm_vm_executor_key_bop(mvdm_standalone_worker_executor(),
            vm, low, high);
        break;
    case 17:
        result->status = mvdm_vm_executor_bios_key_bop(mvdm_standalone_worker_executor(),
            vm, low, high);
        break;
    case 19:
        result->status = mvdm_vm_executor_mouse_bop(mvdm_standalone_worker_executor(),
            vm, low, high);
        break;
    case 18:
        result->status = mvdm_vm_executor_return_key_bop(vm, low, high,
            &result->value, &result->serial_low);
        return 1;
    case 12: {
        mvdm_vm_completion completion;
        result->value = 0;
        result->status = mvdm_vm_continuation_consume(serial, vm, &completion);
        if (!result->status) {
            result->status = completion.error;
            result->value = completion.exit_code;
            result->serial_low = completion.reentry;
        }
        return 1;
    }
    case 13: case 14:
        result->status = mvdm_vm_application_bop(operation, vm, low, high);
        break;
    default: result->status = ERROR_INVALID_FUNCTION; break;
    }
    if (!result->status) {
        result->value = irq;
        if (operation == 2 || operation == 3 || operation == 9) {
            result->serial_low = (uint32_t)serial;
            result->serial_high = (uint32_t)(serial >> 32);
        }
    }
    return 1;
}
