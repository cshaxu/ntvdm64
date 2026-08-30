#include "session/session.h"

int main(void)
{
    session first;
    session second;

    session_initialize(&first, 301u);
    if (session_machine_backend(&first) != SESSION_MACHINE_BACKEND_NONE ||
        session_select_machine_backend(&first, UINT32_C(2)) ||
        !session_select_machine_backend(&first, SESSION_MACHINE_BACKEND_SOFTPC) ||
        session_machine_backend(&first) != SESSION_MACHINE_BACKEND_SOFTPC)
        return 1;
    if (!session_activate(&first) ||
        session_select_machine_backend(&first, SESSION_MACHINE_BACKEND_SOFTPC) ||
        !session_dispose(&first))
        return 2;

    session_initialize(&second, 302u);
    if (!session_select_machine_backend(&second, SESSION_MACHINE_BACKEND_SOFTPC) ||
        !session_activate(&second) ||
        session_machine_backend(&second) != SESSION_MACHINE_BACKEND_SOFTPC ||
        !session_dispose(&second))
        return 3;
    return 0;
}
