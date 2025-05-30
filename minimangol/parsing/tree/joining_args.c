#include "../../minishell.h"

char    *helper_join(char **args, int *arr, int *start)
{
    int i;
    char    *tmp;
    char    *return_value;

    return_value = ft_strdup(args[*start]);
    i = *start;
    i++;
    while (i < 5 && !arr[i])
    {
        tmp = return_value;
        return_value = ft_strjoin(return_value, args[i]);
        free(tmp);
        i++;
    }
    *start = i;
    return (return_value);
}

char **join_arg(char **args, int *arr)
{
    int size;
    int j;
    char    **mtr;
    
    mtr = malloc(sizeof(char *) * 5);
    size = 5;
    j = 0;
    int i;
    i = 0;
    while (i < size)
    {
        mtr[j] = helper_join(args, arr, &i);
        j++;
    }
    mtr[j] = NULL;
    return (mtr);
}

int	main(void)
{
	int arr[] = {1, 1, 0, 0, 1};
    char *args[] = {"'", "c", "a", "t", "'"};
    char **mtr = join_arg(args, arr);
	for (int i = 0; mtr[i]; i++)
		printf("%s\n", mtr[i]);
	return (0);
}
