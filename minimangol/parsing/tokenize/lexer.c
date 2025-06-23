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

static int	handle_operator(char *input, int i, t_token **tokens)
{
	char	*value;
	int		type;
	int		len;

	set_operator_type(input[i], input[i + 1], &type, &len);
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

int	handle_lex_token(char *input, int i, t_token **tokens, int *is_space)
{
	int	result;

	if (ft_strchr("><|", input[i]))
		result = handle_operator(input, i, tokens);
	else if (input[i] == '\'' || input[i] == '"')
	{
		result = handle_quotes(input, i, tokens, input[i], *is_space);
		*is_space = 0;
	}
	else
	{
		result = handle_word(input, i, tokens, *is_space);
		*is_space = 0;
	}
	return (result);
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
		result = handle_lex_token(input, i, &tokens, &is_space);
		if (result == -1)
		{
			free_tokens(tokens);
			return (NULL);
		}
		i = result;
	}
	return (tokens);
}
