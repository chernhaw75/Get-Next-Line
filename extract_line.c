/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   extract_line.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tongchernhaw <tongchernhaw@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/15 09:18:45 by tongchernha       #+#    #+#             */
/*   Updated: 2026/10/15 09:20:28 by tongchernha      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

char	*extract_line(char *storage)
{
	size_t	i;

	if (!storage || !storage[0])
		return (NULL);
	i = 0;
	while (storage[i] && storage[i] != '\n')
		i++;
	if (storage[i] == '\n')
		i++;
	return (ft_substr(storage, 0, i));
}
