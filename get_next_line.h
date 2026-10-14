/*******************************************************************************/
/*                                                                              */
/*                                                         :::      ::::::::    */
/*    get_next_line.h                                         :+:      :+:    :+:    */
/*                                                     +:+ +:+         +:+      */
/*    By: tongchernhaw <tongchernhaw@student.42.f    +#+  +:+       +#+         */
/*                                                 +#+#+#+#+#+   +#+            */
/*    Created: 2026/10/14 14:54:00 by tongchernha       #+#    #+#              */
/*    Updated: 2026/10/14 14:54:00 by tongchernha      ###   ########.fr        */
/*                                                                              */                                                                              #
/***************************************************************************** */


#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

// 1. Include necessary headers
# include <unistd.h>
# include <stdlib.h>
# include <stddef.h>

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 42
# endif

char    *get_next_line(int fd);