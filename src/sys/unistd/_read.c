/* Copyright 2011-2016 Tyler Gilbert; 
 * This file is part of Stratify OS.
 *
 * Stratify OS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Stratify OS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Stratify OS.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * 
 */

/*! \addtogroup UNI_FILE_ACCESS
 * @{
 */

/*! \file */

#include  "unistd_fs.h"
#include  "unistd_flags.h"
#include "sos/stratify.h"

/*! \details This function reads \a nbyte bytes from \a fildes to the memory
 * location pointed to by \a buf.
 *
 * read() is always a synchronous call which is either blocking or non-blocking
 * depending on the value of O_NONBLOCK for \a fildes.
 *
 * \param fildes The file descriptor returned by \ref open()
 * \param buf A pointer to the destination memory (process must have write access)
 * \param nbyte The number of bytes to read
 *
 * \return The number of bytes actually read of -1 with errno (see \ref ERRNO) set to:
 * - EBADF:  \a fildes is bad
 * - EACCESS:  \a fildes is on in O_WRONLY mode
 * - EIO:  IO error
 * - EAGAIN:  O_NONBLOCK is set for \a fildes and no new data is available
 *
 *
 */
int read(int fildes, void *buf, size_t nbyte);

int _read(int fildes, void *buf, size_t nbyte){
	void * handle;
	int flags;
	char * bufp;
	int loc;
	int bytes_read;
	const sysfs_t * fs;

	fildes = u_fildes_is_bad(fildes);
	if ( fildes < 0 ){
		//check to see if fildes is a socket
		errno = EBADF;
		return -1;
	}

	//This code will ensure that the process has permissions to access the specified memory
	//because reading is done in ISR (privileged mode) it will override the MPU
	if ( nbyte > 0 ){
		bufp = (char*)buf;
		//write the memory location to see if a fault happens
		loc = bufp[0];
		bufp[0] = 0;
		bufp[0] = loc;
		loc = bufp[nbyte-1];
		bufp[nbyte-1] = 0;
		bufp[nbyte-1] = loc;
	}

	if( fildes & FILDES_SOCKET_FLAG ){
		if( stratify_board_config.socket_api != 0 ){
			return stratify_board_config.socket_api->read(fildes, buf, nbyte);
		}
		errno = EBADF;
		return -1;
	}


	if ( (get_flags(fildes) & O_ACCMODE) == O_WRONLY ){
		errno = EACCES;
		return -1;
	}

	fs = get_fs(fildes);
	if ( fs->read_async != NULL ){  //This means the handle is not a regular file -- must be a device
		return u_read(get_open_file(fildes), buf, nbyte);
	}


	//initialize the file offset location
	loc = get_loc(fildes);
	flags = get_flags(fildes);
	handle = get_handle(fildes);
	bytes_read = fs->read(fs->cfg, handle, flags, loc, buf, nbyte);

	if ( bytes_read > 0 ){
		set_loc(fildes, loc + bytes_read);
	}

	return bytes_read;
}

/*! @} */
