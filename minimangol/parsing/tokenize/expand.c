#include "../../minishell.h"

// int	size_of_var(char *str)
// {
// 	int	i;
	
// 	i = 0;
// 	while (str[i] && str[i] != '$')
// 		i++;
// 	return (i);
// }

char	*ret_env_v(t_env *lst, char *str)
{
	t_env	*tmp;
	
	while (lst)
	{
		if (!ft_strncmp(lst->key, str, ft_strlen(str)))
		{
			// printf("%s\n", lst->key);
			free(str);
			return lst->value;
		}
		lst = lst->next;
	}
	return (NULL);
}

char    *take_key(char *line, int *end, t_env *lst)
{
	int		size;
	char	*key;

	size = 0;
	while (line[size] && line[size] != ' ' && line[size] != '\'' && line[size] != '\"' && line[size] != '$')
		size++;
	key = ft_calloc(size + 1, sizeof(char));
	if (!key)
		return (NULL);
	ft_memcpy(key, line, size);
	key = ret_env_v(lst, key);
	*end = size;
	return (key);
}

int	is_valide(char *line, int i)
{
	if (!i)
		return (1);
	if (line[i - 1] == '\'')
		return (0);
	return (1);
}

// why clear doesn't work when i dont pass env to execve

char	*expand_line(char *line, t_env *lst)
{
	int     i;
	char    *ptr;
	char	*return_value;
	int		offset;
	int		size;

	i = 0;
	while (line[i])
	{
		if (line[i] == '$' && is_valide(line, i))
		{
			ptr = take_key(line + i + 1, &offset, lst);
			size = i + ft_strlen(ptr) + ft_strlen(line + offset + i);
			return_value = ft_calloc(size + 1, 1);
			ft_memcpy(return_value, line, i);
			ft_memcpy(return_value + i, ptr, ft_strlen(ptr));
			ft_memcpy(return_value + i + ft_strlen(ptr), line + i + offset + 1, ft_strlen(line + i + offset + 1));
			free(line);
			line = return_value;
			i = i + ft_strlen(ptr) - 1;
			// printf("%c\n", return_value[i]);
		}
		i++;
	}
	return (line);
}
