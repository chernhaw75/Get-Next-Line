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

static int	fill(int fd, t_str *s, char *buffer)
{
	ssize_t	r;
	int		nl;

	nl = (ft_strchr(s->data, '\n') != NULL);
	r = 1;
	while (!nl && r > 0)
	{
		r = read(fd, buffer, BUFFER_SIZE);
		if (r <= 0)
			break ;
		buffer[r] = '\0';
		if (!str_append(s, buffer, (size_t)r))
			return (-1);
		nl = (ft_strchr(buffer, '\n') != NULL);
	}
	return ((int)r);
}

char	*read_and_store(int fd, char *storage)
{
	char	*buffer;
	t_str	s;
	int		r;

	buffer = malloc((size_t)BUFFER_SIZE + 1);
	if (!buffer)
	{
		free(storage);
		return (NULL);
	}
	s.data = storage;
	s.len = ft_strlen(storage);
	s.cap = 0;
	if (storage)
		s.cap = s.len + 1;
	r = fill(fd, &s, buffer);
	free(buffer);
	if (r < 0)
	{
		free(s.data);
		return (NULL);
	}
	return (s.data);
}

char	*extract_line(char *storage)
{
	size_t	len;

	len = 0;
	while (storage[len] && storage[len] != '\n')
		len++;
	if (storage[len] == '\n')
		len++;
	return (ft_substr(storage, 0, len));
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
