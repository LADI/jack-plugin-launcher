/* -*- Mode: C ; c-basic-offset: 4 -*- */
/* SPDX-FileCopyrightText: Copyright © 2009-2023 Nedko Arnaudov */
/* SPDX-License-Identifier: ( GPL-2.0-or-later OR LGPL-2.1-or-later OR MIT OR AFL-2.1 ) */
/**
 * @file   catdup.h
 * @brief  String concatenation into newly allocated buffer
 */
/**************************************************************************
 * This file contains implementation of double linked list (kernel style)
 **************************************************************************/

#ifndef CATDUP_H__D42302F1_4D96_4EE4_AC09_E97ED5748277__INCLUDED
#define CATDUP_H__D42302F1_4D96_4EE4_AC09_E97ED5748277__INCLUDED

/**
 * @brief Concatenate two strings into a newly allocated buffer
 *
 * @param s1 First string to concatenate
 * @param s2 Second string to concatenate
 *
 * @return Newly allocated buffer where the input strings are concatenated into.
 */
char * catdup(const char * s1, const char * s2);

/**
 * @brief Concatenate three strings into a newly allocated buffer
 *
 * @param s1 First string to concatenate
 * @param s2 Second string to concatenate
 * @param s3 Third string to concatenate
 *
 * @return Newly allocated buffer where the input strings are concatenated into.
 */
char * catdup3(const char * s1, const char * s2, const char * s3);

/**
 * @brief Concatenate four strings into a newly allocated buffer
 *
 * @param s1 First string to concatenate
 * @param s2 Second string to concatenate
 * @param s3 Third string to concatenate
 * @param s4 Fourth string to concatenate
 *
 * @return Newly allocated buffer where the input strings are concatenated into.
 */
char * catdup4(const char * s1, const char * s2, const char * s3, const char * s4);

/**
 * @brief Concatenate any number of strings into a newly allocated buffer
 *
 * @param s1 First string to concatenate
 * @param s2 Second string to concatenate
 * @param ... Other strings to concatenate, NULL delimits end of list.
 *
 * @return Newly allocated buffer where the input strings are concatenated into.
 */
char * catdupv(const char * s1, const char * s2, ...);

/**
 * @brief Concatenate strings specified as array into a newly allocated buffer,
 * with optional delimiter, 
 *
 * @param array NULL terminated array of input strings to concatenate
 * @param delimiter Optional (can be NULL) string to insert in between input strings. 
 *
 * @return Newly allocated buffer where the input strings are concatenated into.
 */
char * catdup_array(const char * const * array, const char * delimiter);

#endif /* #ifndef CATDUP_H__D42302F1_4D96_4EE4_AC09_E97ED5748277__INCLUDED */
