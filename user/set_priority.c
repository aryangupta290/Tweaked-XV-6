#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Invalid arguments\n");
        return (0);
    }
    int new_p = atoi(argv[1]);
    int pid = atoi(argv[2]);

    int val = set_priority(new_p, pid);
    if (val == -1)
    {
        printf("No such process with %d is present \n", pid);
    }
    else
    {
        printf("The old static priority was %d\n", val);
    }
    return (0);
}