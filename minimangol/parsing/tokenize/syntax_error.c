
#include "../../minishell.h"

int	is_redir_token(t_token_type type)
{
	return (type == TOKEN_REDIR_IN || type == TOKEN_REDIR_OUT || 
			type == TOKEN_APPEND || type == TOKEN_HEREDOC);
}

int	is_operator_token(t_token_type type)
{
	return (type == TOKEN_PIPE || is_redir_token(type));
}

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

int	check_pipe_position(t_token *tokens)
{
	t_token *current;
	t_token *last_token;

	current = tokens;
	last_token = NULL;
	if (current && current->type == TOKEN_PIPE)
	{
		printf("minishell: syntax error near unexpected token `|'\n");
		return (1);
	}    
	while (current)
	{
		if (current->type != TOKEN_WORD || current->value)
			last_token = current;
		current = current->next;
	}    
	if (last_token && last_token->type == TOKEN_PIPE)
	{
		printf("minishell: syntax error near unexpected token `|'\n");
		return (1);
	}
	return (0);
}

int check_consecutive_pipes(t_token *tokens)
{
	t_token *current;

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
					printf("minishell: syntax error near unexpected token `<'\n");
				else if (current->type == TOKEN_REDIR_OUT)
					printf("minishell: syntax error near unexpected token `>'\n");
				else if (current->type == TOKEN_APPEND)
					printf("minishell: syntax error near unexpected token `>>'\n");
				return (1);
			}
		}
		current = current->next;
	}
	return (0);
}

int check_pipe_after_redir(t_token *tokens)
{
	t_token *current;
	t_token *next;
	
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

int check_trailing_redir(t_token *tokens)
{
	t_token *current;
	t_token *last_token;

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

int check_empty_command(t_token *tokens)
{
	t_token		*current;
	int			has_command;

	has_command = 0;
	current = tokens;
	while (current)
	{
		if (current->type == TOKEN_PIPE)
		{
			if (!has_command)
			{
				printf("minishell: syntax error near unexpected token `|'\n");
				return (1);
			}
			has_command = 0;
		}
		else if (current->type == TOKEN_WORD)
		{
			has_command = 1;
		}
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
	t_token	*cur = tokens;
	int		out = 0;
	int		append = 0;

	while (cur)
	{
		if (cur->type == TOKEN_PIPE)
			out = append = 0;
		else if (update_redir_flags(cur->type, &out, &append))
			return (1);
		cur = cur->next;
	}
	return (0);
}

int check_syntax_errors(t_token *tokens)
{
	if (!tokens)
		return (1);
	if (check_unclosed_quotes(tokens))
		return (1);
	// if (check_redir_without_file(tokens))
	// 	return (1);
	if (check_pipe_position(tokens))
		return (1);    
	if (check_consecutive_pipes(tokens))
		return (1);    
	if (check_trailing_redir(tokens))
		return (1);
	// if (check_pipe_after_redir(tokens))
	// 	return (1);
	if (check_empty_command(tokens))
		return (1);
	return (0);
}
