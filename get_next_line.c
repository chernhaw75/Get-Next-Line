/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tongchernhaw <tongchernhaw@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/15 09:18:45 by tongchernha       #+#    #+#             */
/*   Updated: 2026/10/15 09:20:28 by tongchernha      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

char	*get_next_line(int fd)
{
	static char	*storage;
	char		*line;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	storage = read_and_store(fd, storage);
	if (!storage)
		return (NULL);
	line = extract_line(storage);
	storage = update_storage(storage);
	return (line);
}
