#include "../../minishell.h"

t_token	*advance_token(t_token *token, int steps)
{
	while (token && steps-- > 0)
		token = token->next;
	return (token);
}

static t_token *skip_redir_and_target(t_token *token)
{
	if (token && is_redirection(token->type))
	{
		token = token->next;
		if (token && token->type == TOKEN_WORD)
			token = token->next;
	}
	return token;
}

static void initialize_ast_fields(t_ast *node, t_token_type type)
{
	node->e_token_type = type;
	node->cmd = NULL;
	node->args = NULL;
	node->arg_count = 0;
	node->ar_pipe = NULL;
	node->redirs = NULL;
	node->left = NULL;
	node->right = NULL;
	node->pid = 0;
	node->is_pipe = 0;
}

t_ast *create_ast_node(t_token_type type)
{
	t_ast *node = malloc(sizeof(t_ast));
	if (!node)
		return NULL;
	initialize_ast_fields(node, type);
	return node;
}

static int is_redir_target(t_token *token)
{
	return token->prev && is_redirection(token->prev->type);
}

static int count_consecutive_words(t_token *token)
{
	int count = 0;

	while (token)
	{
		if (token->type == TOKEN_WORD)
		{
			if (is_redir_target(token))
			{
				token = token->next;
				continue;
			}
			count++;
		}
		else if (token->type == TOKEN_PIPE)
			break;
		token = token->next;
	}
	return count;
}

static void add_token_to_args(t_ast *cmd_node, char *value, int index)
{
	cmd_node->args[index] = ft_strdup(value);
}

static void fill_args_array_skip_redirs(t_ast *cmd_node, t_token *token, int count)
{
	int i = 0;

	while (token && i < count)
	{
		if (token->type == TOKEN_WORD)
		{
			if (is_redir_target(token))
			{
				token = token->next;
				continue;
			}
			add_token_to_args(cmd_node, token->value, i++);
		}
		else if (is_redirection(token->type))
		{
			token = skip_redir_and_target(token);
			continue;
		}
		else if (token->type == TOKEN_PIPE)
			break;
		token = token->next;
	}
	cmd_node->args[count] = NULL;
	cmd_node->arg_count = count;
	if (count > 0)
		cmd_node->cmd = ft_strdup(cmd_node->args[0]);
}

static void unlink_redir_pair(t_token **head, t_token *prev, t_token *target)
{
	if (prev)
		prev->next = target->next;
	else
		*head = target->next;
}

t_token *remove_redirection_tokens(t_token **head)
{
	t_token *current = *head;
	t_token *prev = NULL;

	while (current)
	{
		if (is_redirection(current->type))
		{
			t_token *redir_target = current->next;
			if (!redir_target || redir_target->type != TOKEN_WORD)
			{
				current = current->next;
				continue;
			}
			unlink_redir_pair(head, prev, redir_target);
			current = redir_target->next;
		}
		else
		{
			prev = current;
			current = current->next;
		}
	}
	return *head;
}

static char **grow_args_array(char **args, int *capacity)
{
	*capacity *= 2;
	return ft_realloc(args, sizeof(char *) * (*capacity));
}

static void copy_args_to_cmd_node(t_ast *cmd_node, char **args, int count, int *arr)
{
	int i;

	i = 0;
	cmd_node->args = malloc(sizeof(char *) * (count + 1));
	cmd_node->arr_space = malloc(sizeof(int) * (count + 1));;
	if (!cmd_node->args)
		return;
	while(i < count)
	{
		cmd_node->args[i] = args[i];
		cmd_node->arr_space[i] = arr[i];
		i++;
	}
	cmd_node->args[count] = NULL;
	cmd_node->arg_count = count;
	cmd_node->cmd = ft_strdup(cmd_node->args[0]);
}

static int culc_words(t_token *token)
{
	int count;

	count = 0;
	while (token && token->type != TOKEN_PIPE)
	{
		if (token->type == TOKEN_WORD)
			count++;
		else if (is_redirection(token->type))
		{
			token = token->next;
			if (token && token->type == TOKEN_WORD)
				token = token->next;
			continue;
		}
		token = token->next;
	}
	return (count + 1);
}

t_token *merge_consecutive_words(t_token *tokens, t_ast *cmd_node)
{
	t_token *current = tokens;
	char **temp_args;
	int word_count = 0;
	int capacity = culc_words(tokens);
	temp_args = malloc(sizeof(char *) * capacity);
	int	*arr;
	arr = malloc(sizeof(int) * capacity);

	if (!temp_args)
		return tokens;
	while (current && current->type != TOKEN_PIPE)
	{
		if (current->type == TOKEN_WORD)
		{
			if (word_count >= capacity - 1)
			{
				temp_args = grow_args_array(temp_args, &capacity);
				if (!temp_args)
					return tokens;
			}
			temp_args[word_count] = ft_strdup(current->value);
			arr[word_count++] = current->is_space;
			current = current->next;
		}
		else if (is_redirection(current->type))
		{
			current = current->next;
			if (current && current->type == TOKEN_WORD)
				current = current->next;
		}
		else
			current = current->next;
	}

	if (word_count > 0)
		copy_args_to_cmd_node(cmd_node, temp_args, word_count, arr);

	free(temp_args);
	// 5as tdi free matrix han ^
	return current;
}


t_ast *build_command_node(t_token **tokens)
{
	t_ast *cmd_node;
	t_token *original_tokens;

	original_tokens = *tokens;
	cmd_node = create_ast_node(TOKEN_WORD);
	if (!cmd_node)
		return (NULL);    
	cmd_node->redirs = handle_redir(tokens);    
	*tokens = merge_consecutive_words(*tokens, cmd_node);
	return (cmd_node);
}


t_ast *connect_pipe_nodes(t_token **tokens)
{
	t_ast *left;

	left = build_command_node(tokens);
	if (!left) 
		return (NULL);
	while (*tokens && (*tokens)->type == TOKEN_PIPE)
	{
		t_ast *pipe_node = create_ast_node(TOKEN_PIPE);
		if (!pipe_node) {
			free_ast(left);
			return NULL;
		}
		pipe_node->left = left;
		*tokens = (*tokens)->next;
		pipe_node->right = build_command_node(tokens);
		
		if (!pipe_node->right)
		{
			free_ast(pipe_node);
			return NULL;
		}
		left = pipe_node;
	}
	return left;
}

t_ast *build_ast(t_token *tokens)
{
	return connect_pipe_nodes(&tokens);
}

void free_ast(t_ast *ast)
{
	if (!ast)
		return;
	
	free_ast(ast->left);
	free_ast(ast->right);
	
	if (ast->cmd)
		free(ast->cmd);
	
	if (ast->args)
	{
		for (int i = 0; i < ast->arg_count; i++)
			free(ast->args[i]);
		free(ast->args);
	}
	
	t_redir *redir = ast->redirs;
	while (redir)
	{
		t_redir *next = redir->next;
		free(redir->file);
		free(redir);
		redir = next;
	}    
	free(ast);
}

void	compl_heredoc(t_ast *node, int *infd, int *outfd)
{
	t_redir	*lst;

	lst = node->redirs;
	while (lst)
	{
		if (lst->type == TOKEN_HEREDOC)
		{
			close(lst->fd[1]);
			*infd = lst->fd[0];
		}
		lst = lst->next;
	}
}

void	handle_redirection(t_ast *node, int *infd, int *outfd)
{
	t_redir	*lst;

	lst = node->redirs;
	// compl_heredoc(node, infd, outfd);
	while (lst)
	{
		if (lst->type == TOKEN_REDIR_IN)
		{
			*infd = open(lst->file, O_RDONLY);
			if (*infd == -1)
			{
				perror("open");
				return ;
			}
		}
		else if (lst->type == TOKEN_APPEND || lst->type == TOKEN_REDIR_OUT)
		{
			if (lst->type == TOKEN_APPEND)
				*outfd = open(lst->file, O_CREAT | O_APPEND | O_WRONLY, 0644);
			else
				*outfd = open(lst->file, O_CREAT | O_TRUNC | O_WRONLY, 0644);
			if (*outfd == -1)
			{
				perror("open");
				return ;
			}
		}
		else if (lst->type == TOKEN_HEREDOC)
		{
			close(lst->fd[1]);
			*infd = lst->fd[0];
		}
		lst = lst->next;
	}
}

int my_execve(t_ast *node, t_shell *sh)
{
	if (!ft_strcmp(node->args[0], "env"))
		return (ft_env(sh->env_lst));
	else if (!ft_strcmp(node->args[0], "export"))
		return (ft_export(&sh->env_lst, node->args));
	else if (!ft_strcmp(node->args[0], "cd"))
		return (ft_cd(sh->env_lst, node->args[1]));
	else if (!ft_strcmp(node->args[0], "unset"))
		return (ft_unset(&sh->env_lst, node->args));
	else if (!ft_strcmp(node->args[0], "pwd"))
		return (ft_pwd());
	else if (!ft_strcmp(node->args[0], "echo"))
		return (ft_echo(node->args));
	else if (!ft_strcmp(node->args[0], "exit"))
		ft_exit(0);

	return (1);
}

int execute_builtin(t_ast *node, int infd, int outfd, t_shell *sh)
{
	handle_redirection(node, &infd, &outfd);
	sh->stdinput_fl = dup(0);
	sh->stdout_fl = dup(1);
	if (infd)
	{
		dup2(infd, 0);
		close(infd);
	}
	if (outfd != 1)
	{
		dup2(outfd, 1);
		close(outfd);
	}
	my_execve(node, sh);
	// printf("***************1337*********************\n");
	dup2(sh->stdinput_fl, 0);
	dup2(sh->stdout_fl, 1);
	close(sh->stdinput_fl);
	close(sh->stdout_fl);
	return (0);
}

int	is_builtin(char *cmd)
{
	if (!ft_strcmp(cmd, "echo"))
		return (1);
	else if (!ft_strcmp(cmd, "cd"))
		return (1);
	else if (!ft_strcmp(cmd, "pwd"))
		return (1);
	else if (!ft_strcmp(cmd, "export"))
		return (1);
	else if (!ft_strcmp(cmd, "unset"))
		return (1);
	else if (!ft_strcmp(cmd, "env"))
		return (1);
	else if (!ft_strcmp(cmd, "exit"))
		return (1);
	return (0);
}

char	*helper_path_cmd(char **commands_path, char *cmd)
{
	int		i;
	char	*command_path;

	i = 0;
	while (commands_path[i])
	{
		// Use path separator correctly
		command_path = ft_strjoin(ft_strjoin(commands_path[i], "/"), cmd);
		if (!command_path)
		{
			// free_args(commands_path);
			free(cmd);
			return (NULL);
		}
		if (!access(command_path, X_OK))
		{
			// free_args(commands_path);
			free(cmd);
			return (command_path);
		}
		free(command_path);
		i++;
	}
	// free_args(commands_path);
	free(cmd);
	return (NULL);
}

char	*helper_path(char *str, char *cmd, int size)
{
	char	*complete_path;
	char	**commands_path;

	complete_path = ft_substr(str, size, ft_strlen(str) - size);
	if (!complete_path)
		return (NULL);
	commands_path = ft_split(complete_path, ':');
	if (!commands_path)
	{
		free(complete_path);
		return (NULL);
	}
	free(complete_path);
	return (helper_path_cmd(commands_path, cmd));
}

char	*debug_okda(char **env, char *cmd)
{
	char	*str;
	char	*cmdr;
	int		i;

	if (!cmd)
		return (NULL);
	if (!access(cmd, X_OK))
		return (ft_strdup(cmd));
	cmdr = ft_strjoin("/", cmd);
	if (!cmdr)
		return (NULL);
	i = 0;
	while (env[i])
	{
		str = ft_strnstr(env[i], "PATH=", ft_strlen("PATH="));
		if (str)
			return (helper_path(str, cmdr, ft_strlen("PATH=")));
		i++;
	}
	return (NULL);
}

int	env_size(t_env *env_lst)
{
	int	i;

	i = 0;
	while (env_lst)
	{
		i++;
		env_lst = env_lst->next;
	}
	return (i);
}

char	**create_mtr_env(t_env	*env_lst)
{
	char	**mtr;
	char	*tmp;
	int		size;
	int		i;

	i = 0;
	size = env_size(env_lst);
	mtr = malloc((size + 1) * sizeof(char*));
	while (i < size)
	{
		tmp = ft_strjoin("=", env_lst->value);
		mtr[i] = ft_strjoin(env_lst->key, tmp);
		free(tmp);
		// printf("%s\n", mtr[i]);
		i++;
		env_lst = env_lst->next;
	}
	mtr[i] = NULL;
	return (mtr);
}

int	run_execve(t_ast *node, t_shell *sh)
{
	char	**env;
	char	*path;

	env = create_mtr_env(sh->env_lst);
	if (!env)
		return (0);
	path = debug_okda(env, node->args[0]);
	if (!path)
	{
		perror("minishell");
		exit(127);
	}
	execve(path, node->args, env);
	exit(1);
}



int execute_command(t_ast *node, int infd, int outfd, int cs, t_shell *sh)
{
	int status;
	
	node->pid = fork();
	if (!node->pid)
	{
		if (node->args && is_builtin(node->args[0]))
		{
			execute_builtin(node, infd, outfd, sh);
			exit(0);
		}
		close(cs);
		handle_redirection(node, &infd, &outfd);
		if (infd)
		{
			dup2(infd, 0);
			close(infd);
		}
		if (outfd != 1)
		{
			dup2(outfd, 1);
			close(outfd);
		}
		run_execve(node, sh);
	}    
	return (0);
}

int abs_execute(t_ast *node, int infd, int outfd, int cs, t_shell *sh)
{
	if (!node->args)
	{
		handle_redirection(node, &infd, &outfd);
	}
	else if (node->is_pipe || (!is_builtin(node->args[0])))
	{
		execute_command(node, infd, outfd, cs, sh);
	}
	else
	{
		// printf("***************1337*********************\n");
		execute_builtin(node, infd, outfd, sh);
	}
}

int execute_tree(t_ast *node, int fd, int outfd, int cs, t_shell *sh)
{
	int status;
	
	status = 1;
	if (!node)
		return (1);
	if (node->e_token_type == TOKEN_PIPE)
	{
		flaging_pipe(node);
		node->right->ar_pipe = malloc(2 * sizeof(int));
		pipe(node->right->ar_pipe);
		execute_tree(node->left, fd, node->right->ar_pipe[1], node->right->ar_pipe[0], sh);
		status = execute_tree(node->right, node->right->ar_pipe[0], outfd, node->right->ar_pipe[1], sh);
		close(node->right->ar_pipe[0]);
		close(node->right->ar_pipe[1]);
	}
	else if (node->e_token_type == TOKEN_WORD)
	{
		// for (int i = 0; node->args[i]; i++)
		// 	printf("%s\n", node->args[i]);
		node->args = join_arg(node->args, node->arr_space, node->arg_count);
		status = abs_execute(node, fd, outfd, cs, sh);
		close_all_herdocs(node->redirs);
	}
	return (status);
}
