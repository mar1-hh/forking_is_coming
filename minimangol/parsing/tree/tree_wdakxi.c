/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tree_wdakxi.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achat <achat@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/21 23:22:29 by achat             #+#    #+#             */
/*   Updated: 2025/06/21 23:22:29 by achat            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

t_token	*advance_token(t_token *token, int steps)
{
	while (token && steps-- > 0)
		token = token->next;
	return (token);
}

static void initialize_ast_fields(t_ast *node, t_token_type type, t_shell *sh)
{
	node->e_token_type = type;
	node->cmd = NULL;
	node->args = NULL;
	node->sh = sh;
	node->arg_count = 0;
	node->ar_pipe = NULL;
	node->redirs = NULL;
	node->left = NULL;
	node->right = NULL;
	node->pid = 0;
	node->is_pipe = 0;
}

t_ast *create_ast_node(t_token_type type, t_shell *sh)
{
	t_ast *node = malloc(sizeof(t_ast));
	if (!node)
		return NULL;
	initialize_ast_fields(node, type, sh);
	return node;
}


static void unlink_redir_pair(t_token **head, t_token *prev, t_token *target)
{
	if (prev)
		prev->next = target->next;
	else
		*head = target->next;
}

t_token *remove_redirection_tokens(t_token **head)
{
	t_token	*current;
	t_token	*prev;
	t_token	*redir_target;

	prev = NULL;
	current = *head;
	while (current)
	{
		if (is_redirection(current->type))
		{
			redir_target = current->next;
			if (!redir_target || redir_target->type != TOKEN_WORD)
			{
				current = current->next;
				continue ;
			}
			unlink_redir_pair(head, prev, redir_target);
			current = redir_target->next;
		}
		else
		{
			prev = current;
			current = current->next;
		}
	}
	return *head;
}

static char **grow_args_array(char **args, int *capacity)
{
	*capacity *= 2;
	return ft_realloc(args, sizeof(char *) * (*capacity));
}

static void copy_args_to_cmd_node(t_ast *cmd_node, char **args, int count)
{
	int i;

	i = 0;
	cmd_node->args = malloc(sizeof(char *) * (count + 1));
	if (!cmd_node->args)
		return;
	while(i < count)
	{
		cmd_node->args[i] = args[i];
		i++;
	}
	cmd_node->args[count] = NULL;
	cmd_node->arg_count = count;
	cmd_node->cmd = ft_strdup(cmd_node->args[0]);
}

static int culc_words(t_token *token)
{
	int count;

	count = 0;
	while (token && token->type != TOKEN_PIPE)
	{
		if (token->type == TOKEN_WORD)
			count++;
		else if (is_redirection(token->type))
		{
			token = token->next;
			if (token && token->type == TOKEN_WORD)
				token = token->next;
			continue;
		}
		token = token->next;
	}
	return (count + 1);
}

t_token *merge_consecutive_words(t_token *tokens, t_ast *cmd_node)
{
	t_token *current = tokens;
	char **temp_args;
	int word_count = 0;
	int capacity = culc_words(tokens);
	temp_args = malloc(sizeof(char *) * capacity);


	if (!temp_args)
		return tokens;
	while (current && current->type != TOKEN_PIPE)
	{
		if (current->type == TOKEN_WORD)
		{
			if (word_count >= capacity - 1)
			{
				temp_args = grow_args_array(temp_args, &capacity);
				if (!temp_args)
					return tokens;
			}
			temp_args[word_count++] = ft_strdup(current->value);
			current = current->next;
		}
		else if (is_redirection(current->type))
		{
			current = current->next;
			if (current && current->type == TOKEN_WORD)
				current = current->next;
		}
		else
			current = current->next;
	}
	if (word_count > 0)
		copy_args_to_cmd_node(cmd_node, temp_args, word_count);
	free(temp_args);
	return current;
}

t_ast *build_command_node(t_token **tokens, t_shell *sh)
{
	t_ast *cmd_node;

	cmd_node = create_ast_node(TOKEN_WORD, sh);
	if (!cmd_node)
		return (NULL);
	cmd_node->redirs = handle_redir(tokens);
	*tokens = merge_consecutive_words(*tokens, cmd_node);
	return (cmd_node);
}

t_ast	*connect_pipe_nodes(t_token **tokens, t_shell *sh)
{
	t_ast	*left;
	t_ast	*pipe_node;

	left = build_command_node(tokens, sh);
	if (!left) 
		return (NULL);
	while (*tokens && (*tokens)->type == TOKEN_PIPE)
	{
		pipe_node = create_ast_node(TOKEN_PIPE, sh);
		if (!pipe_node)
		{
			free_tree(left);
			return (NULL);
		}
		pipe_node->left = left;
		*tokens = (*tokens)->next;
		pipe_node->right = build_command_node(tokens, sh);
		if (!pipe_node->right)
		{
			free_tree(pipe_node);
			return (NULL);
		}
		left = pipe_node;
	}
	return (left);
}

t_ast	*build_ast(t_token *tokens, t_shell *sh)
{
	return (connect_pipe_nodes(&tokens, sh));
}
