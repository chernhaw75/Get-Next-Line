/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tongchernhaw <tongchernhaw@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/15 09:18:45 by tongchernha       #+#    #+#             */
/*   Updated: 2026/10/15 09:20:28 by tongchernha      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "get_next_line.h"

size_t	ft_strlen(const char *str)
{
	size_t	i;

	i = 0;
	if (!str)
		return (0);
	while (str[i])
		i++;
	return (i);
}

char	*ft_strchr(const char *s, int c)
{
	if (!s)
		return (NULL);
	while (*s)
	{
		if (*s == (char)c)
			return ((char *)s);
		s++;
	}
	if ((char)c == '\0')
		return ((char *)s);
	return (NULL);
}

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char	*sub;
	size_t	s_len;
	size_t	i;

	if (!s)
		return (NULL);
	s_len = ft_strlen(s);
	if (start >= s_len)
		len = 0;
	else if (len > s_len - start)
		len = s_len - start;
	sub = malloc(len + 1);
	if (!sub)
		return (NULL);
	i = 0;
	while (i < len)
	{
		sub[i] = s[start + i];
		i++;
	}
	sub[i] = '\0';
	return (sub);
}

static int	str_grow(t_str *s, size_t need)
{
	char	*tmp;
	size_t	cap;
	size_t	i;

	if (need <= s->cap)
		return (1);
	cap = s->cap * 2;
	if (cap < need)
		cap = need;
	tmp = malloc(cap);
	if (!tmp)
		return (0);
	i = 0;
	while (i < s->len)
	{
		tmp[i] = s->data[i];
		i++;
	}
	free(s->data);
	s->data = tmp;
	s->cap = cap;
	return (1);
}

int	str_append(t_str *s, const char *buf, size_t n)
{
	size_t	i;

	if (!str_grow(s, s->len + n + 1))
		return (0);
	i = 0;
	while (i < n)
	{
		s->data[s->len + i] = buf[i];
		i++;
	}
	s->len += n;
	s->data[s->len] = '\0';
	return (1);
}
