#include "adapter-bochs/machine_facade.h"

#include <string.h>

int main()
{
  const unsigned char input[] = { 0x4d, 0x30, 0x2d, 0x54, 0x32, 0x38, 0x36 };
  unsigned char output[sizeof(input)];

  if (!machine_facade_machine_begin(0x200000u, 0x200000u)) return 1;
  if (!machine_facade_memory_writable(0x1000u, sizeof(input)) ||
      !machine_facade_memory_write(0x1000u, sizeof(input), input) ||
      !machine_facade_memory_readable(0x1000u, sizeof(output)) ||
      !machine_facade_memory_read(0x1000u, sizeof(output), output) ||
      memcmp(input, output, sizeof(input)) != 0) {
    machine_facade_machine_cleanup();
    return 2;
  }
  if (machine_facade_memory_writable(0xa0000u, 1u) ||
      machine_facade_memory_readable(0x200000u, 1u) ||
      machine_facade_memory_read(0x1000u, 0u, output)) {
    machine_facade_machine_cleanup();
    return 3;
  }
  return machine_facade_machine_cleanup() ? 0 : 4;
}
