#include "../../minishell.h"

t_ast *create_ast_node(t_token_type type)
{
    t_ast *node = (t_ast *)malloc(sizeof(t_ast));
    if (!node) return NULL;
    
    node->e_token_type = type;
    node->cmd = NULL;
    node->args = NULL;
    node->arg_count = 0;
    node->ar_pipe = NULL;
    node->redirs = NULL;
    node->left = node->right = NULL;
    node->pid = 0;
	node->is_pipe = 0;
    node->e_precedence = (type == TOKEN_PIPE) ? 1 : 0;
    
    return node;
}

t_ast *build_command_node(t_token **tokens)
{
    t_ast *cmd_node = create_ast_node(TOKEN_WORD);
    t_token *current = *tokens;
    int arg_count = 0;

    while (current && current->type == TOKEN_WORD)
    {
        cmd_node->args = ft_realloc(cmd_node->args, sizeof(char *) * (arg_count + 2));
        cmd_node->args[arg_count++] = ft_strdup(current->value);
        current = current->next;
    }
    if (cmd_node->args) {
        cmd_node->args[arg_count] = NULL;
        cmd_node->arg_count = arg_count;
    }
    if (arg_count > 0)
        cmd_node->cmd = ft_strdup(cmd_node->args[0]);
    cmd_node->redirs = handle_redir(tokens);    
    *tokens = current;
    return cmd_node;
}

t_ast *connect_pipe_nodes(t_token **tokens)
{
    t_ast *left = build_command_node(tokens);
    if (!left) 
		return NULL;

    while (*tokens && (*tokens)->type == TOKEN_PIPE)
    {
        t_ast *pipe_node = create_ast_node(TOKEN_PIPE);
        if (!pipe_node) {
            free_ast(left);
            return NULL;
        }
        
        pipe_node->left = left;
        *tokens = (*tokens)->next;  // Skip PIPE token
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

// int	ft_strcmp(char *str1, char *str2)
// {
// 	int	i;

// 	i = 0;
// 	while (str1[i] || str2[i])
// 	{
// 		if (str1[i] > str2[i])
// 			return (1);
// 		else if (str1[i] < str2[i])
// 			return (-1);
// 		i++;
// 	}
// 	return (0);
// }

// void	open_herdoc(t_redirection *node)
// {
// 	int		new_in_fd;
// 	char	*line;

// 	new_in_fd = open("here_doc", O_CREAT | O_WRONLY | O_TRUNC, 0644);
// 	// if (new_in_fd == -1)
// 	// 	t_error("pipex: input", data, 1);
// 	line = get_next_line(0);
// 	line[ft_strlen(line) - 1] = 0;
// 	while (ft_strcmp(node->file, line))
// 	{
// 		line[ft_strlen(line)] = '\n';
// 		write(new_in_fd, line, ft_strlen(line));
// 		free(line);
// 		line = get_next_line(0);
// 		line[ft_strlen(line) - 1] = 0;
// 	}
// 	free(line);
// 	close(new_in_fd);
// }

void	handle_redirection(t_ast *node, int *infd, int *outfd)
{
	t_redir	*lst;

	lst = node->redirs;
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
		// else if (lst->type == TOKEN_HEREDOC)
		// {
		// 	open_herdoc(lst);
		// 	*infd = open("here_doc", O_RDONLY);
		// 	if (*outfd == -1)
		// 	{
		// 		perror("open");
		// 		return ;
		// 	}
		// }
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
		// mn ba3d nzide perror
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
		if (is_builtin(node->args[0]))
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
	if (node->is_pipe || !is_builtin(node->args[0]))
		execute_command(node, infd, outfd, cs, sh);
	else
	{
		// printf("***************1337*********************\n");
		execute_builtin(node, infd, outfd, sh);
	}
}

int execute_tree(t_ast *node, int fd, int outfd, int cs, t_shell *sh)
{
	int status = 1;

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
		return (abs_execute(node, fd, outfd, cs, sh));
	return (status);
}

// void init_data(t_ast *ast, int flag)
// {
//     if (!ast)
//         return ;
//     if (ast->type == AST_PIPE)
//         flag = 1;
//     if (flag && ast->type == AST_CMD)
//     {
//         ast->is_wait = 1;
//         flag = 0;
//     }
//     init_data(ast->left, flag);
//     init_data(ast->right, 0);
// }

// int waiting(t_ast *ast)
// {
// 	int	status;
	
// 	while (wait(NULL) > 0);
// 	// if (!ast->right)
// 	// {
// 	// 	// waitpid(ast->pid, &status, 0);
// 	// 	while (wait(NULL) > 0);
// 	// 	return (0);
// 	// 	// return (WEXITSTATUS(status));
// 	// }
// 	// waiting(ast->right);
// 	// return (0);
// 	// wait(NULL);
// 	// if (!ast)
// 	//     return ;
// 	// if(ast->next)
// 	// {    if (ast->type == AST_CMD && ast->next->token->type == TOKEN_PIPE)
// 	//     	wait(NULL);
// 	// }
// 	// // wait(NULL);
// 	// waiting(ast->left, counter);
// 	// waiting(ast->right, counter);
// }

// int main()
// {
//     extern char **environ;  // Use system environment
//     t_ast *ast;
//     t_token *tokens = tokenize_compat("ls -la | grep minishell.h");
// 	// printf("tokens %s: \n", tokens->value);
//     if (!tokens)
//         return 1;
// 	// printf("tokens %s: \n", tokens->value);
//     t_ast *head = function_lmli7a(tokens, NULL);
// 	// printf("------>>>>>>>> %s: \n", head->left->cmd); 
//     //   // Uncomment for debugging
//     execute_tree(head, 0, 1, -1, environ);
//     free_ast(head);
//     // Free tokens here
//     return 0;
// }
