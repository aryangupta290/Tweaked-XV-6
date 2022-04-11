#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    int i;
    char *command[MAXARG];
    if (argc < 3)
    {
        fprintf(2, "Usage: %s mask command\n", argv[0]);
        exit(1);
    }
    int val = argv[1][0] - 48;
    if (val < 0 || val > 9)
    {
        fprintf(2, "Usage: %s mask command\n", argv[0]);
        exit(1);
    }
    if (trace(atoi(argv[1])) < 0)
    {
        fprintf(2, "%s: trace failed\n", argv[0]);
        exit(1);
    }

    for (i = 2; i < argc && i < MAXARG; i++)
    {
        command[i - 2] = argv[i];
    }
    exec(command[0], command);
    exit(0);
}