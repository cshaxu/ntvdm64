#ifndef T200_S3_CMDINFO_OBSERVATION_H
#define T200_S3_CMDINFO_OBSERVATION_H

#include <stdint.h>

/* Fixture-only copied evidence; never a product or adapter ABI. */
typedef struct t200_s3_cmdinfo_observation {
    uint32_t observed, geometry_valid, read_ok, decoded, accepted, disposition;
    uint16_t cs, ds, es, ss, command_segment, command_offset, command_bytes;
    uint16_t environment_segment, environment_bytes, executable_segment;
    uint16_t executable_offset, executable_bytes, return_code;
    uint32_t eip, edx, bootstrap_stage, has_launch_plan, has_drive_snapshot;
    uint32_t delivered, launch_registration_valid;
} t200_s3_cmdinfo_observation;

#ifdef __cplusplus
extern "C" {
#endif
int t200_s3_native_ntio_boundary_copy_cmdinfo_observation(
    t200_s3_cmdinfo_observation *out_value);
#ifdef __cplusplus
}
#endif
#endif
