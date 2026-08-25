#include "bochs.h"
#include "app/ntio_preentry.h"

#include <string.h>

int main()
{
  static Bit8u image[0x8400];
  byob_image ntio;
  byob_component_descriptor identity;
  runtime_preentry_input preentry;
  runtime_finite_run_request request;
  memset(&identity, 0, sizeof(identity));
  image[0] = 0xe9; image[1] = 0x6d; image[2] = 0x03;
  ntio.bytes = image; ntio.byte_count = sizeof(image);
  identity.bytes = sizeof(image);
  runtime_preentry_input_clear(&preentry);
  preentry.range_count = 1u; preentry.payload_bytes = 4u;
  preentry.ranges[0].physical_address = 0x714u;
  preentry.ranges[0].byte_count = 4u;
  if (!runtime_ntio_preentry_prepare(&ntio, &identity, &preentry, &request, 64, 1000000)) return 1;
  if (request.request_version != RUNTIME_FINITE_RUN_REQUEST_VERSION ||
      request.entry_byte_count != sizeof(image) || request.entry_physical_address != 0x700 ||
      request.entry_cs != 0x70 || request.entry_eip != 0 ||
      request.preserve_physical_address != 0x714 || request.preserve_byte_count != 4 ||
      !request.has_preentry_action || request.preentry_action.range_count != 1u ||
      request.preentry_action.payload_bytes != 4u ||
      memcmp(request.entry_bytes, image, sizeof(image)) != 0) return 2;
  identity.bytes--;
  if (runtime_ntio_preentry_prepare(&ntio, &identity, &preentry, &request, 64, 1000000)) return 3;
  identity.bytes++;
  preentry.ranges[0].byte_count = 3u;
  if (runtime_ntio_preentry_prepare(&ntio, &identity, &preentry, &request, 64, 1000000)) return 4;
  preentry.ranges[0].byte_count = 2u; preentry.ranges[1].physical_address = 0x718u;
  preentry.ranges[1].byte_count = 2u; preentry.range_count = 2u;
  return runtime_ntio_preentry_prepare(&ntio, &identity, &preentry, &request, 64, 1000000) ? 5 : 0;
}
