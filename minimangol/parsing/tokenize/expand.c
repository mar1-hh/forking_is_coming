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

void	print_nodes(t_token *tk)
{
	while (tk)
	{
		printf("%s\n", tk->value);
		tk = tk->next;
	}
}
void	add_first_node(t_token **token, t_token *next, char *line, int is_space)
{
	char	**mtx;
	int		i;
	t_token *lst;

	i = 0;
	lst = NULL;
	mtx = ft_split(line, ' ');
	if (!mtx)
		return ;
	while (mtx[i])
	{
		if (!i)
			add_tokens(&lst, mtx[i], TOKEN_WORD, is_space, -1);
		else
			add_tokens(&lst, mtx[i], TOKEN_WORD, 1, -1);
		i++;
	}
	*token = lst;
	while (lst->next)
		lst = lst->next;
	lst->next = next;
}

void	add_nodes(t_token *token, t_token *next, char *line, int is_space)
{
	char	**mtx;
	int		i;
	t_token	*lst;

	i = 0;
	lst = NULL;
	mtx = ft_split(line, ' ');
	if (!mtx)
		return ;
	while (mtx[i])
	{
		if (!i)
			add_tokens(&lst, mtx[i], TOKEN_WORD, is_space, -1);
		else
			add_tokens(&lst, mtx[i], TOKEN_WORD, 1, -1);
		i++;
	}
	token->next = lst;
	while (lst->next)
		lst = lst->next;
	lst->next = next;
}


void	expand_tokens(t_token **token, t_env *env)
{
	t_token	*tmp;
	char	*line;

	tmp = *token;
	if (tmp->type == TOKEN_WORD && tmp->quote_type == -1)
	{
		line = expand_line(tmp->value, env);
		add_first_node(token, tmp->next, line, tmp->is_space);
		tmp = *token;
	}
	else if (tmp->type == TOKEN_WORD && tmp->quote_type == 2)
	{
		line = expand_line(tmp->value, env);
		free(tmp->value);
		tmp->value = line;
	}
	while (tmp->next)
	{
		if (tmp->next->type == TOKEN_WORD && tmp->next->quote_type == -1)
		{
			line = expand_line(tmp->next->value, env);
			add_nodes(tmp, tmp->next->next, line, tmp->next->is_space);
		}
		else if (tmp->next->type == TOKEN_WORD && tmp->next->quote_type == 2)
		{
			line = expand_line(tmp->next->value, env);
			tmp->next->value = line;
		}
		tmp = tmp->next;
	}
}
