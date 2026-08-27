#include "mvdm_host_identity.h"
#include "session/session.h"

int main(void)
{
    session first;
    session second;
    uint32_t first_identity;
    uint32_t second_identity;
    uintptr_t observed;
    int first_value;
    int second_value;

    if (mvdm_host_identity_publish((uintptr_t)&first_value, &first_identity) ||
        mvdm_host_identity_resolve(1u, &observed) ||
        mvdm_host_identity_release(1u)) return 1;

    session_initialize(&first, 1u);
    session_initialize(&second, 2u);
    if (!session_activate(&first) || !session_activate(&second) ||
        !session_thread_bind(&first)) return 2;
    if (!mvdm_host_identity_publish((uintptr_t)&first_value, &first_identity) ||
        first_identity == 0u ||
        !mvdm_host_identity_resolve(first_identity, &observed) ||
        observed != (uintptr_t)&first_value ||
        !mvdm_host_identity_lookup((uintptr_t)&first_value, &second_identity) ||
        second_identity != first_identity ||
        !mvdm_host_identity_publish((uintptr_t)&first_value, &second_identity) ||
        second_identity != first_identity || !session_thread_unbind(&first)) return 3;

    if (!session_thread_bind(&second) ||
        mvdm_host_identity_resolve(first_identity, &observed) ||
        !mvdm_host_identity_publish((uintptr_t)&second_value, &second_identity) ||
        second_identity == 0u || !session_thread_unbind(&second)) return 4;

    if (!session_thread_bind(&first) ||
        !mvdm_host_identity_release(first_identity) ||
        mvdm_host_identity_resolve(first_identity, &observed) ||
        mvdm_host_identity_lookup((uintptr_t)&first_value, &second_identity) ||
        mvdm_host_identity_release(first_identity) ||
        !mvdm_host_identity_publish((uintptr_t)&first_value, &first_identity) ||
        !session_thread_unbind(&first) || !session_dispose(&first)) return 5;

    if (mvdm_host_identity_resolve(first_identity, &observed) ||
        !session_dispose(&second)) return 6;
    return 0;
}
