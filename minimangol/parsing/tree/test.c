#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
    int pid = fork();
    
    if (!pid)
    {
        printf("1337\n");
        exit(0);
    }
    usleep(1000);
    printf("1337\n");
}