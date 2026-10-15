/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_and_store.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tongchernhaw <tongchernhaw@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/15 09:18:45 by tongchernha       #+#    #+#             */
/*   Updated: 2026/10/15 09:20:28 by tongchernha      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

static char	*join_free(char *storage, char *buffer)
{
	char	*tmp;

	if (!storage)
		return (ft_strdup(buffer));
	tmp = ft_strjoin(storage, buffer);
	free(storage);
	return (tmp);
}

static char	*finish(char *buffer, char *storage, ssize_t bytes_read)
{
	free(buffer);
	if (bytes_read < 0)
	{
		free(storage);
		return (NULL);
	}
	return (storage);
}

char	*read_and_store(int fd, char *storage)
{
	char	*buffer;
	ssize_t	bytes_read;

	buffer = malloc(BUFFER_SIZE + 1);
	if (!buffer)
	{
		free(storage);
		return (NULL);
	}
	bytes_read = 1;
	while (bytes_read > 0 && !ft_strchr(storage, '\n'))
	{
		bytes_read = read(fd, buffer, BUFFER_SIZE);
		if (bytes_read <= 0)
			break ;
		buffer[bytes_read] = '\0';
		storage = join_free(storage, buffer);
		if (!storage)
			bytes_read = -1;
	}
	return (finish(buffer, storage, bytes_read));
}
