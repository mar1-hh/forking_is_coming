/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achat <achat@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/21 23:22:39 by achat             #+#    #+#             */
/*   Updated: 2025/06/21 23:22:39 by achat            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

bool	is_redirection(t_token_type type)
{
	return (type == TOKEN_REDIR_IN || 
		type == TOKEN_REDIR_OUT 
		|| type == TOKEN_APPEND
		|| type == TOKEN_HEREDOC);
}

t_redir	*create_redir_node(t_token_type type, char *file, int is_expnd)
{
	t_redir	*new_redir;

	new_redir = malloc(sizeof(t_redir));
	if (!new_redir)
		return (NULL);
	new_redir->file = ft_strdup(file);
	if (!new_redir->file) 
	{
		free(new_redir);
		return (NULL);
	}
	new_redir->type = type;
	new_redir->next = NULL;
	if (type == TOKEN_HEREDOC)
		pipe(new_redir->fd);
	if (is_expnd == 2 || is_expnd == 1)
		new_redir->is_expand = 0;
	else
		new_redir->is_expand = 1;
	return (new_redir);
}

void	add_tokens(t_token **head, char *value, int type
		, int is_space, int quote_type) // fix norm al 3abd
{
	t_token	*new_token;
	t_token	*last;

	new_token = malloc(sizeof(t_token));
	if (!new_token)
		return ;
	new_token->value = ft_strdup(value);
	new_token->type = type;
	new_token->next = NULL;
	new_token->is_space = is_space;
	new_token->quote_type = quote_type;
	if (!*head)
	{
		*head = new_token;
		return ;
	}
	last = *head;
	while (last->next)
		last = last->next;
	last->next = new_token;
}

static int	handle_operator(char *input, int i, t_token **tokens)
{
	char	*value;
	int		type;
	int		len;

	type = TOKEN_WORD;
	len = 1;
	if (input[i] == '>' && input[i + 1] == '>')
	{
		type = TOKEN_APPEND;
		len = 2;
	}
	else if (input[i] == '<' && input[i + 1] == '<')
	{
		type = TOKEN_HEREDOC;
		len = 2;
	}
	else if (input[i] == '>')
		type = TOKEN_REDIR_OUT;
	else if (input[i] == '<')
		type = TOKEN_REDIR_IN;
	else if (input[i] == '|')
		type = TOKEN_PIPE;
	value = ft_substr(input, i, len);
	add_tokens(tokens, value, type, -1, -1);
	free(value);
	return (i + len);
}

static int	handle_quotes(char *input, int i, t_token **tokens, char quote, int is_space)
{
	int		start;
	int		quote_type;
	char	*value;

	start = i;
	i++;
	if (quote == '\'')
		quote_type = 1;
	else
		quote_type = 2;
	while (input[i] && input[i] != quote)
		i++;
	if (!input[i])
	{
		ft_putstr_fd("Error: Unclosed quotes\n", 2);
		return (-1);
	}
	value = ft_substr(input, start + 1, i - start - 1);
	add_tokens(tokens, value, TOKEN_WORD, is_space, quote_type);
	free(value);
	return (i + 1);
}

static int	handle_word(char *input, int i, t_token **tokens, int is_space)
{
	int		start;
	char	*value;

	start = i;
	while (input[i] && !ft_isspace(input[i]) && 
		!ft_strchr("><|\"'", input[i]))
		i++;
	value = ft_substr(input, start, i - start);
	add_tokens(tokens, value, TOKEN_WORD, is_space, -1);
	free(value);
	return (i);
}

t_token	*lexer(char *input)
{
	t_token	*tokens;
	int		i;
	int		result;
	int		is_space;

	i = 0;
	tokens = NULL;
	is_space = 1;
	while (input[i])
	{
		if (ft_isspace(input[i]))
		{
			is_space = 1;
			i++;
			continue ;
		}
		if (ft_strchr("><|", input[i]))
			result = handle_operator(input, i, &tokens);
		else if (input[i] == '\'' || input[i] == '"')
		{
			result = handle_quotes(input, i, &tokens, input[i], is_space);
			is_space = 0;
		}
		else
		{
			result = handle_word(input, i, &tokens, is_space);
			is_space = 0;
		}
		if (result == -1)
			return (NULL);
		i = result;
	}
	return (tokens);
}

t_redir	*handle_redir(t_token **tokens)
{
	t_token	*curr;
	t_redir	*redirs;
	t_redir	*redir_tail;
	t_redir	*new_redir;

	redir_tail = NULL;
	redirs = NULL;
	curr = *tokens;
	while (curr && curr->next && curr->type != TOKEN_PIPE)
	{
		if (is_redirection(curr->type))
		{
			new_redir = create_redir_node(curr->type,
					curr->next->value, curr->next->quote_type);
			if (!redirs) 
			{
				redirs = new_redir;
				redir_tail = new_redir;
			}
			else
			{
				redir_tail->next = new_redir;
				redir_tail = new_redir;
			}
			curr = curr->next->next;
		}
		else
			curr = curr->next;
	}
	return (redirs);
}

