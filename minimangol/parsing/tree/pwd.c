/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msaadaou <msaadaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/23 13:46:05 by marouane          #+#    #+#             */
/*   Updated: 2025/06/19 16:14:50 by msaadaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

int	ft_pwd(t_env *env_lst)
{
	char	*ptr;

	ptr = getcwd(NULL, 0);
	if (!ptr)
	{
		
		return (1);
	}
	printf("%s\n", ptr);
	free(ptr);
	return (0);
}
