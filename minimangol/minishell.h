#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <unistd.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <fcntl.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <sys/dir.h>
# include <stdbool.h>
# include <signal.h>
# include "libft/libft.h"

typedef enum e_token_type
{
	TOKEN_WORD,
	TOKEN_REDIR_IN,
	TOKEN_REDIR_OUT,
	TOKEN_APPEND,
	TOKEN_HEREDOC,
	TOKEN_PIPE,
} t_token_type;

typedef struct s_redir	t_redir;
typedef struct s_token	t_token;
typedef struct s_ast	t_ast;
typedef struct s_shell	t_shell;

struct s_global
{
	int				status;
	int				hd;
	int				interrupted;
};

extern struct s_global	g_data;

struct s_token
{
	char            *value;
	t_token_type    type;
	t_token         *prev;
	int				is_space;
	int				quote_type;
	struct s_token  *next;
};

struct s_redir
{
	t_token_type    type;
	char            *file;
	int				is_expand;
	struct s_redir  *next;
	int				fd[2];
};

struct s_ast
{
	t_token_type  e_token_type;
	int           e_precedence;
	char          *cmd;
	char          **args;
	int           arg_count;
	int           *ar_pipe;
	t_redir       *redirs;
	int           is_pipe;
	int				is_last;
	t_shell			*sh;
	pid_t         pid;
	struct s_ast  *left;
	struct s_ast  *right;
	struct s_ast  *node;
};

typedef struct	s_qu_sp
{
	int	is_space;
	int	quote_type;
}	t_qu_sp;

typedef struct	s_env
{
	char			*key;
	char			*value;
	struct s_env	*next;
}	t_env;


typedef struct s_shell
{
	int     stdinput_fl;
	int     stdout_fl;
	char	**env;
	t_env	*env_lst;
	int		exit_status;
}   t_shell;

typedef struct s_expand
{
	char	*ptr;
	char	*return_value;
	char	*tmp;
	int		offset;
	int		size;
	// int		i;
}   t_expand;

/* Tokenization functions */
t_token     *lexer(char *input);
t_token	*merge_consecutive_words(t_token *tokens, t_ast *cmd_node);
t_ast	*connect_pipe_nodes(t_token **tokens, t_shell *sh);

void        free_tokens(t_token *tokens);
int	check_pipe_position(t_token *tokens);
int	check_consecutive_pipes(t_token *tokens);
int check_empty_command(t_token *tokens);

/* AST functions */
t_ast *create_ast_node(t_token_type type, t_shell *sh);
t_ast	*build_ast(t_token *tokens, t_shell *sh);
bool is_redirection(t_token_type type);
void	print_error(char *s);
int	is_redir_token(t_token_type type);
int	is_operator_token(t_token_type type);
int check_trailing_redir(t_token *tokens);
int	check_consecutive_redirections(t_token *tokens);
int	check_unclosed_quotes(t_token *tokens);
/* Redirection functions */
/* Redirections */
t_redir     *handle_redir(t_token **tokens);
char	*get_token_string(t_token_type type);
int         check_syntax_errors(t_token *tokens);

/* Execution functions */
int        handle_redirection(t_ast *node, int *infd, int *outfd);
t_redir *handle_redir(t_token **tokens);
char        *debug_okda(char **env, char *cmd);
int         execute_tree(t_ast *node, int fd, int outfd, int cs);
int         execute_command(t_ast *node, int infd, int outfd, int cs);

/* Helper functions */
int	check_consecutive_redirections(t_token *tokens);
int	check_trailing_redir(t_token *tokens);
int         check_syntax_errors(t_token *tokens);
int	check_empty_command(t_token *tokens);
int check_pipe_after_redir(t_token *tokens);
void        error(char *str);
int	check_pipe_position(t_token *tokens);
void    flaging_pipe(t_ast *sequence);
int	check_conflicting_redirections(t_token *tokens);
int	check_consecutive_pipes(t_token *tokens);
int	check_redir_without_file(t_token *tokens);
int	check_pipe_after_redir(t_token *tokens);
void	get_env(t_env **lst, char **env);
int 	ft_env(t_env *lst);
char	**split_env(char *str);
int ft_export(t_env **lst, char **export_param);
void	add_back_env(t_env **lst, t_env *new);
t_env	*new_env_node(char **mtr);
int	ft_cd(t_env **head, char *path);
int	ft_unset(t_env **lst, char **argv);
int	ft_pwd();
int ft_echo(char **args);
void    ft_exit(char **args, t_shell *sh);
int	check_syntax_errors(t_token *tokens);
int prepare_all_herdocs(t_ast *head, t_shell *sh);
int close_all_herdocs(t_redir *redirs);
char	*expand_line(char **line, t_shell *sh, t_token *next);
char	**join_arg(char **args, int *arr, int size);
void add_tokens(t_token **head, char *value, int type, t_qu_sp *s_q);
void	expand_tokens(t_token **token, t_shell *sh);
t_token	*joining_tokens(t_token *old_lst);
int	is_builtin(char *cmd);
void	free_mtx(char **mtr);
int	check_pipe_after_redir(t_token *tokens);
void free_tokens(t_token *tokens);
void	free_node(t_token *token);
void	free_tree(t_ast *head);
void	free_env(t_env *env);
int	prepare_herdoc(t_ast *head, t_shell *sh);
char	**ft_split_exp(char const *s, char c);
char	**split_env(char *str);
int	size_of_var(char *str);
t_env	*new_env_node(char **mtr);
void	add_back_env(t_env **lst, t_env *new);
void	set_operator_type(char c, char next, int *type, int *len);
int	pid_fail(int pid);
void	is_dir(char **args);
int	run_execve(t_ast *node, t_shell *sh);
int	env_size(t_env *env_lst);
int	execute_builtin(t_ast *node, int infd, int outfd, t_shell *sh);

#endif