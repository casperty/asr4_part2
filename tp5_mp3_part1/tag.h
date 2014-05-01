/* This file is part of asr4tag.                        
 *									 
 * asr4tag is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by	 
 * the Free Software Foundation, either version 3 of the License, or	 
 * (at your option) any later version.					 
 * 									 
 * asr4tag is distributed in the hope that it will be useful,		 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of	 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the	 
 * GNU General Public License for more details.				 
 * 									 
 * You should have received a copy of the GNU General Public License	 
 * along with asr4tag.  If not, see <http://www.gnu.org/licenses/>.	 
 */

/*
  
    Author: Michael Hauspie <michael.hauspie@univ-lille1.fr>
    Created: 2011-08-17
    Time-stamp: <2011-08-24 15:25:43 (hauspie)>
*/

#ifndef __TAG_H__
#define __TAG_H__


#define U8_SIZE 1
#define U16_SIZE 2
#define U32_SIZE 4

#define ENC_ASCII 0
#define ENC_UNICODE 1

#define ENC_UNICODE_FFFE 0
#define ENC_UNICODE_FEFF 1

#define ID3V2_FRAME_ID_LENGTH 4

/* Type definition */
typedef unsigned       char u8;
typedef unsigned short int  u16;
typedef unsigned       int  u32;

typedef struct
{
    char ID3[4]; /* "ID3" */
    u16  version;
    u8   flags;
    u32  size;
} id3v2_header_t;


typedef struct
{
    char id[ID3V2_FRAME_ID_LENGTH + 1]; /* +1 for '\0' */
    u32  size;
    u16  flags;
    char *text; /* value of the frame if it is a text frame */
} id3v2_frame_t;


/**
 * Reads an u8 from a file descriptor
 *
 * @param fd file descriptor
 * @param val where the value should be stored
 *
 * @return 0 on error, 1 otherwise
 */
int read_u8(int fd, u8 *val);


/**
 * Reads an u16 from a file descriptor
 *
 * @param fd file descriptor
 * @param val where the value should be stored
 *
 * @return 0 on error, 1 otherwise
 */
int read_u16(int fd, u16 *val);


/**
 * Reads an u32 from a file descriptor
 *
 * @param fd file descriptor
 * @param val where the value should be stored
 *
 * @return 0 on error, 1 otherwise
 */
int read_u32(int fd, u32 *val);


/**
 * Converts ID3 size
 *
 * @param size initial size
 *
 * @return the converted size (every 7th bit of each byte has been removed)
 */
u32 convert_size(u32 size);


/**
 * Reads a string from a file descriptor
 *
 * @param fd the file descriptor
 * @param to where to store the string
 * @param size how many bytes
 * @param encoding how to read chars
 *
 * if "to" is NULL, malloc memory space
 * else use "to" pointer to store the string
 *
 * @return the readed string
 */
char * read_string(int fd, char *to, int size, int encoding);

/** 
 * Reads the ID3v2 header. After reading the header, position the file at the
 * start of the frames
 * 
 * @param fd file descriptor
 * @param header pointer to a valid header structure
 * 
 * @return -1 on error, 0 otherwise
 */

int tag_read_id3_header(int fd, id3v2_header_t *header);


/**
 * Reads a frame, stores data into the structure
 *
 * @param fd file descriptor
 * @param frame structure pointer
 *
 * @return -1 on error, 0 otherwise
 */
int tag_read_one_frame(int fd, id3v2_frame_t *frame);

/** 
 * @brief Returns the list of frames of the file
 * 
 * @param file the path to the file to parse
 * @param frame_array_size pointer to a valid integer where the function will
 * store the number of read frames
 * 
 * @return NULL if error, an array of frames if successful
 */
id3v2_frame_t *tag_get_frames(const char *file, int *frame_array_size);

/** 
 * @brief frees a frames buffer and all that has been allocated to hold frame values
 * 
 * @param frames 
 * @param frame_count 
 */
void tag_free(id3v2_frame_t *frames, int frame_count);

#endif
