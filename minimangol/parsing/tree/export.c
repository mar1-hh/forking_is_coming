/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marouane <marouane@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 15:13:35 by marouane          #+#    #+#             */
/*   Updated: 2025/06/03 00:04:10 by marouane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

char	*token_join(t_token **start)
{
	t_token	*tmp;
	char	*rt_value;
	char	*rt_tmp;

	tmp = *start;
	rt_value = ft_strdup(tmp->type);
	tmp = tmp->next;
	while (tmp && !tmp->is_space)
	{
		rt_tmp = rt_value;
		rt_value = ft_strjoin(rt_value, tmp->value);
		free(rt_tmp);
		tmp = tmp->next;
	}
	*start = tmp;
	return (rt_value);
}

t_token	*joining_tokens(t_token *old_lst)
{
	t_token	*new_lst;
	char	*line;

	new_lst = NULL;
	while (old_lst)
	{
		line = token_join(&old_lst);
		add_token(&new_lst, line, )
	}
}

int env_export(t_env *lst)
{
	while (lst)
	{
		printf("declare -x %s=\"%s\"\n", lst->key, lst->value);
		lst = lst->next;
	}
	return (0);
}

int export_valide(char *ptr)
{
	if (!ft_isalpha(ptr[0]) && ptr[0] != '_')
	{
		printf("bash: export: `%s': not a valid identifier\n", ptr);
		return (0);
	}
	return (1);
}

int ft_export(t_env **lst, char **export_param)
{
	char	**mtr;
	char	*value;
	t_env	*temp;
	int     i;
	int     flag;
	
	if (!export_param[1])
	{
		env_export(*lst);
		return (0);
	}
	i = 1;
	while (export_param[i])
	{
		flag = 0;
		mtr = split_env(export_param[i]);
		if (!export_valide(mtr[0]))
		{
			i++;
			continue ;
		}
		value = ft_strdup(mtr[1]);
		free(mtr[1]);
		mtr[1] = value;
		temp = *lst;
		while (temp)
		{
			if (!ft_strcmp(temp->key, mtr[0]))
			{
				free(temp->value);
				temp->value = value;
				flag = 1;
				break;
			}
			temp = temp->next;
		}
		if (!flag)
			add_back_env(lst, new_env_node(mtr));
		i++;
	}
	return (0);
}
