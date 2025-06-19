/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prepare_heredoc.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msaadaou <msaadaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/24 12:46:47 by marouane          #+#    #+#             */
/*   Updated: 2025/06/19 16:32:44 by msaadaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

void	sigint_heredoc_handler(int sig)
{
	(void)sig;
	write(1, "\n", 1);
	exit(1);
}

void	read_froma_stdin(t_redir *redir, t_shell *sh)
{
	char	*line;
	char	*tmp;
	char	*tmp_2;

	close(redir->fd[0]);
	line = readline("> ");
	while (ft_strcmp(redir->file, line))
	{
		if (redir->is_expand)
			line = expand_line(&line, sh, NULL);
		tmp = ft_strjoin(line, "\n");
		write(redir->fd[1], tmp, ft_strlen(tmp));
		free(line);
		free(tmp);
		line = readline("> ");
	}
	free(line);
	close(redir->fd[1]);
	exit(0);
}

int	prepare_one_heredoc(t_redir *redir, t_shell *sh)
{
	int	pid;
	int	st;

	while (redir)
	{
		if (redir->type == TOKEN_HEREDOC)
		{
			pipe(redir->fd);
			pid = fork();
			if (!pid)
			{
				signal(SIGINT, sigint_heredoc_handler);
				read_froma_stdin(redir, sh);
			}
			else if (pid < 0)
				perror("pipe: ");
			else
			{
				waitpid(pid, &st, 0);
				sh->exit_status = WEXITSTATUS(st);
				if (WIFSIGNALED(st))
					return (1);
			}
		}
		redir = redir->next;
	}
	return (0);
}

int	prepare_all_herdocs(t_ast *head, t_shell *sh)
{
	if (!head)
		return (0);
	if (head->e_token_type == TOKEN_WORD)
		return (prepare_one_heredoc(head->redirs, sh));
	prepare_all_herdocs(head->left, sh);
	prepare_all_herdocs(head->right, sh);
	return (0);
}

int	close_all_herdocs(t_redir *redirs)
{
	while (redirs)
	{
		if (redirs->type == TOKEN_HEREDOC)
		{
			close(redirs->fd[0]);
			close(redirs->fd[1]);
		}
		redirs = redirs->next;
	}
	return (0);
}
