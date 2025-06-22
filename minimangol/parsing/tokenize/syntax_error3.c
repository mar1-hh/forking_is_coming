/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax_error3.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achat <achat@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/22 10:41:53 by achat             #+#    #+#             */
/*   Updated: 2025/06/22 10:44:38 by achat            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	print_error(char *s)
{
	printf("minishell: syntax error near unexpected token `%s'\n", s);
}

int	is_redir_token(t_token_type type)
{
	return (type == TOKEN_REDIR_IN || type == TOKEN_REDIR_OUT || 
		type == TOKEN_APPEND || type == TOKEN_HEREDOC);
}

int	is_operator_token(t_token_type type)
{
	return (type == TOKEN_PIPE || is_redir_token(type));
}

int	check_consecutive_redirections(t_token *tokens)
{
	t_token	*curr;

	curr = tokens;
	while (curr && !is_redir_token(curr->type))
		curr = curr->next;
	if (curr && curr->next && is_redirection(curr->type) 
		&& is_redirection(curr->next->type))
	{
		printf("syntax error near unexpected token `%s'\n", curr->next->value);
		return (1);
	}
	return (0);
}

int	check_trailing_redir(t_token *tokens)
{
	t_token	*current;
	t_token	*last_token;

	current = tokens;
	last_token = NULL;
	while (current)
	{
		last_token = current;
		current = current->next;
	}
	if (last_token && is_redir_token(last_token->type))
	{
		if (last_token->type == TOKEN_REDIR_IN)
			printf("minishell: syntax error near unexpected token `newline'\n");
		else if (last_token->type == TOKEN_REDIR_OUT)
			printf("minishell: syntax error near unexpected token `newline'\n");
		else if (last_token->type == TOKEN_APPEND)
			printf("minishell: syntax error near unexpected token `newline'\n");
		else if (last_token->type == TOKEN_HEREDOC)
			printf("minishell: syntax error near unexpected token `newline'\n");
		return (1);
	}
	return (0);
}
