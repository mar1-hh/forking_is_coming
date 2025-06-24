/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes_handling.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
<<<<<<< HEAD
/*   By: achat <achat@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/18 08:01:11 by marouane          #+#    #+#             */
/*   Updated: 2025/06/24 11:45:39 by achat            ###   ########.fr       */
=======
/*   By: msaadaou <msaadaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/18 08:01:11 by marouane          #+#    #+#             */
/*   Updated: 2025/06/24 12:15:40 by msaadaou         ###   ########.fr       */
>>>>>>> 9253f33e8628bd0bbce17f831c2631abb4d304b0
/*                                                                            */
/* ************************************************************************** */


<<<<<<< HEAD

void	add_pipe_front(t_pipe **head, t_ast *node)
{
	t_pipe	*lst;

	if (!head)
		return ;+
		
	lst = malloc(sizeof(t_pipe));
	if (!lst)
		return ;
	lst->node = node;
	lst->next = *head;
	*head = lst;
}


t_pipe	*create_pipe_lst(t_ast *node)
{
	t_pipe	*head;

	head = NULL;
	while (node->e_token_type == TOKEN_PIPE)
	{
		add_pipe_front(&head, node->right);
		if (node->left->e_precedence != TOKEN_PIPE)
			add_pipe_front(&head, node->left);
		node = node->left;
	}
	return (head);
}
=======
>>>>>>> 9253f33e8628bd0bbce17f831c2631abb4d304b0
