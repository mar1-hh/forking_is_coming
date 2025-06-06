/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marouane <marouane@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 15:13:35 by marouane          #+#    #+#             */
/*   Updated: 2025/06/06 18:53:08 by marouane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

void	cp_lbakiya(t_token *old, t_token *new)
{
	new->is_space = 1;
	new->type = old->type;
	new->next = NULL;
	new->quote_type = old->quote_type;
	new->prev = old->prev;
}

t_token	*token_join(t_token **start)
{
	t_token	*tmp;
	t_token	*new_tk;
	char	*rt_tmp;

	new_tk = malloc(sizeof(t_token));
	if (!new_tk)
		return (NULL);
	tmp = *start;
	new_tk->value = ft_strdup(tmp->value);
	cp_lbakiya(tmp, new_tk);
	tmp = tmp->next;
	while (tmp && !tmp->is_space)
	{
		rt_tmp = new_tk->value;
		new_tk->value = ft_strjoin(new_tk->value, tmp->value);
		free(rt_tmp);
		tmp = tmp->next;
	}
	*start = tmp;
	return (new_tk);
}

void	add_f_lkhr(t_token **head, t_token *new)
{
	t_token	*tmp;
	
	tmp = *head;
	if (!*head)
	{
		new->next = *head;
		*head = new;
		return ;
	}
	while (tmp->next)
		tmp = tmp->next;
	tmp->next = new;
}

t_token	*joining_tokens(t_token *old_lst)
{
	t_token	*new_lst;
	t_token	*new_node;

	new_lst = NULL;
	while (old_lst)
	{
		new_node = token_join(&old_lst);
		add_f_lkhr(&new_lst, new_node);
	}
	return (new_lst);
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
	int		st;
	
	st = 0;
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
			st = 1;
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
	return (st);
}
