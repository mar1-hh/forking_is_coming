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

