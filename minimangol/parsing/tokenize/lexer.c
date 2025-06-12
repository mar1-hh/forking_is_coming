#include "../../minishell.h"

char *ft_strjoin_char(char *str, char c)
{
	char    *new;
	size_t  len;

	if (!str)
	{
		new = malloc(2);
		new[0] = c;
		new[1] = '\0';
		return new;
	}
	len = ft_strlen(str);
	new = malloc(len + 2);
	ft_strlcpy(new, str, len + 1);
	new[len] = c;
	new[len + 1] = '\0';
	free(str);
	return new;
}

bool is_redirection(t_token_type type)
{
	return (type == TOKEN_REDIR_IN || 
			type == TOKEN_REDIR_OUT ||
			type == TOKEN_APPEND ||
			type == TOKEN_HEREDOC);
}

t_redir *create_redir_node(t_token_type type, char *file)
{
	t_redir *new_redir = malloc(sizeof(t_redir));
	if (!new_redir)
		return NULL;
	new_redir->file = ft_strdup(file);
	if (!new_redir->file) 
	{
		free(new_redir);
		return NULL;
	}
	new_redir->type = type;
	new_redir->next = NULL;
	return new_redir;
}

void	add_tokens(t_token **head, char *value, int type, int is_space, int quote_type)
{
	t_token *new_token;
	t_token *last;

	new_token = malloc(sizeof(t_token));
	if (!new_token)
		return;
	
	new_token->value = ft_strdup(value);
	new_token->type = type;
	new_token->next = NULL;
	new_token->is_space = is_space;
	new_token->quote_type = quote_type;
	if (!*head)
	{
		*head = new_token;
		return;
	}

	last = *head;
	while (last->next)
		last = last->next;
	last->next = new_token;
}

void remove_token(t_token **head, t_token *to_remove, t_token *prev)
{
	if (!*head || !to_remove)
		return;
	
	if (prev)
		prev->next = to_remove->next;
	else
		*head = to_remove->next;
	
	free(to_remove->value);
	free(to_remove);
}

static int handle_operator(char *input, int i, t_token **tokens)
{
	int type;
	int len;

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

	char *value = ft_substr(input, i, len);
	add_tokens(tokens, value, type, -1, -1);
	free(value);
	return (i + len);
}

static int handle_quotes(char *input, int i, t_token **tokens, char quote, int is_space)
{
	int start = i;
	int	quote_type;
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

	char *value = ft_substr(input, start + 1, i - start - 1);
	// printf("%s %d\n", value, quote_type);
	add_tokens(tokens, value, TOKEN_WORD, is_space, quote_type);
	free(value);
	return (i + 1);
}


static int handle_word(char *input, int i, t_token **tokens, int is_space)
{
	int start = i;
	char	*value;

	while (input[i] && !ft_isspace(input[i]) && !ft_strchr(">'\"<|", input[i]))
		i++;
	value = ft_substr(input, start, i - start);
	add_tokens(tokens, value, TOKEN_WORD, is_space, -1);
	free(value);
	return (i);
}

t_token *lexer(char *input)
{
	t_token *tokens = NULL;
	int i = 0;
	int result;
	int	is_space;

	is_space = 1;
	while (input[i])
	{
		if (ft_isspace(input[i]))
		{
			is_space = 1;
			i++;
			continue;
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
		{
			// free_tokens(tokens);
			return (NULL);
		}
		i = result;
	}
	return (tokens);
}

t_redir *handle_redir(t_token **tokens)
{
    t_token *curr = *tokens;
    t_redir *redirs = NULL;
    t_redir *redir_tail = NULL;

    while (curr && curr->next && curr->type != TOKEN_PIPE)
    {
        if (is_redirection(curr->type))
        {
            t_redir *new_redir = create_redir_node(curr->type, curr->next->value);            
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
        {
            curr = curr->next;
        }
    }
    return redirs;
}

void print_redirs(t_redir *redirs)
{
	t_redir *current = redirs;
	printf("\n--- Redirections ---\n");
	while (current)
	{
		printf("Type: %d, File: '%s'\n", current->type, current->file);
		current = current->next;
	}
}

void free_redirs(t_redir *redirs)
{
	t_redir *current = redirs;
	while (current)
	{
		t_redir *next = current->next;
		free(current->file);
		free(current);
		current = next;
	}
}



// t_ast *merge_consecutive_words(t_token *tokens, t_ast *head)
// {
// 	t_token *current = tokens;
// 	int args = 0;
// 	while (current && current->next)
// 	{
// 		t_token *next = current->next;
		
// 		if (current->type == TOKEN_WORD && next->type == TOKEN_WORD)
// 		{
// 			char *merged = malloc(ft_strlen(current->value) + 
// 						  ft_strlen(next->value) + 2);
// 			if (!merged)
// 				return tokens;
// 			ft_strcpy(merged, current->value);
// 			ft_strcat(merged, " ");
// 			ft_strcat(merged, next->value);
// 			free(current->value);
// 			head->args[args++] = merged;
// 			current->next = next->next;
			
// 			free(next->value);
// 			free(next);
// 			printf("%s\n", current->value);
// 		}
// 		else
// 			current = current->next;
// 	}
// 	return head;
// }