/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marouane <marouane@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/23 13:39:43 by marouane          #+#    #+#             */
/*   Updated: 2025/05/23 13:40:56 by marouane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

void	rm_node(t_env *lst, char *str)
{
	t_env	*tmp;
	
	while (lst->next)
	{
		if (!ft_strncmp(lst->next->key, str, ft_strlen(str)))
		{
			tmp = lst->next;
			lst->next = tmp->next;
			free(tmp->key);
			free(tmp->value);
			free(tmp);
			return ;
		}
		lst = lst->next;
	}
}

int	ft_unset(t_env **lst, char **argv)
{
	int	i;

	i = 1;
	while (argv[i])
	{
		rm_node(*lst, argv[i]);
		i++;
	}
	return (0);
}
