/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   extract_line.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tongchernhaw <tongchernhaw@student.42.fr      +#+  +:+       +#+      */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 15:42:27 by tongchernhaw     #+#    #+#             */
/*   Updated: 2026/10/14 15:42:27 by tongchernhaw    ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "get_next_line.h"

char    *extract_line(char *storage)
{
    int    i;
    char    *line;

    i = 0;
    if (!storage[i])
        return (NULL);
    while(storage[i] && storage[i] !='\n')
        i++;
    if (storage)
    {
        line = malloc(sizeof(char) * (i + 2));
        if (!line)
            return (NULL);
        i = 0;
        while (storage[i] && storage[i] != '\n')
        {
            line[i] = storage[i];
            i++;
        }
        if (storage[i] == '\n')
            line[i] = ft_substr(storage, 0, i + 1);
        else
            line[i] = ft_substr(storage, 0, i);
       return (line);
    }   
    
}