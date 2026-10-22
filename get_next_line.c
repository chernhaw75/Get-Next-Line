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

static char	*join_free(char *storage, char *buffer)
{
	char	*tmp;

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

	buffer = malloc((size_t)BUFFER_SIZE + 1);
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

char	*update_storage(char *storage)
{
	char	*rest;
	char	*nl;

	nl = ft_strchr(storage, '\n');
	if (!nl || !*(nl + 1))
	{
		free(storage);
		return (NULL);
	}
	rest = ft_substr(storage, (unsigned int)(nl - storage + 1),
			ft_strlen(nl + 1));
	free(storage);
	return (rest);
}

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
	if (!line)
	{
		free(storage);
		storage = NULL;
		return (NULL);
	}
	storage = update_storage(storage);
	return (line);
}
