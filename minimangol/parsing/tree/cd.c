/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marouane <marouane@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/23 10:35:04 by marouane          #+#    #+#             */
/*   Updated: 2025/05/23 10:47:29 by marouane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

int	ft_cd(t_env *head, char *path)
{
	char	*old_pwd;
	char	*pwd;

	old_pwd = getcwd(NULL, 0);
	if (chdir(path) == -1)
	{
		perror("cd");
		free(old_pwd);
		return 1;
	}
	pwd = getcwd(NULL, 0);
	while (head)
	{
		if (!ft_strcmp("OLDPWD", head->key))
		{
			free(head->value);
			head->value = old_pwd;
		}
		else if (!ft_strcmp("PWD", head->key))
		{
			free(head->value);
			head->value = pwd;
		}
		head = head->next;
	}
	return (0);
}
