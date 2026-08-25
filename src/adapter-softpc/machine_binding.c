#include "machine_binding.h"

static runtime_machine_binding_a20_get_v1 runtime_machine_binding_get_a20;
static runtime_machine_binding_a20_set_v1 runtime_machine_binding_set_a20;
static runtime_machine_binding_memory_range_v1 runtime_machine_binding_readable;
static runtime_machine_binding_memory_range_v1 runtime_machine_binding_writable;
static runtime_machine_binding_memory_copy_v1 runtime_machine_binding_read;
static runtime_machine_binding_memory_copy_v1 runtime_machine_binding_write;

int runtime_machine_binding_v1_bind_a20(
    runtime_machine_binding_a20_get_v1 get_a20,
    runtime_machine_binding_a20_set_v1 set_a20)
{
    if (get_a20 == 0 || set_a20 == 0 ||
        runtime_machine_binding_get_a20 != 0 ||
        runtime_machine_binding_set_a20 != 0) return 0;
    runtime_machine_binding_get_a20 = get_a20;
    runtime_machine_binding_set_a20 = set_a20;
    return 1;
}

void runtime_machine_binding_v1_unbind_a20(void)
{
    runtime_machine_binding_get_a20 = 0;
    runtime_machine_binding_set_a20 = 0;
}

int runtime_machine_binding_v1_get_a20(uint32_t *enabled)
{
    return runtime_machine_binding_get_a20 != 0 &&
        runtime_machine_binding_get_a20(enabled);
}

int runtime_machine_binding_v1_set_a20(uint32_t enabled)
{
    return runtime_machine_binding_set_a20 != 0 &&
        runtime_machine_binding_set_a20(enabled);
}

int runtime_machine_binding_v1_bind_memory(
    runtime_machine_binding_memory_range_v1 readable,
    runtime_machine_binding_memory_range_v1 writable,
    runtime_machine_binding_memory_copy_v1 read,
    runtime_machine_binding_memory_copy_v1 write)
{
    if (readable == 0 || writable == 0 || read == 0 || write == 0 ||
        runtime_machine_binding_readable != 0 ||
        runtime_machine_binding_writable != 0 ||
        runtime_machine_binding_read != 0 ||
        runtime_machine_binding_write != 0) return 0;
    runtime_machine_binding_readable = readable;
    runtime_machine_binding_writable = writable;
    runtime_machine_binding_read = read;
    runtime_machine_binding_write = write;
    return 1;
}

void runtime_machine_binding_v1_unbind_memory(void)
{
    runtime_machine_binding_readable = 0;
    runtime_machine_binding_writable = 0;
    runtime_machine_binding_read = 0;
    runtime_machine_binding_write = 0;
}

int runtime_machine_binding_v1_memory_readable(uint64_t address,
    uint64_t byte_count)
{
    return runtime_machine_binding_readable != 0 &&
        runtime_machine_binding_readable(address, byte_count);
}

int runtime_machine_binding_v1_memory_writable(uint64_t address,
    uint64_t byte_count)
{
    return runtime_machine_binding_writable != 0 &&
        runtime_machine_binding_writable(address, byte_count);
}

int runtime_machine_binding_v1_memory_read(uint64_t address,
    uint64_t byte_count, uint8_t *bytes)
{
    return runtime_machine_binding_read != 0 &&
        runtime_machine_binding_read(address, byte_count, bytes);
}

int runtime_machine_binding_v1_memory_write(uint64_t address,
    uint64_t byte_count, const uint8_t *bytes)
{
    return runtime_machine_binding_write != 0 &&
        runtime_machine_binding_write(address, byte_count, (uint8_t *)bytes);
}
