#include "config.h"
#include "error.h"
#include "strbuf.h"
#include "common.h"
#include "../build/BUILTIN_CONFIG.h"

const char *config_file;
int config_line;

static const char *const reset_colors_rc =
"hi default\n"
"hi nontext blue keep\n"
"hi noline blue\n"
"hi wserror default yellow\n"
"hi selection keep gray keep\n"
"hi currentline keep keep keep\n"
"hi linenumber\n"
"hi statusline black gray\n"
"hi commandline\n"
"hi errormsg bold red\n"
"hi infomsg bold blue\n"
"hi tabbar black gray\n"
"hi activetab bold\n"
"hi inactivetab black gray\n";

static bool is_command(const char *str, int len)
{
    for (int i = 0; i < len; i++) {
        if (str[i] == '#') {
            return false;
        }
        if (!ascii_isspace(str[i])) {
            return true;
        }
    }
    return false;
}

// Odd number of backslashes at end of line?
static bool has_line_continuation(const char *str, int len)
{
    int pos = len - 1;

    while (pos >= 0 && str[pos] == '\\') {
        pos--;
    }
    return (len - 1 - pos) % 2;
}

void exec_config(const Command *cmds, const char *buf, size_t size)
{
    const char *ptr = buf;
    char *cmd;
    StringBuffer line = STRBUF_INIT;

    while (ptr < buf + size) {
        size_t n = buf + size - ptr;
        char *end = memchr(ptr, '\n', n);

        if (end) {
            n = end - ptr;
        }

        if (line.len || is_command(ptr, n)) {
            if (has_line_continuation(ptr, n)) {
                strbuf_add_buf(&line, ptr, n - 1);
            } else {
                strbuf_add_buf(&line, ptr, n);
                cmd = strbuf_cstring(&line);
                handle_command(cmds, cmd);
                free(cmd);
                strbuf_clear(&line);
            }
        }
        config_line++;
        ptr += n + 1;
    }
    if (line.len) {
        cmd = strbuf_cstring(&line);
        handle_command(cmds, cmd);
        free(cmd);
    }
    strbuf_free(&line);
}

int do_read_config(const Command *cmds, const char *filename, ConfigFlags flags)
{
    const bool must_exist = flags & CFG_MUST_EXIST;
    const bool builtin = flags & CFG_BUILTIN;

    if (builtin) {
        for (size_t i = 0; i < ARRAY_COUNT(builtin_configs); i++) {
            if (streq(filename, builtin_configs[i].name)) {
                config_file = filename;
                config_line = 1;
                const char *src = builtin_configs[i].source;
                exec_config(cmds, src, strlen(src));
                return 0;
            }
        }
        if (must_exist) {
            error_msg (
                "Error reading '%s': no built-in config exists for that path",
                filename
            );
            return 1;
        }
        return 0;
    }

    char *buf;
    ssize_t size = read_file(filename, &buf);
    int err = errno;

    if (size < 0) {
        if (err != ENOENT || must_exist) {
            error_msg("Error reading %s: %s", filename, strerror(err));
        }
        return err;
    }

    config_file = filename;
    config_line = 1;

    exec_config(cmds, buf, size);
    free(buf);
    return 0;
}

int read_config(const Command *cmds, const char *filename, ConfigFlags flags)
{
    // Recursive
    const char *saved_config_file = config_file;
    int saved_config_line = config_line;
    int ret = do_read_config(cmds, filename, flags);
    config_file = saved_config_file;
    config_line = saved_config_line;
    return ret;
}

void exec_builtin_rc(const char *rc)
{
    // No need to change filename because there can't be any errors
    int saved_config_line = config_line;
    exec_config(commands, rc, strlen(rc));
    config_line = saved_config_line;
}

void exec_reset_colors_rc(void)
{
    exec_builtin_rc(reset_colors_rc);
}
