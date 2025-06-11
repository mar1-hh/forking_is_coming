#include "../../minishell.h"


char    *helper_join(char **args, int *arr, int *start, int size)
{
	int i;
	char    *tmp;
	char    *return_value;

	return_value = ft_strdup(args[*start]);
	i = *start;
	i++;
	while (i < size && !arr[i])
	{
		tmp = return_value;
		return_value = ft_strjoin(return_value, args[i]);
		free(tmp);
		i++;
	}
	*start = i;
	return (return_value);
}

char	**join_arg(char **args, int *arr, int size)
{
	int j;
	char    **mtr;
	int i;
	
	if (!args)
		return (NULL);
	mtr = malloc(sizeof(char *) * (size + 1));
	j = 0;
	i = 0;
	while (i < size)
	{
		mtr[j] = helper_join(args, arr, &i, size);
		j++;
	}
	mtr[j] = NULL;
	return (mtr);
}

