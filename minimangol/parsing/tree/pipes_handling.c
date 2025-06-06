/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes_handling.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marouane <marouane@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/18 14:09:38 by marouane          #+#    #+#             */
/*   Updated: 2025/06/06 17:06:10 by marouane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"


void    flaging_pipe(t_ast *sequence)
{
    if (!sequence)
        return ;
    sequence->is_pipe = 1;
    flaging_pipe(sequence->left);
    flaging_pipe(sequence->right);
}
