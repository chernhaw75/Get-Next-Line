/ *****************************************************************************/
/*                                                                              */
/*                                                         :::      ::::::::    */
/*    Get Next Line                                         :+:      :+:    :+:    */
/*                                                     +:+ +:+         +:+      */
/*    By: tongchernhaw <tongchernhaw@student.42.f    +#+  +:+       +#+         */
/*                                                 +#+#+#+#+#+   +#+            */
/*    Created: 2026/10/14 14:54:00 by tongchernha       #+#    #+#              */
/*    Updated: 2026/10/14 14:54:00 by tongchernha      ###   ########.fr        */
/*                                                                              */                                                                              #
/***************************************************************************** */

#ifndef BUFFER_SIZE
# define BUFFER_SIZE 42
#endif

char    *get_next_line(int fd)
{
	char static *storage;
    char        *line;

    if (fd < 0 || BUFFER_SIZE <= 0)
        return (NULL);
    
}