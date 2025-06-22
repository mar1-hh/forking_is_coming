/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax_error2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achat <achat@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/22 10:37:40 by achat             #+#    #+#             */
/*   Updated: 2025/06/22 12:56:17 by achat            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	check_pipe_position(t_token *tokens)
{
	t_token	*current;
	t_token	*last_token;

	current = tokens;
	last_token = NULL;
	if (current && current->type == TOKEN_PIPE)
	{
		printf("minishell: syntax error near unexpected token `|'\n");
		return (1);
	}
	return (0);
}

int	check_consecutive_pipes(t_token *tokens)
{
	t_token	*current;

	current = tokens;
	while (current && current->next)
	{
		if (current->type == TOKEN_PIPE && current->next->type == TOKEN_PIPE)
		{
			printf("minishell: syntax error near unexpected token `|'\n");
			return (1);
		}
		current = current->next;
	}
	return (0);
}

int check_empty_command(t_token *tokens)
{
	t_token		*current;
	int			has_command;

	has_command = 0;
	current = tokens;
	while (current)
	{
		if (current->next && current->next->value)
		{
			if (!has_command && current->type == TOKEN_PIPE)
			{
				print_error(current->next->value);
				return (1);
			}
			has_command = 0;
		}
		else if (current->next && 
			current->type == TOKEN_PIPE && !current->next->value)
		{
			print_error("newline");
			return (1);
		}
		else if (current->type == TOKEN_WORD)
			has_command = 1;
		current = current->next;
	}
	return (0);
}

static int	update_redir_flags(int type, int *out, int *append)
{
	if (type == TOKEN_REDIR_OUT || type == TOKEN_APPEND)
	{
		if (*out || *append)
		{
			printf("minishell: syntax error: multiple output redirections\n");
			return (1);
		}
		if (type == TOKEN_REDIR_OUT)
			*out = 1;
		else
			*append = 1;
	}
	return (0);
}

int	check_conflicting_redirections(t_token *tokens)
{
	t_token	*cur;
	int		out;
	int		append;

	cur = tokens;
	out = 0;
	append = 0;
	while (cur)
	{
		if (cur->type == TOKEN_PIPE)
		{
			out = 0;
			append = 0;
		}
		else if (update_redir_flags(cur->type, &out, &append))
			return (1);
		cur = cur->next;
	}
	return (0);
}
