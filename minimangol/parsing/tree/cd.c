/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msaadaou <msaadaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/23 10:35:04 by marouane          #+#    #+#             */
/*   Updated: 2025/06/21 13:24:54 by msaadaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

int	cd_err(char *old_pwd)
{
	perror("cd");
	free(old_pwd);
	return 1;
}

void	update_old_new(t_env *head, char *old_pwd, char *pwd)
{
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
}

int	to_home(t_env *head, char *old_pwd)
{
	char	*pwd;
	t_env	*tmp;

	pwd = NULL;
	tmp = head;
	while (head)
	{
		if (!ft_strcmp(head->key, "HOME"))
			pwd = ft_strdup(head->value);
		head = head->next;
	}
	if (!pwd)
	{
		printf("minishell: cd: HOME not set\n");
		return (1);
	}
	chdir(pwd);
	free(pwd);
	pwd = getcwd(NULL, 0);
	update_old_new(tmp, old_pwd, pwd);
	return (0);
}

int	ft_cd(t_env *head, char *path)
{
	char	*old_pwd;
	char	*pwd;

	old_pwd = getcwd(NULL, 0);
	if (!path)
		return (to_home(head, old_pwd));
	if (chdir(path) == -1)
		return (cd_err(old_pwd));
	pwd = getcwd(NULL, 0);
	update_old_new(head, old_pwd, pwd);
	return (0);
}
