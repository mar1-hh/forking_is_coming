/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msaadaou <msaadaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/23 13:51:21 by marouane          #+#    #+#             */
/*   Updated: 2025/05/28 15:44:49 by msaadaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

int ft_echo(char **args)
{
	int	i;
	int	flag;

	flag = 0;
	i = 1;
	if (args[1] && !ft_strncmp(args[1], "-n", ft_strlen(args[1])))
	{
		flag = 1;
		i++;
	}
	while (args[i])
	{
		if (i != 1)
			printf(" ");
		printf("%s", args[i]);
		i++;
	}
	if (!flag)
		printf("\n");
	return (0);
}
