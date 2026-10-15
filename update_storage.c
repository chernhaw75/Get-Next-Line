/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   update_storage.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tongchernhaw <tongchernhaw@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/15 09:18:45 by tongchernha       #+#    #+#             */
/*   Updated: 2026/10/15 09:20:28 by tongchernha      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

char    *update_storage(char *storage)
{
    size_t  i;
    char    *remainder

    if (!storage)
        return (NULL);
    i = 0;
    while (storage[i] && storage[i] != '\n')
        i++;
    if (!storage[i] || !storage[i + 1])
    {
        free(storage);
        return (NULL);
    }
    remainder = ft_substr(storage, i + 1, ft_strlen(storage) - i - 1);
    free(storage);
    return (remainder);
}
