#include "../../minishell.h"

struct s_global	g_data;

void free_tokens(t_token *tokens)
{
	t_token *tmp;
	
	while (tokens)
	{
		tmp = tokens;
		tokens = tokens->next;
		free(tmp->value);
		free(tmp);
	}
}

static void cleanup(t_token *tokens, t_redir *redirs, t_ast *head, char *input)
{
	if (tokens) free_tokens(tokens);
	if (input) free(input);
}

static int execute_command_sequence(char *input, t_shell *sh)
{
	t_ast *head = NULL;
	t_redir *redirs = NULL;
	t_token *tokens = NULL;

	tokens = lexer(input);
	if (!tokens)
	{
		printf("Lexer error in input: %s\n", input);
		free(input);
		return 1;
	}
	expand_tokens(&tokens, sh->env);
	head = build_ast(tokens);
	if (!head)
	{
		printf("Parser error!\n");
		cleanup(tokens, redirs, head, input);
		return 1;
	}
	prepare_all_herdocs(head); // hafin lmoxkil almangoli
	int status = execute_tree(head, 0, 1, -1, sh);
	cleanup(tokens, redirs, head, input);
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

int main(int ac, char **av, char **env)
{
	char *input;
	t_shell sh;
	(void)ac;
	(void)av;

	signals();
	get_env(&(sh.env_lst), env);
	while (1)
	{
		const char *prompt = "\001\033[0;31m\002MINISHELL𒉭 > \001\033[0m\002";
		input = readline(prompt);
		if (!input)
			exit (1);
		add_history(input);
		input = expand_line(input, sh.env_lst);
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
		while (wait(NULL) > 0);
	}    
	return 0;
}
