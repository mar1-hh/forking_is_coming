/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marouane <marouane@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 15:13:35 by marouane          #+#    #+#             */
/*   Updated: 2025/05/24 18:29:27 by marouane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

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
        value = ft_strtrim(mtr[1], "\"");
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
