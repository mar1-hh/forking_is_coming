#include "../../minishell.h"

struct s_global	g_data;

static void cleanup(t_token *tokens, t_redir *redirs, t_ast *head, char *input)
{
	if (tokens)
		free_tokens(tokens);
	if (input)
		free(input);
	if (head)
		free_tree(head);
}

void	print_lst(t_token *token)
{
	while (token)
	{
		printf("%s\n", token->value);
		token = token->next;
	}
}

int	wai_st(t_ast *node)
{
	int	status;
	

	if (node->right && node->right->is_pipe)
	{
		waitpid(node->right->pid, &status, 0);
	}
	else if (node->e_token_type == TOKEN_WORD)
	{
		// if (!is_builtin(node->args[0]))
		waitpid(node->pid, &status, 0);
	}
	while (wait(NULL) > 0);
	return (WEXITSTATUS(status));
}

static int execute_command_sequence(char *input, t_shell *sh)
{
	t_ast *head = NULL;
	t_redir *redirs = NULL;
	t_token *tokens = NULL;
	t_token	*new;

	tokens = lexer(input);
	if (!tokens)
	{
		printf("Lexer error in input: %s\n", input);
		free(input);
		return 1;
	}
	expand_tokens(&tokens, sh);
	// printf("1337\n");
	new = joining_tokens(tokens);
	head = build_ast(new);
	if (!head)
	{
		printf("Parser error!\n");
		cleanup(new, redirs, head, input);
		return 1;
	}
	prepare_all_herdocs(head, sh);
	execute_tree(head, 0, 1, -1, sh);
	if (head->e_token_type == TOKEN_PIPE || (head->args && !is_builtin(head->args[0])))
		sh->exit_status = wai_st(head);
	cleanup(tokens, redirs, head, input);
	free_tokens(new);
	return 0;
}

static void	handle_quit(int sign)
{
	pid_t	pid;
	int		status;

	(void)sign;
	pid = waitpid(-1, &status, 0);
	if (pid == -1)
		SIG_IGN ;
	else if (!g_data.hd)
	{
		write(1, "Quit (core dumped)\n", 20);
		return ;
	}
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

void	some_inits(t_shell *sh)
{
	signals();
	sh->exit_status = 0;
	sh->env_lst = NULL;
}

int main(int ac, char **av, char **env)
{
	char *input;
	t_shell sh;
	(void)ac;
	(void)av;

	some_inits(&sh);
	get_env(&(sh.env_lst), env);
	while (1)
	{
		const char *prompt = "\001\033[0;31m\002MINISHELL𒉭 > \001\033[0m\002";
		input = readline(prompt);
		if (!input)
		{
			printf("exit\n");
			exit (0);
		}
		add_history(input);
		if (!input)
		{
			printf("\thala!\n");
			break;
		}
		if (ft_strlen(input) == 0)
		{
			free(input);
			continue;
		}
		execute_command_sequence(input, &sh);
	}
	return 0;
}
