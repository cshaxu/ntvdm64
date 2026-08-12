#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *skip_space(char *text) {
    while (*text == ' ' || *text == '\t') {
        ++text;
    }
    return text;
}

static void trim_right(char *text) {
    size_t length = strlen(text);
    while (length && (text[length - 1] == '\n' || text[length - 1] == '\r' ||
                      text[length - 1] == ' ' || text[length - 1] == '\t')) {
        text[--length] = '\0';
    }
}

static int emit_message(FILE *message, const char *section, int id, FILE *output, const char *symbol) {
    char line[1024];
    int in_section = 0;
    int found = 0;
    rewind(message);
    while (fgets(line, sizeof(line), message)) {
        char *text = skip_space(line);
        char token[64];
        int number;
        int flag;
        int offset;
        trim_right(text);
        if (!found && sscanf(text, "%63s", token) == 1 && strcmp(token, section) == 0) {
            in_section = 1;
            continue;
        }
        if (!in_section) continue;
        if (sscanf(text, "%d U %d %n", &number, &flag, &offset) == 2) {
            if (found) break;
            if (number == id) {
                fprintf(output, ";_______________________\n\n%s DB %s\n", symbol, text + offset);
                found = 1;
            }
            continue;
        }
        if (found && (line[0] == ' ' || line[0] == '\t')) {
            fprintf(output, "\tDB %s\n", text);
        }
    }
    if (found) fputc('\n', output);
    return found;
}

int main(int argc, char **argv) {
    FILE *input;
    FILE *message;
    FILE *output;
    char line[1024];
    const char *base_name;
    int wanted_class;
    int current_class = -1;
    int emit_continuation = 0;

    const char *section;
    if (argc != 7) {
        fprintf(stderr, "usage: %s <input.skl> <country.msg> <section> <class-number> <output.clN> <base-name>\n", argv[0]);
        return 2;
    }
    wanted_class = atoi(argv[4]);
    if (wanted_class < 0) {
        fprintf(stderr, "class number must be non-negative\n");
        return 2;
    }
    input = fopen(argv[1], "rb");
    message = fopen(argv[2], "rb");
    output = fopen(argv[5], "wb");
    if (!input || !message || !output) {
        fprintf(stderr, "unable to open input, messages, or output\n");
        if (input) fclose(input);
        if (message) fclose(message);
        if (output) fclose(output);
        return 1;
    }
    section = argv[3];
    base_name = argv[6];
    fprintf(output, "; %s.cl%d \n\n\n", base_name, wanted_class);

    while (fgets(line, sizeof(line), input)) {
        char *text = skip_space(line);
        trim_right(text);
        if (strncmp(text, ":class", 6) == 0 && isspace((unsigned char)text[6])) {
            current_class = atoi(skip_space(text + 6));
            emit_continuation = 0;
            continue;
        }
        if (strncmp(text, ":def", 4) == 0 && isspace((unsigned char)text[4])) {
            char symbol[128];
            int id;
            char *definition = skip_space(text + 4);
            if (sscanf(definition, "%d %127s", &id, symbol) != 2) return 1;
            emit_continuation = 0;
            if (current_class == wanted_class && !emit_message(message, section, id, output, symbol)) {
                fprintf(stderr, "missing %s message %d\n", section, id);
                return 1;
            }
            continue;
        }
        if (text[0] == ':' || text[0] == ';' || text[0] == '\0') {
            emit_continuation = 0;
            continue;
        }
    }
    fclose(input);
    fclose(message);
    fclose(output);
    return 0;
}
