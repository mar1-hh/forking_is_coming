NAME = minishell

GREEN = \033[0;32m

SOURCES = \
	parsing/tokenize/lexer.c parsing/error/error.c parsing/tree/pipes_handling.c\
	parsing/tree/tree_wdakxi.c parsing/tokenize/main.c parsing/tree/prepare_env.c\
	parsing/tree/export.c parsing/tree/cd.c parsing/tree/unset.c parsing/tree/pwd.c\
	parsing/tree/echo.c parsing/tree/exit.c parsing/tree/prepare_heredoc.c parsing/tokenize/expand.c\
	parsing/tree/joining_args.c parsing/tree/free_all.c parsing/tokenize/syntax_error.c\

OBJECTS = $(SOURCES:.c=.o)

CC = cc -fsanitize=address

LIBFT_DIR = ./libft
LIBFT = $(LIBFT_DIR)/libft.a

READLINE_PATH = /Users/achat/homebrew/opt/readline

READLINE_INCLUDE = -I$(READLINE_PATH)/include
READLINE_LIB = -L$(READLINE_PATH)/lib -lreadline -lncurses


all: $(NAME)

$(LIBFT):
	@echo "$(BLUE)Building libft library...$(RESET)"
	@$(MAKE) -C $(LIBFT_DIR)

$(NAME): $(LIBFT) $(OBJECTS)
	@echo "$(GREEN)Linking executable...$(RESET)"
	@$(CC) -o $(NAME) $(OBJECTS) -L$(LIBFT_DIR) -lft $(READLINE_LIB)
	@echo "$(GREEN)✓ $(NAME) successfully built!$(RESET)"

%.o: %.c minishell.h
	@printf "$(YELLOW)▶ Compiling... [%s]\r$(RESET)" $
	@$(CC) -I$(LIBFT_DIR) $(READLINE_INCLUDE) -c $< -o $@
	@printf "$(GREEN)✓ Compiled   $(RESET)\n"

clean:
	@echo "$(BLUE)Cleaning project files...$(RESET)"
	@rm -f $(OBJECTS)
	@$(MAKE) -C $(LIBFT_DIR) clean

fclean: clean
	@echo "$(BLUE)Removing executable...$(RESET)"
	@rm -f $(NAME)
	@$(MAKE) -C $(LIBFT_DIR) fclean

re: fclean all

.PHONY: all clean fclean re