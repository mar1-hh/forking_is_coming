#include "../../minishell.h"

char	*ret_env_v(t_shell *sh, char *str)
{
	t_env	*lst;

	if (!ft_strcmp(str, "?"))
		return (ft_itoa(sh->exit_status));
	lst = sh->env_lst;
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

char    *take_key(char *line, int *end, t_shell *sh)
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
	key = ret_env_v(sh, key);
	*end = size;
	return (key);
}

int	is_valide(t_token *next, char *line, int i)
{
	if (line[i] == '$' && line[i + 1])
		return (1);
	if (line[i] == '$' && next)
	{
		// printf("%d\n", next->is_space);
		if (!next->is_space)
			return (1);
	}
	return (0);
}

char	*expand_line(char **line, t_shell *sh, t_token *next)
{
	t_expand	exp_var;
	int			i;

	i = 0;
	exp_var.tmp = ft_strdup(*line);
	free(*line);
	*line = NULL;
	while (exp_var.tmp[i])
	{
		if (is_valide(next, exp_var.tmp, i))
		{
			exp_var.ptr = take_key(exp_var.tmp + i + 1, &exp_var.offset, sh);
			exp_var.size = i + ft_strlen(exp_var.ptr) + ft_strlen(exp_var.tmp + exp_var.offset + i);
			exp_var.return_value = ft_calloc(exp_var.size + 1, 1);
			ft_memcpy(exp_var.return_value, exp_var.tmp, i);
			ft_memcpy(exp_var.return_value + i, exp_var.ptr, ft_strlen(exp_var.ptr));
			ft_memcpy(exp_var.return_value + i + ft_strlen(exp_var.ptr), exp_var.tmp + i + exp_var.offset + 1, ft_strlen(exp_var.tmp + i + exp_var.offset + 1));
			free(exp_var.tmp);
			exp_var.tmp = exp_var.return_value;
			i = i + ft_strlen(exp_var.ptr) - 1;
		}
		i++;
	}
	return (exp_var.tmp);
}

void	print_nodes(t_token *tk)
{
	while (tk) 
	{
		printf("%s\n", tk->value);
		tk = tk->next;
	}
}

void	add_help(char **mtx, t_token **lst, int is_space)
{
	int	i;
	
	i = 0;
	while (mtx[i])
	{
		if (!i)
			add_tokens(lst, mtx[i], TOKEN_WORD, is_space, -1);
		else
			add_tokens(lst, mtx[i], TOKEN_WORD, 1, -1);
		i++;
	}
}


void	add_first_node(t_token **token, t_token *next, char *line, int is_space)
{
	char	**mtx;
	t_token *lst;

	lst = NULL;
	mtx = ft_split(line, ' ');
	if (!mtx)
		return ;
	if (!mtx[0])
	{
		free((*token)->value);
		(*token)->value = ft_strdup("");
		return ;
	}
	add_help(mtx, &lst, is_space);
	free_mtx(mtx);
	free_node(*token);
	*token = lst;
	while (lst->next)
		lst = lst->next;
	lst->next = next;
}

void	add_nodes(t_token *token, t_token *next, char *line, int is_space)
{
	char	**mtx;
	t_token	*lst;
	t_token	*tmp;

	lst = NULL;
	mtx = ft_split(line, ' ');
	if (!mtx)
		return ;
	if (!mtx[0])
	{
		free(token->next->value);
		token->next->value = ft_strdup("");
		return ;
	}
	add_help(mtx, &lst, is_space);
	tmp = token->next;
	token->next = lst;
	while (lst->next)
		lst = lst->next;
	free_mtx(mtx);
	free_node(tmp);
	lst->next = next;
}

void	is_here_doc(t_token *tmp, int *is_heredoc)
{
	if (tmp->next->type == TOKEN_HEREDOC)
		*is_heredoc = 1;
	else
		*is_heredoc = 0;
}

void	expand_tokens_2(t_token **token, t_shell *sh, int is_heredoc, char *line)
{
	t_token	*tmp;

	tmp = *token;
	while (tmp->next)
	{
		if (tmp->next->type == TOKEN_WORD && tmp->next->quote_type == -1)
		{
			if (!is_heredoc)
			{
				line = expand_line(&tmp->next->value, sh, tmp->next->next);
				add_nodes(tmp, tmp->next->next, line, tmp->next->is_space);
				free(line);
			}
		}
		else if (tmp->next->type == TOKEN_WORD && tmp->next->quote_type == 2)
		{
			if (!is_heredoc)
			{
				line = expand_line(&tmp->next->value, sh, tmp->next->next);
				tmp->next->value = line;
			}
		}
		is_here_doc(tmp, &is_heredoc);
		tmp = tmp->next;
	}
}


void	expand_tokens(t_token **token, t_shell *sh)
{
	char	*line;
	int		is_heredoc;
	
	is_heredoc = 0;
	if ((*token)->type == TOKEN_HEREDOC)
		is_heredoc = 1;
	else if ((*token)->type == TOKEN_WORD && (*token)->quote_type == -1)
	{
		if (!is_heredoc)
		{
			line = expand_line(&(*token)->value, sh, (*token)->next);
			add_first_node(token, (*token)->next, line, (*token)->is_space);
			free(line);
		}
	}
	else if ((*token)->type == TOKEN_WORD && (*token)->quote_type == 2)
	{
		if (!is_heredoc)
		{
			line = expand_line(&(*token)->value, sh, (*token)->next);
			(*token)->value = line;
		}
	}
	expand_tokens_2(token, sh, is_heredoc, line);
}
