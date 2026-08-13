#include "bochs.h"
#include "bx-vdm/bx_ntvdm_ntio_preentry_v1.h"

#include <string.h>

int main()
{
  static Bit8u image[0x8400];
  byob_image ntio;
  byob_component_descriptor identity;
  bx_ntvdm_finite_run_request request;
  memset(&identity, 0, sizeof(identity));
  image[0] = 0xe9; image[1] = 0x6d; image[2] = 0x03;
  ntio.bytes = image; ntio.byte_count = sizeof(image);
  identity.bytes = sizeof(image);
  if (!bx_ntvdm_ntio_preentry_v1_prepare(&ntio, &identity, &request, 64, 1000000)) return 1;
  if (request.request_version != BX_NTVDM_FINITE_RUN_REQUEST_VERSION ||
      request.entry_byte_count != sizeof(image) || request.entry_physical_address != 0x700 ||
      request.entry_cs != 0x70 || request.entry_eip != 0 ||
      request.preserve_physical_address != 0x714 || request.preserve_byte_count != 4 ||
      memcmp(request.entry_bytes, image, sizeof(image)) != 0) return 2;
  identity.bytes--;
  return bx_ntvdm_ntio_preentry_v1_prepare(&ntio, &identity, &request, 64, 1000000) ? 3 : 0;
}
