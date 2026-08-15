#include "bx_ntvdm_command_profile_consumer_v1.h"
#include <string.h>

int bx_ntvdm_command_profile_consumer_v1_register_class(
    bx_ntvdm_mutation_profile_v1 *profile, uint32_t mutation_class,
    uint32_t permitted_mode_mask)
{
    bx_ntvdm_mutation_owner_registration_v1 registration;
    registration.owner_id = BX_NTVDM_MUTATION_OWNER_V1_COMMAND;
    registration.mutation_class = mutation_class;
    registration.permitted_mode_mask = permitted_mode_mask;
    registration.reserved0 = 0u;
    return bx_ntvdm_mutation_profile_v1_register_owner(profile, &registration);
}
int bx_ntvdm_command_profile_consumer_v1_valid(
    const bx_ntvdm_command_profile_consumer_v1 *consumer)
{ return consumer && consumer->magic==BX_NTVDM_COMMAND_PROFILE_CONSUMER_V1_MAGIC &&
  consumer->abi_version==BX_NTVDM_COMMAND_PROFILE_CONSUMER_V1_VERSION &&
  consumer->struct_bytes==sizeof(*consumer) && !consumer->reserved0 &&
  bx_ntvdm_mutation_profile_v1_valid(&consumer->profile); }
int bx_ntvdm_command_profile_consumer_v1_initialize(
    bx_ntvdm_command_profile_consumer_v1 *consumer,
    const bx_ntvdm_mutation_profile_v1 *profile)
{ if(!consumer||!profile||!bx_ntvdm_mutation_profile_v1_valid(profile))return 0;
  memset(consumer,0,sizeof(*consumer)); consumer->magic=BX_NTVDM_COMMAND_PROFILE_CONSUMER_V1_MAGIC;
  consumer->abi_version=BX_NTVDM_COMMAND_PROFILE_CONSUMER_V1_VERSION;
  consumer->struct_bytes=(uint32_t)sizeof(*consumer); consumer->profile=*profile;
  return bx_ntvdm_command_profile_consumer_v1_valid(consumer); }
int bx_ntvdm_command_profile_consumer_v1_resolve(
    const bx_ntvdm_command_profile_consumer_v1 *consumer,
    uint32_t mutation_class, uint32_t *policy_result_out)
{ return bx_ntvdm_command_profile_consumer_v1_valid(consumer) &&
  bx_ntvdm_mutation_profile_v1_resolve(&consumer->profile,
    BX_NTVDM_MUTATION_OWNER_V1_COMMAND, mutation_class, policy_result_out); }
