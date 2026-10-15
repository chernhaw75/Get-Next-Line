/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tongchernhaw <tongchernhaw@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/23 08:17:17 by tchern-h          #+#    #+#             */
/*   Updated: 2026/10/14 15:48:43 by tongchernha      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

static size_t	get_sublen(size_t str_len, unsigned int start, size_t len)
{
	size_t	available;

	if (start >= str_len)
		return (0);
	available = str_len - start;
	if (available < len)
		return (available);
	return (len);
}

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char	*sub;
	size_t	sublen;

	if (!s)
		return (NULL);
	sublen = get_sublen(ft_strlen(s), start, len);
	sub = malloc(sublen + 1);
	if (!sub)
		return (NULL);
	ft_memcpy(sub, s + start, sublen);
	sub[sublen] = '\0';
	return (sub);
}
