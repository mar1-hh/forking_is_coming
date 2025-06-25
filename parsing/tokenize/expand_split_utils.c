/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_split_utils.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achat <achat@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 15:08:15 by msaadaou          #+#    #+#             */
/*   Updated: 2025/06/25 17:57:18 by achat            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

int	count_words(const char *s, char c)
{
	size_t	i;
	int		flag;
	int		counter;

	i = 0;
	flag = 0;
	counter = 0;
	while (s[i])
	{
		if (s[i] != c)
		{
			if (!flag)
				counter++;
			flag = 1;
		}
		else
			flag = 0;
		i++;
	}
	return (counter);
}
