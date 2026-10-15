/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_and_store.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tongchernhaw <tongchernhaw@student.42.fr      +#+  +:+       +#+      */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 15:24:29 by tongchernhaw     #+#    #+#             */
/*   Updated: 2026/10/14 15:24:29 by tongchernhaw    ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "get_next_line.h"

char*   read_and_store(int fd, char *storage)
{
    char    *buffer;
    int bytes_read;

    buffer  = malloc (sizeof(char) * (BUFFER_SIZE+1));
    if(!buffer)
        return (NULL);
    bytes_read = 1;
    while ((!storage||!ft_strchr(storage, '\n')) && bytes_read != 0)
    {
        bytes_read = read(fd, buffer, BUFFER_SIZE);
        if (bytes_read == -1)
        {
            free(buffer);
            return (NULL);
        }
        if (bytes_read == 0)
        {
            free(buffer);
            return (storage);
        }   
        if (bytes_read > 0)
        {
            buffer[bytes_read] = '\0';
            if (!storage)
                storage = ft_strdup(buffer);
            else
            storage = ft_strjoin(storage, buffer);
        }
        else
        {
            free(buffer);
            return (storage);
        }
          
    }

    free(buffer);
    return (storage);
}