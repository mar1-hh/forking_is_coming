/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prepare_env.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marouane <marouane@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/18 16:21:39 by marouane          #+#    #+#             */
/*   Updated: 2025/05/24 18:28:57 by marouane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

int	size_of_var(char *str)
{
	int	i;

	
	i = 0;
	while (str[i] && str[i] != '=')
		i++;
	return (i);
}

char	**split_env(char *str)
{
	char	**mtr;
	int		i;
	int		size;

	i = 0;
	mtr = malloc(2 * sizeof(char *));
	size = size_of_var(str);
	mtr[0] = malloc(size + 1);
	ft_memcpy(mtr[0], str, size + 1);
	mtr[0][size] = 0;
	mtr[1] = malloc(ft_strlen(str + size + 1) + 1);
	ft_memcpy(mtr[1], str + size + 1, ft_strlen(str + size + 1) + 1);
	return (mtr); 
}


t_env	*new_env_node(char **mtr)
{
	t_env	*node;

	node = malloc(sizeof(t_env));
	if (!node)
		return (NULL);
	node->key = mtr[0];
	node->value = mtr[1];
	node->next = NULL;
	return (node);
}

void	add_back_env(t_env **lst, t_env *new)
{
	t_env	*temp;

	if (!lst || !new)
		return ;
	if (!(*lst))
		*lst = new;	
	else
	{
		temp = *lst;
		while (temp->next)
			temp = temp->next;
		temp->next = new;
	}
}


void	get_env(t_env **lst, char **env)
{
	int		i;
	char	**mtr;

	if (!env)
		return ;
	i = 0;
	while (env[i])
	{
		mtr = split_env(env[i]);
		add_back_env(lst, new_env_node(mtr));
		free(mtr);
		i++;
	}
}

int ft_env(t_env *lst)
{
	while (lst)
	{
		printf("%s=%s\n", lst->key, lst->value);
		lst = lst->next;
	}
	return (0);
}
