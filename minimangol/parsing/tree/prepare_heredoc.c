/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prepare_heredoc.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msaadaou <msaadaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/24 12:46:47 by marouane          #+#    #+#             */
/*   Updated: 2025/05/30 17:47:24 by msaadaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

void    read_froma_stdin(t_redir *redir)
{
    char    *line;
    char    *tmp;

    close(redir->fd[0]);
    printf("%s\n", redir->file);
    line = readline("> ");
	while (ft_strcmp(redir->file, line))
	{
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


int prepare_one_heredoc(t_redir *redir)
{
    int pid;
    
    while (redir)
    {
        if (redir->type == TOKEN_HEREDOC)
        {
            pipe(redir->fd);
            pid = fork();
            if (!pid)
                read_froma_stdin(redir);
            else if (pid < 0)
                perror("pipe: ");
            else
                waitpid(pid, NULL, 0);
        }
        redir = redir->next;
    }
    return (0);
}

int prepare_all_herdocs(t_ast *head)
{
    if (!head)
        return (0);
    if (head->e_token_type == TOKEN_WORD)
        prepare_one_heredoc(head->redirs);
    prepare_all_herdocs(head->left);
    prepare_all_herdocs(head->right);
    return (0);
}

int close_all_herdocs(t_redir *redirs)
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
