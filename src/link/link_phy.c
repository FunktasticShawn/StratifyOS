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
 * along with Stratify OS.  If not, see <http://www.gnu.org/licenses/>. */


#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <errno.h>


#include "link_local.h"

#define MAX_DEVICE_PATH 1024

static int check_name_length(const char * name){
    if( strnlen(name, MAX_DEVICE_PATH) >= MAX_DEVICE_PATH ){
        return -1;
    }
    return 0;
}

#if defined __win32 || defined __win64
#include <windows.h>

typedef struct {
	HANDLE handle;
    char name[MAX_DEVICE_PATH];
} link_phy_container_t;


#define COM_PORT_NAME "\\\\.\\COM"
#define COM_PORT_MAX 128


int link_phy_getname(char * dest, const char * last, int len){
	int com_port;
	char buffer[12];

	if( strlen(last) == 0 ){
		com_port = 0;
		sprintf(dest, "%s%d", COM_PORT_NAME, com_port);
		return 0;
	} else {
		com_port = 0;
		do {
			sprintf(buffer, "%s%d", COM_PORT_NAME, com_port++);
			if( strcmp(buffer, last) == 0 ){
				break;
			}
		} while(com_port < COM_PORT_MAX);
		if( com_port > (COM_PORT_MAX-1) ){
			return LINK_PHY_ERROR;
		}
	}

	sprintf(buffer, "%s%d", COM_PORT_NAME, com_port);
	strcpy(dest, buffer);

	return 0;
}

int link_phy_status(link_transport_phy_t handle){
    link_phy_container_t * phy = handle;
    HANDLE test_handle;
    DWORD err;

    test_handle = CreateFile(phy->name, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if( test_handle == INVALID_HANDLE_VALUE ){
        err = GetLastError();
        if( err == 5 ){
            //all is well
            return 0;
        }
    } else {
        CloseHandle(test_handle);
        err = 100;
    }

    printf("LAST ERROR: %d\n", err);
    fflush(stdout);

    return -1;
}


link_transport_phy_t link_phy_open(const char * name, int baudrate){

	link_phy_container_t * handle;
	DCB params;

    if( check_name_length(name) < 0 ){
        return LINK_PHY_OPEN_ERROR;
    }

	handle = malloc(sizeof(link_phy_container_t));
	if( handle == 0 ){
		return LINK_PHY_OPEN_ERROR;
	}

    memset(handle->name, 0, MAX_DEVICE_PATH);
    strncpy(handle->name, name, MAX_DEVICE_PATH-1);

	handle->handle = CreateFile(name, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if( handle->handle == INVALID_HANDLE_VALUE ){
		free(handle);
		return LINK_PHY_OPEN_ERROR;
	}

	if( !GetCommState(handle->handle, &params) ){
		CloseHandle(handle->handle);
		free(handle);
		return LINK_PHY_OPEN_ERROR;
	}


	params.BaudRate = 460800;
	params.ByteSize = 8;
	params.StopBits = TWOSTOPBITS;
	params.Parity = EVENPARITY;
	params.fBinary=1;


	if ( !SetCommState(handle->handle, &params)){
		CloseHandle(handle->handle);
		free(handle);
		link_error("Failed set COMM state\n");
		return LINK_PHY_OPEN_ERROR;
	}



	COMMTIMEOUTS timeouts={0};
	timeouts.ReadIntervalTimeout=MAXDWORD;
	timeouts.ReadTotalTimeoutConstant=1;
	timeouts.ReadTotalTimeoutMultiplier=MAXDWORD;
	timeouts.WriteTotalTimeoutConstant=0;
	timeouts.WriteTotalTimeoutMultiplier=0;
	if(!SetCommTimeouts(handle->handle, &timeouts)){
		//error occurred. Inform user
		link_error("Failed to set Timeouts\n");
		CloseHandle(handle->handle);
		free(handle);
		return LINK_PHY_OPEN_ERROR;
	}


	return handle;
}

int link_phy_write(link_transport_phy_t handle, const void * buf, int nbyte){
	DWORD bytes_written;
    link_phy_container_t * phy = handle;

	if( handle == LINK_PHY_OPEN_ERROR){
		return LINK_PHY_ERROR;
	}

    if( link_phy_status(handle) < 0 ){
        return LINK_PHY_ERROR;
    }

    if( !WriteFile(phy->handle, buf, nbyte, &bytes_written, NULL) ){
		link_error("Failed to write %d bytes from handle:%d\n", nbyte, (int)handle);
		return LINK_PHY_ERROR;
	}
	return bytes_written;
}

int link_phy_read(link_transport_phy_t handle, void * buf, int nbyte){
	DWORD bytes_read;
    link_phy_container_t * phy = handle;

	if( handle == LINK_PHY_OPEN_ERROR){
		return LINK_PHY_ERROR;
	}

    if( link_phy_status(handle) < 0 ){
        return LINK_PHY_ERROR;
    }

    if( !ReadFile(phy->handle, buf, nbyte, &bytes_read, NULL) ){
		link_error("Failed to read %d bytes from handle:%d\n", nbyte, (int)handle);
		return LINK_PHY_ERROR;
	}
	return bytes_read;
}

int link_phy_close(link_transport_phy_t * handle){
	link_phy_container_t * phy = (link_phy_container_t *)*handle;

	if( phy == LINK_PHY_OPEN_ERROR ){
		return 0;
	}

	*handle = LINK_PHY_OPEN_ERROR;
	if( CloseHandle(phy->handle) == 0 ){
		link_error("Failed to close handle\n");
	}
	free(phy);
	return 0;
}

void link_phy_wait(int msec){
	Sleep(msec);
}

void link_phy_flush(link_transport_phy_t handle){
	char c;
	while( link_phy_read(handle, &c, 1) == 1 ){
		;
	}
}
#endif

#if defined __macosx || defined __linux
#include <string.h>
#include <dirent.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#define BAUDRATE 460800

typedef struct {
	int fd;
    char device_path[MAX_DEVICE_PATH];
} link_phy_container_t;

//This is the mac osx prefix -- this needs to be in a list so it can also check bluetooth
#ifdef __macosx
#define TTY_DEV_PREFIX "tty.usbmodem"
#endif

#ifdef __linux
#define TTY_DEV_PREFIX "ttyACM"
#endif

int link_phy_getname(char * dest, const char * last, int len){
	//lookup the next eligible device
	struct dirent entry;
	struct dirent * result;
	DIR * dirp;
	int pre_len;
	int past_last;


	dirp = opendir("/dev");
	if( dirp == NULL ){
		link_error("/dev directory not found");
		return LINK_PHY_ERROR;
	}

	pre_len = strlen(TTY_DEV_PREFIX);
	past_last = false;
	if( strlen(last) == 0 ){
		past_last = true;
	}

	while( (readdir_r(dirp, &entry, &result) == 0) && (result != NULL) ){
		if( strncmp(TTY_DEV_PREFIX, entry.d_name, pre_len) == 0 ){
			//the entry matches the prefix

			if( past_last == true ){
				if( strlen(entry.d_name) > len ){
					//name won't fit in destination
					closedir(dirp);
					return LINK_PHY_ERROR;
				}

				strcpy(dest, entry.d_name);
				closedir(dirp);
				return 0;
			} else if( strcmp(last, entry.d_name) == 0 ){
				past_last = true;
			}
		}
	}

	//no more entries to be found
	closedir(dirp);
	return LINK_PHY_ERROR;
}


link_transport_phy_t link_phy_open(const char * name, int baudrate){
    link_transport_phy_t phy;
	int fd;
	struct termios options;
    link_phy_container_t * container;

    if( strnlen(name, MAX_DEVICE_PATH) >= MAX_DEVICE_PATH ){
        return LINK_PHY_OPEN_ERROR;
    }


	//open serial port
	fd = open(name, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if( fd < 0 ){
		link_error("Failed to open %s %d", name, errno);
		perror("Failed to open");
		fflush(stderr);
		return LINK_PHY_OPEN_ERROR;
	}

	if( ioctl(fd, TIOCEXCL) == -1 ){
		link_error("failed to make device exclusive");
		return LINK_PHY_OPEN_ERROR;
	}

	memset(&options, 0, sizeof(options));


	//make the buffer raw
	cfmakeraw(&options); //raw with no buffering

	cfsetspeed(&options, BAUDRATE);
	//even parity
	//8 bit data
	//one stop bit
	options.c_cflag |= PARENB; //parity on
	options.c_cflag &= ~PARODD; //parity is not odd (use even parity)
	options.c_cflag |= CSTOPB; //two stop bits
	options.c_cflag |= CREAD; //enable receiver
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CREAD | CLOCAL | CS8;  //8 bit data
	options.c_cc[VMIN]=0;
	options.c_cc[VTIME]=0;

	//set the attributes
	if( tcflush(fd, TCIFLUSH) < 0 ){
		return LINK_PHY_OPEN_ERROR;
	}

	if( tcsetattr(fd, TCSANOW, &options) < 0 ){
		return LINK_PHY_OPEN_ERROR;
	}


	phy = malloc(sizeof(link_phy_container_t));
    container = phy;
    if( phy == 0 ){
        close(fd);
        return LINK_PHY_OPEN_ERROR;
    }

    container->fd = fd;
    strncpy(container->device_path, name, MAX_DEVICE_PATH);

	link_phy_flush(phy);
	return phy;
}

int link_phy_status(link_transport_phy_t handle){
    link_phy_container_t * phy = handle;

    if( access(phy->device_path, F_OK) < 0 ){
        //file does not exist

        printf("NO Access to %s\n", phy->device_path);
        fflush(stdout);

        return LINK_PHY_ERROR;
    }
	return 0;
}

int link_phy_write(link_transport_phy_t handle, const void * buf, int nbyte){
	struct termios options;
	link_phy_container_t * phy = handle;
    int tmp;
    int ret;

	if( handle == LINK_PHY_OPEN_ERROR ){
		return LINK_PHY_ERROR;
	}

    if( link_phy_status(handle) < 0 ){
		return LINK_PHY_ERROR;
	}

    tmp = errno;
    ret = write(phy->fd, buf, nbyte);
    if( ret < 0 ){
        if ( errno == EAGAIN ){
            errno = tmp;
            return 0;
        }
        return LINK_PHY_ERROR;
    }

	return nbyte;
}

int link_phy_read(link_transport_phy_t handle, void * buf, int nbyte){
	int ret;
	int tmp;
	link_phy_container_t * phy = handle;

	if( handle == LINK_PHY_OPEN_ERROR ){
		return LINK_PHY_ERROR;
	}

	tmp = errno;

    if( link_phy_status(handle) < 0 ){
        return LINK_PHY_ERROR;
    }

	ret = read(phy->fd, buf, nbyte);
	if( ret < 0 ){
		if ( errno == EAGAIN ){
			errno = tmp;
			return 0;
		}
		return LINK_PHY_ERROR;
	}

	return ret;
}

int link_phy_close(link_transport_phy_t * handle){
	link_phy_container_t * phy = (link_phy_container_t *)*handle;
	if( *handle == LINK_PHY_OPEN_ERROR ){
		return LINK_PHY_ERROR;
	}
	*handle = LINK_PHY_OPEN_ERROR;
	int fd = phy->fd;
	free(phy);

    if( close(fd) < 0 ){
		return LINK_PHY_ERROR;
	}

    return 0;
}

void link_phy_wait(int msec){
	usleep(msec*1000);
}

void link_phy_flush(link_transport_phy_t handle){
	unsigned char c;
	while( link_phy_read(handle, &c, 1) == 1 ){
		;
	}
}

#endif

int link_phy_lock(link_transport_phy_t phy){
	return 0;
}

int link_phy_unlock(link_transport_phy_t phy){
    return 0;
}
