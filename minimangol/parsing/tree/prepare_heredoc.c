/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prepare_heredoc.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marouane <marouane@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/24 12:46:47 by marouane          #+#    #+#             */
/*   Updated: 2025/05/24 15:42:13 by marouane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

void    read_froma_stdin(int fd)
{
    char    *line;

    line = get_next_line(0);
}

int prepare_one_heredoc(t_redir *redir)
{
    int arr[2];
    int pid;
    
    while (redir)
    {
        if (redir->type == TOKEN_HEREDOC)
        {
            pipe(arr);
            pid = fork();
            if (!pid)
                
        }
        redir = redir->next;
    }
}

int prepare_all_herdocs(t_ast *head)
{
    if (head->e_token_type == TOKEN_WORD)
    {
        
    }
}
