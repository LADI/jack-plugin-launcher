/* -*- Mode: C ; c-basic-offset: 2 -*- */
/* SPDX-FileCopyrightText: Copyright © 2010-2023 Nedko Arnaudov */
/* SPDX-FileCopyrightText: Copyright © 2010 Nikita Zlobin <nick87720z@gmail.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */
/**
 * @file   file.h
 * @brief  File helpers
 */

#ifndef FILE_H__A3F70F24_F665_462E_87D0_608A2A392A5A__INCLUDED
#define FILE_H__A3F70F24_F665_462E_87D0_608A2A392A5A__INCLUDED

/**
 * @brief Read (small) file contents into newly allocated buffer
 * Use this function with caution as big enough files may not fit in RAM.
 *
 * @param filename name of file to read
 *
 * @return Newly allocated buffer where the contents if specified file are read into.
 */
char * read_file_contents(const char * filename);

#endif /* #ifndef FILE_H__A3F70F24_F665_462E_87D0_608A2A392A5A__INCLUDED */
