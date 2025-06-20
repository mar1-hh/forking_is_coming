#include "../../minishell.h"

int is_redir_token(t_token_type type)
{
	return (type == TOKEN_REDIR_IN || type == TOKEN_REDIR_OUT || 
			type == TOKEN_APPEND || type == TOKEN_HEREDOC);
}

int is_operator_token(t_token_type type)
{
	return (type == TOKEN_PIPE || is_redir_token(type));
}

int check_unclosed_quotes(t_token *tokens)
{
	t_token		*current;
	int			single_quote_open;
	int			double_quote_open;
	
	current = tokens;
	single_quote_open = 0;
	double_quote_open = 0;
	while (current)
	{
		if (current->value)
		{
			char *str = current->value;
			int i = 0;
			while (str[i])
			{
				if (str[i] == '\'' && !double_quote_open)
					single_quote_open = !single_quote_open;
				else if (str[i] == '"' && !single_quote_open)
					double_quote_open = !double_quote_open;
				i++;
			}
		}
		current = current->next;
	}
	if (single_quote_open || double_quote_open)
	{
		if (single_quote_open)
			printf("minishell: syntax error: unclosed single quote\n");
		else
			printf("minishell: syntax error: unclosed double quote\n");
		return (1);
	}
	return (0);
}

int check_pipe_position(t_token *tokens)
{
	t_token *current = tokens;
	t_token *last_token = NULL;
	
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
	t_token *current = tokens;
	
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
			while (next && next->is_space)
				next = next->next;            
			if (!next || next->type != TOKEN_WORD)
			{
				if (current->type == TOKEN_REDIR_IN)
					printf("minishell: syntax error near unexpected token `<'\n");
				else if (current->type == TOKEN_REDIR_OUT)
					printf("minishell: syntax error near unexpected token `>'\n");
				else if (current->type == TOKEN_APPEND)
					printf("minishell: syntax error near unexpected token `>>'\n");
				else if (current->type == TOKEN_HEREDOC)
					printf("minishell: syntax error near unexpected token `<<'\n");
				return (1);
			}
		}
		current = current->next;
	}
	return (0);
}

int check_consecutive_redirections(t_token *tokens)
{
	t_token		*current;
	t_token		*next;

	current = tokens;
	while (current && current->next)
	{
		if (is_redir_token(current->type))
		{
			next = current->next;            
			if (next && next->type == TOKEN_WORD)
				next = next->next;            
			if (next && next->type == current->type)
			{
				if (current->type == TOKEN_REDIR_IN)
					printf("minishell: syntax error near unexpected token `<'\n");
				else if (current->type == TOKEN_REDIR_OUT)
					printf("minishell: syntax error near unexpected token `>'\n");
				else if (current->type == TOKEN_APPEND)
					printf("minishell: syntax error near unexpected token `>>'\n");
				else if (current->type == TOKEN_HEREDOC)
					printf("minishell: syntax error near unexpected token `<<'\n");
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
			if (next && next->type == TOKEN_PIPE)
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

int check_conflicting_redirections(t_token *tokens)
{
	t_token *current = tokens;
	int has_stdout_redir = 0;
	int has_append_redir = 0;
	
	while (current)
	{
		if (current->type == TOKEN_PIPE)
		{
			has_stdout_redir = 0;
			has_append_redir = 0;
		}
		else if (current->type == TOKEN_REDIR_OUT)
		{
			if (has_stdout_redir || has_append_redir)
			{
				printf("minishell: syntax error: multiple output redirections\n");
				return (1);
			}
			has_stdout_redir = 1;
		}
		else if (current->type == TOKEN_APPEND)
		{
			if (has_stdout_redir || has_append_redir)
			{
				printf("minishell: syntax error: multiple output redirections\n");
				return (1);
			}
			has_append_redir = 1;
		}
		current = current->next;
	}
	return (0);
}

// int check_syntax_errors(t_token *tokens)
// {
// 	if (!tokens)
// 		return (1);
// 	if (check_unclosed_quotes(tokens))
// 		return (1);
// 	if (check_redir_without_file(tokens))
// 		return (1);
// 	if (check_pipe_position(tokens))
// 		return (1);    
// 	if (check_consecutive_pipes(tokens))
// 		return (1);    
// 	// if (check_trailing_redir(tokens))
// 	// 	return (1);
// 	if (check_pipe_after_redir(tokens))
// 		return (1);
// 	if (check_empty_command(tokens))
// 		return (1);
// 	// if (check_consecutive_redirections(tokens))
// 	// 	return (1);
// 	return (0);
// }

// void print_syntax_error(char *token)
// {
//     printf("minishell: syntax error near unexpected token `%s'\n", token);
// }
// int check_invalid_combinations(t_token *tokens)
// {
//     t_token *current = tokens;
	
//     while (current && current->next)
//     {
//         // Check for << followed by >>
//         if (current->type == TOKEN_HEREDOC && current->next->type == TOKEN_APPEND)
//         {
//             printf("minishell: syntax error near unexpected token `>>'\n");
//             return (1);
//         }
		
//         // Check for >> followed by <<
//         if (current->type == TOKEN_APPEND && current->next->type == TOKEN_HEREDOC)
//         {
//             printf("minishell: syntax error near unexpected token `<<'\n");
//             return (1);
//         }
		
//         current = current->next;
//     }
//     return (0);
// }
