/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achat <achat@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 16:26:06 by achat             #+#    #+#             */
/*   Updated: 2025/04/14 08:29:41 by achat            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBFT_H
# define LIBFT_H

# include <stdlib.h>
# include <unistd.h>

void				ft_bzero(void *s, size_t n);
char				*ft_strcat(char *dest, char *src);
int					ft_isalnum(int c);
int					ft_isalpha(int c);
int					ft_isascii(int c);
int					ft_isdigit(int c);
char				*ft_strcpy(char *s1, char *s2);
int					ft_isprint(int c);
size_t				ft_strlen(const char *s);
int					ft_tolower(int c);
int					ft_toupper(int c);
char				*ft_strchr(const char *s, int c);
char				*ft_strrchr(const char *s, int c);
size_t				ft_strlcat(char *dst, const char *src, size_t dstsize);
size_t				ft_strlcpy(char *dst, const char *src, size_t dstsize);
int					ft_strncmp(const char *s1, const char *s2, size_t n);
char				*ft_strnstr(const char *haystack, const char *needle,
						size_t len);
int					ft_atoi(const char *str);
void				*ft_memset(void *b, int c, size_t len);
void				*ft_memchr(const void *s, int c, size_t n);
int					ft_memcmp(const void *s1, const void *s2, size_t n);
void				*ft_realloc(void *ptr, size_t newsize);
void				*ft_memcpy(void *dst, const void *src, size_t n);
void				*ft_memmove(void *dst, const void *src, size_t len);
void				ft_putchar_fd(char c, int fd);
void				ft_putstr_fd(char *s, int fd);
void				ft_putnbr_fd(int n, int fd);
void				ft_putendl_fd(char *s, int fd);
void				ft_striteri(char *s, void (*f)(unsigned int, char *));
char				*ft_substr(char const *s, unsigned int start, size_t len);
char				*ft_strtrim(char const *s1, char const *set);
char				*ft_strdup(const char *s1);
void				*ft_calloc(size_t count, size_t size);
char				*ft_strmapi(char const *s, char (*f)(unsigned int, char));
char				*ft_itoa(int n);
char				**ft_split(char const *s, char c);
char				*ft_strjoin(char const *s1, char const *s2);
int					ft_isspace(int c);
int					ft_strcmp(const char *str1, const char *str2);

#endif
