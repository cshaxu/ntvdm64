#include "launch_options.h"

static const wchar_t *skip_space(const wchar_t *text)
{
    while (*text == L' ' || *text == L'\t') ++text;
    return text;
}

static int prefix_token(const wchar_t *text, const wchar_t *token)
{
    size_t length = wcslen(token);
    return wcsncmp(text, token, length) == 0 &&
        (!text[length] || text[length] == L' ' || text[length] == L'\t');
}

int run16_parse_launch_options(const wchar_t *command, run16_launch_options *options)
{
    const wchar_t *target;
    if (!options) return 0;
    options->command = NULL;
    options->wait = 0;
    if (!command) return 0;
    target = skip_space(command);
    if (prefix_token(target, L"--wait")) {
        options->wait = 1;
        target = skip_space(target + 6);
        if (prefix_token(target, L"--wait")) return 0;
    }
    if (prefix_token(target, L"--")) target = skip_space(target + 2);
    if (!*target) return 0;
    options->command = target;
    return 1;
}
