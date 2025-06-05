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
		if (!ft_strcmp(lst->key, str))
		{
			free(str);
			return lst->value;
		}
		lst = lst->next;
	}
	// printf("1337\n");
	return (ft_strdup(""));
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
	// printf("%s|||||\n", key);
	key = ret_env_v(lst, key);
	*end = size;
	return (key);
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
		if (line[i] == '$')
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
	if (!mtx[i])
	{
		
		return ;
	}
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
		// printf("1337\n");
	if (!mtx[i])
	{
		token->next->value = ft_strdup("");
		return ;
	}
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
	int		is_heredoc;

	tmp = *token;
	is_heredoc = 0;
	if (tmp->type == TOKEN_HEREDOC)
		is_heredoc = 1;
	else if (tmp->type == TOKEN_WORD && tmp->quote_type == -1)
	{
		if (!is_heredoc)
		{
			line = expand_line(tmp->value, env);
			add_first_node(token, tmp->next, line, tmp->is_space);
		}
		tmp = *token;
	}
	else if (tmp->type == TOKEN_WORD && tmp->quote_type == 2)
	{
		if (!is_heredoc)
			line = expand_line(tmp->value, env);
		tmp->value = line;
	}
	while (tmp->next)
	{
		if (tmp->next->type == TOKEN_WORD && tmp->next->quote_type == -1)
		{
			if (!is_heredoc)
			{
				line = expand_line(tmp->next->value, env);
				add_nodes(tmp, tmp->next->next, line, tmp->next->is_space);
			}
		}
		else if (tmp->next->type == TOKEN_WORD && tmp->next->quote_type == 2)
		{
			if (!is_heredoc)
				line = expand_line(tmp->next->value, env);
			// printf("%s||||\n", line);
			tmp->next->value = line;
		}
		if (tmp->next->type == TOKEN_HEREDOC)
			is_heredoc = 1;
		else
			is_heredoc = 0;
		tmp = tmp->next;
	}
}
