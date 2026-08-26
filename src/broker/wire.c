#include "wire.h"

#include <string.h>

static int user_key_present(const uint8_t *user_key)
{
    uint32_t index;
    for (index = 0u; index < BROKER_USER_KEY_BYTES; ++index) {
        if (user_key[index] != 0u) return 1;
    }
    return 0;
}

int broker_wire_message_valid(const broker_wire_message *message)
{
    return message != NULL && message->magic == BROKER_WIRE_MAGIC &&
        message->version == BROKER_WIRE_VERSION &&
        message->byte_count == sizeof(*message) &&
        message->operation >= BROKER_WIRE_REGISTER &&
        message->operation <= BROKER_WIRE_NOTIFY &&
        user_key_present(message->user_key);
}

int broker_wire_message_has_native_value(const broker_wire_message *message)
{
    (void)message;
    /* The version-one message contains only fixed-width copied integers and a
     * fixed-size authenticated user key.  Native values have no field. */
    return 0;
}
