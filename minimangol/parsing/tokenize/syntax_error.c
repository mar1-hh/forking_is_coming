/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax_error.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achat <achat@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/21 23:22:45 by achat             #+#    #+#             */
/*   Updated: 2025/06/21 23:22:45 by achat            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

static void	update_quote_state(const char *str, int *s_quote, int *d_quote)
{
	int	i;

	i = 0;
	while (str[i])
	{
		if (str[i] == '\'' && !(*d_quote))
			*s_quote = !(*s_quote);
		else if (str[i] == '"' && !(*s_quote))
			*d_quote = !(*d_quote);
		i++;
	}
}

int	check_unclosed_quotes(t_token *tokens)
{
	int	sq;
	int	dq;

	sq = 0;
	dq = 0;
	while (tokens)
	{
		if (tokens->value)
			update_quote_state(tokens->value, &sq, &dq);
		tokens = tokens->next;
	}
	if (sq || dq)
	{
		if (sq)
			printf("minishell: syntax error: unclosed single quote\n");
		else
			printf("minishell: syntax error: unclosed double quote\n");
		return (1);
	}
	return (0);
}

int check_redir_without_file(t_token *tokens)
{
	t_token	*current;
	t_token	*next;

	current = tokens;
	while (current)
	{
		if (is_redir_token(current->type) && current->type != TOKEN_HEREDOC)
		{
			next = current->next;
			if (!next || next->type != TOKEN_WORD)
			{
				if (current->type == TOKEN_REDIR_IN)
					print_error("<");
				else if (current->type == TOKEN_REDIR_OUT)
					print_error(">");
				else if (current->type == TOKEN_APPEND)
					print_error(">>");
				return (1);
			}
		}
		current = current->next;
	}
	return (0);
}

int	check_pipe_after_redir(t_token *tokens)
{
	t_token	*current;
	t_token	*next;

	current = tokens;
	while (current)
	{
		if (is_redir_token(current->type))
		{
			next = current->next;
			if (next && next->type == TOKEN_WORD)
				next = next->next;
			else if (next && next->type == TOKEN_PIPE)
			{
				printf("minishell: syntax error near unexpected token `|'\n");
				return (1);
			}
		}
		current = current->next;
	}
	return (0);
}

int	check_syntax_errors(t_token *tokens)
{
	if (!tokens)
		return (1);
	if (check_unclosed_quotes(tokens))
		return (1);
	if (check_redir_without_file(tokens))
		return (1);
	if (check_pipe_position(tokens))
		return (1);
	if (check_consecutive_pipes(tokens))
		return (1);
	if (check_trailing_redir(tokens))
		return (1);
	if (check_pipe_after_redir(tokens))
		return (1);
	if (check_consecutive_redirections(tokens))
		return (1);
	if (check_empty_command(tokens))
		return (1);
	return (0);
}
