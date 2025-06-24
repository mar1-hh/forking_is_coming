/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achat <achat@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/21 23:22:14 by achat             #+#    #+#             */
/*   Updated: 2025/06/21 23:22:14 by achat            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

struct s_global	g_data;

static void	cleanup(t_token *tokens, t_ast *head, char *input)
{
	if (tokens)
		free_tokens(tokens);
	if (input)
		free(input);
	if (head)
		free_tree(head);
}

int	wai_st(t_ast *node)
{
	int	status;

	if (node->right && node->right->is_pipe)
	{
		waitpid(node->right->pid, &status, 0);
	}
	else if (node->e_token_type == TOKEN_WORD)
		waitpid(node->pid, &status, 0);
	while (wait(NULL) > 0);
	return (WEXITSTATUS(status));
}

void	print_tk(t_token *tk)
{
	while (tk)
	{
		printf("%s\n", tk->value);
		tk = tk->next;
	}
}

void	trim_first(t_token *tok)
{
	char	*tmp;

	tmp = tok->value;
	if (tmp[0] == ' ')
	{
		tok->value = ft_strdup(tmp + 1);
		free(tmp);
	}
}

void	trim_last(t_token *tok)
{
	char	*tmp;
	int		size;

	tmp = tok->value;
	size = ft_strlen(tmp);
	if (tmp[size - 1] == ' ')
	{
		tok->value = ft_calloc(1, size);
		ft_memcpy(tok->value, tmp, size - 1);
		free(tmp);
	}
}

void	trim_first_last(t_token *lst)
{
	int	i;

	i = 0;	
	while (lst)
	{
		if (!i && lst->quote_type == -1)
			trim_first(lst);
		if (!lst->next && lst->quote_type == -1)
			trim_last(lst);
		lst = lst->next;
		i++;
	}
}

void targaryan(t_token *tokens, t_ast *head, t_shell *sh, t_token *new)
{
	expand_tokens(&tokens, sh);
	trim_first_last(tokens);
	new = joining_tokens(tokens);
	head = build_ast(new, sh);
	sh->exit_status = prepare_herdoc(head, sh);
	if (sh->exit_status)
		return (1);
	execute_tree(head, 0, 1, -1);
	if (head->e_token_type == TOKEN_PIPE || 
		(head->args && !is_builtin(head->args[0])))
		sh->exit_status = wai_st(head);
}

static int	execute_command_sequence(char *input, t_shell *sh)
{
	t_ast	*head;
	t_token	*tokens;
	t_token	*new;
`
	head = NULL;
	tokens = NULL;
	tokens = lexer(input);
	if (check_syntax_errors(tokens))
	{
		sh->exit_status = 258;
		free_tokens(tokens);
		return (1);
	}
	if (!tokens)
	{
		printf("Lexer error in input: %s\n", input);
		free(input);
		return (1);
	}
	targaryan(tokens, head, sh, new);
	cleanup(tokens, head, input);
	free_tokens(new);
	return (0);
}

static void	handle_sign(int sign)
{
	pid_t	pid;
	int		status;

	pid = waitpid(-1, &status, 0);
	(void)sign;
	write(2, "\n", 1);
	if (g_data.hd)
	{
		g_data.interrupted = 1;
		return ;
	}
	if (pid == -1)
	{
		rl_replace_line("", 0);
		rl_on_new_line();
		rl_redisplay();
	}
}

static void	signals(void)
{
	signal(SIGINT, handle_sign);
	signal(SIGQUIT, SIG_IGN);
}

void	some_inits(t_shell *sh, char **env)
{
	char	**mtr;
	
	signals();
	sh->env_lst = NULL;
	if (!env[0])
	{
		mtr = split_env("PATH=/Users/msaadaou/goinfre/homebrew/bin:/Users/msaadaou/goinfre/homebrew/bin:/Users/msaadaou/goinfre/homebrew/bin:/Users/msaadaou/goinfre/homebrew/bin:/Users/msaadaou/goinfre/homebrew/bin:/Users/msaadaou/goinfre/homebrew/bin:/Users/msaadaou/goinfre/homebrew/bin:/Users/msaadaou/.brew/bin:/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin:/usr/local/munki:/Users/msaadaou/.brew/bin");
		add_back_env(&sh->env_lst, new_env_node(mtr));
		free_mtx(mtr);
	}
	sh->exit_status = 0;
}

int	main(int ac, char **av, char **env)
{
	char		*input;
	t_shell		sh;
	const char	*prompt;

	(void) ac;
	(void) av;
	some_inits(&sh, env);
	get_env(&(sh.env_lst), env);
	while (1)
	{
		prompt = "\001\033[0;31m\002MINISHELL𒉭 > \001\033[0m\002";
		input = readline(prompt);
		if (!input)
		{
			printf("exit\n");
			exit (1);
		}
		add_history(input);
		if (ft_strlen(input) == 0)
		{
			free(input);
			continue ;
		}
		if (execute_command_sequence(input, &sh))
			continue ;
	}
	free_env(sh.env_lst);
	return (0);
}
