/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_input_sign.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msaadaou <msaadaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 15:24:03 by msaadaou          #+#    #+#             */
/*   Updated: 2025/06/25 16:22:20 by msaadaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

static void	handle_sign(int sign)
{
	pid_t	pid;
	int		status;

	pid = waitpid(-1, &status, 0);
	(void)sign;
	write(2, "\n", 1);
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
	// char	**mtr;
	(void)env;
	signals();
	sh->env_lst = NULL;
	// if (!env[0])
	// {
	// 	add_back_env(&sh->env_lst, new_env_node(mtr, 1));
	// 	free_mtx(mtr);
	// }
	sh->exit_status = 0;
}

char	*get_user_input(void)
{
	const char	*prompt = "\001\033[0;31m\002MINISHELL𒉭 > \001\033[0m\002";
	char		*input;

	input = readline(prompt);
	if (!input)
	{
		printf("exit\n");
		exit(1);
	}
	add_history(input);
	return (input);
}

int	handle_input(char *input, t_shell *sh)
{
	if (ft_strlen(input) == 0)
	{
		free(input);
		return (1);
	}
	if (execute_command_sequence(input, sh))
		return (1);
	return (0);
}
