//-< UNIFILE.CXX >---------------------------------------------------*--------*
// SAL                       Version 1.0         (c) 1997  GARRET    *     ?  *
// (System Abstraction Layer)                                        *   /\|  *
//                                                                   *  /  \  *
//                          Created:      7-Jan-97    K.A. Knizhnik  * / [] \ *
//                          Last update: 21-Nov-98    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// File implementation for Unix
//-------------------------------------------------------------------*--------*

#include "osfile.h"
#include "mmapfile.h"
#include <malloc.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/uio.h>

local int unix_access_mode[] = { O_RDONLY, O_WRONLY, O_RDWR };

#ifndef O_SYNC
#define O_SYNC	O_FSYNC
#endif

#ifndef O_DSYNC
#define O_DSYNC O_SYNC
#endif

local int unix_open_flags[] = {
    0, O_TRUNC, O_CREAT, O_CREAT|O_TRUNC,
    O_DSYNC, O_DSYNC|O_TRUNC, O_DSYNC|O_CREAT, O_DSYNC|O_CREAT|O_TRUNC,
};

//
// Use pread/pwrite functions to perform atomic positioning and read/write
// operations with file
//
#if defined(__svr4__)
#define CONCURRENT_IO 1
#endif

file::iop_status os_file::set_position(fposi_t pos)
{
    if (opened) {
	off_t rc, off = off_t(pos);
	assert(fposi_t(off) == pos); // position was not truncated
	rc = lseek(fd, off, SEEK_SET);
	if (rc < 0) {
	    return iop_status(errno);
	} else if (rc != off) {
	    return end_of_file;
	}
	return ok;
    }
    return not_opened;
}

file::iop_status os_file::get_position(fposi_t& pos)
{
    if (opened) {
	off_t rc = lseek(fd, 0, SEEK_CUR);
	if (rc < 0) {
	    return iop_status(errno);
	} else {
	    pos = rc;
	    return ok;
	}
    }
    return not_opened;
}

file::iop_status os_file::read(void* buf, size_t size)
{
    if (opened) {
	ssize_t rc = ::read(fd, buf, size);
	if (rc < 0) {
	    return iop_status(errno);
	} else if (size_t(rc) != size) {
	    return end_of_file;
	} else {
	    return ok;
	}
    } else {
	return not_opened;
    }
}

file::iop_status os_file::write(void const* buf, size_t size)
{
    if (opened) {
	ssize_t rc = ::write(fd, buf, size);
	if (rc < 0) {
	    return iop_status(errno);
	} else if (size_t(rc) != size) {
	    return end_of_file;
	} else {
	    return ok;
	}
    } else {
	return not_opened;
    }
}

file::iop_status os_file::read(fposi_t pos, void* buf, size_t size)
{
    if (opened) {
	off_t off = off_t(pos);
	assert(fposi_t(off) == pos); // position was not truncated
#ifdef CONCURRENT_IO
	ssize_t rc = pread(fd, buf, size, off);
	if (rc < 0) {
	    return iop_status(errno);
	} else if (size_t(rc) != size) {
	    return end_of_file;
	} else {
	    return ok;
	}
#else
	critical_section cs(guard);
	off_t rc = lseek(fd, off, SEEK_SET);
	if (rc < 0) {
	    return iop_status(errno);
	} else if (rc != off) {
	    return end_of_file;
	}
	rc = ::read(fd, buf, size);
	if (rc < 0) {
	    return iop_status(errno);
	} else if (size_t(rc) != size) {
	    return end_of_file;
	} else {
	    return ok;
	}
#endif
    } else {
	return not_opened;
    }
}

file::iop_status os_file::write(fposi_t pos, void const* buf, size_t size)
{
    if (opened) {
	off_t off = off_t(pos);
	assert(fposi_t(off) == pos); // position was not truncated
#ifdef CONCURRENT_IO
	ssize_t rc = pwrite(fd, buf, size, off);
	if (rc < 0) {
	    return iop_status(errno);
	} else if (size_t(rc) != size) {
	    return end_of_file;
	} else {
	    return ok;
	}
#else
	critical_section cs(guard);
	off_t rc = lseek(fd, off, SEEK_SET);
	if (rc < 0) {
	    return iop_status(errno);
	} else if (rc != off) {
	    return end_of_file;
	}
	rc = ::write(fd, buf, size);
	if (rc < 0) {
	    return iop_status(errno);
	} else if (size_t(rc) != size) {
	    return end_of_file;
	} else {
	    return ok;
	}
#endif
    } else {
	return not_opened;
    }
}

file::iop_status os_file::close()
{
    if (opened) {
	opened = False;
	if (::close(fd) != 0) {
	    return iop_status(errno);
	}
    }
    return ok;
}



file::iop_status os_file::open(access_mode mode, int flags)
{
    close();
    assert(name != NULL);
    fd = ::open(name, unix_access_mode[mode] |
		unix_open_flags[flags & (fo_truncate|fo_sync|fo_create)],
		0666);
    opened = False;
    this->mode = mode;
    this->flags = flags;
    if (fd < 0) {
	return iop_status(errno);
    } else {
	opened = True;
	//
	// Prevent concurrent access to file
	//
	if (flags & (fo_exclusive|fo_shared)) {
	    struct flock fl;
	    fl.l_type = (flags & fo_exclusive) ? F_WRLCK : F_RDLCK;
	    fl.l_whence = SEEK_SET;
	    fl.l_start = 0;
	    fl.l_len = 0;
	    if (fcntl(fd, F_SETLK, &fl) < 0
		&& (errno == EACCES || errno == EAGAIN))
	    {
		return lock_error;
	    }
	}
	return ok;
    }
}

file::iop_status os_file::remove()
{
    close();
    if (unlink(name) < 0) {
	return iop_status(errno);
    } else {
	return ok;
    }
}

char const* os_file::get_name() const
{
    return name;
}

file::iop_status os_file::set_name(char const* new_name)
{
    iop_status status = close();
    if (status == ok) {
	if (new_name == NULL) {
	    delete[] name;
	    name = NULL;
	} else {
	    if (name != NULL) {
		if (strcmp(name, new_name) != 0) {
		    if (rename(name, new_name) == 0) {
			delete[] name;
			name = strdup(new_name);
		    } else {
			return iop_status(errno);
		    }
		}
	    } else {
		name = strdup(new_name);
	    }
	}
    }
    return status;
}

file::iop_status os_file::get_size(fsize_t& size) const
{
    if (opened) {
	struct stat fs;
	if (fstat(fd, &fs) == 0) {
	    size = fs.st_size;
	    return ok;
	} else {
	    return iop_status(errno);
	}
    } else {
	return not_opened;
    }
}

file::iop_status os_file::set_size(fsize_t size)
{
    if (opened) {
	size_t new_size = size_t(size);
	assert(new_size == size);
	if (ftruncate(fd, new_size) == 0) {
	    return ok;
	} else {
	    return iop_status(errno);
	}
    } else {
	return not_opened;
    }
}

file::iop_status os_file::flush()
{
    if (opened) {
	if (fsync(fd) != 0) {
	    return iop_status(errno);
	}
	return ok;
    } else {
	return not_opened;
    }
}

void os_file::get_error_text(iop_status code, char* buf, size_t buf_size) const
{
    const char* msg;
    switch (code) {
      case ok:
	msg = "Ok";
	break;
      case not_opened:
	msg = "file not opened";
	break;
      case end_of_file:
        msg = "operation not completly finished";
	break;
      case lock_error:
        msg = "file is used by another program";
	break;
      default:
	msg = strerror(code);
    }
    strncpy(buf, msg, buf_size);
}


size_t os_file::get_disk_block_size()
{
    return getpagesize();
}

void* os_file::allocate_disk_buffer(size_t size)
{
    return valloc(size);
}

void  os_file::free_disk_buffer(void* buf)
{
    free(buf);
}

os_file::os_file(const char* name)
{
    opened = False;
    this->name = name ? strdup(name) : (char*)0;
}


os_file::~os_file()
{
    close();
    delete[] name;
}

//
// Mapped on memory file
//

#ifndef MAP_FAILED
#define MAP_FAILED (-1)
#endif

#ifndef MAP_FILE
#define MAP_FILE (0)
#endif


file::iop_status mmap_file::open(access_mode mode, int flags)
{
    iop_status status = os_file::open(mode, flags);
    if (status == ok) {
	size_t size = lseek(fd, 0, SEEK_END);
	if (size < init_size) {
	    size = init_size;
	    if (ftruncate(fd, init_size) != ok) {
		status = iop_status(errno);
		os_file::close();
		return status;
	    }
	}
	if (lseek(fd, 0, SEEK_SET) != 0) {
	    status = iop_status(errno);
	    os_file::close();
	    return status;
	}
	mmap_addr = (char*)mmap(NULL, size,
				mode == fa_read ? PROT_READ :
				mode == fa_write ? PROT_WRITE :
				PROT_READ|PROT_WRITE,
				MAP_FILE|MAP_SHARED,
				fd, 0);
	if (mmap_addr != (char*)MAP_FAILED) {
	    mmap_size = size;
	    return ok;
	}
	mmap_addr = NULL;
	status = iop_status(errno);
	os_file::close();
    }
    return status;
}

file::iop_status mmap_file::close()
{
    if (opened) {
	if (munmap(mmap_addr, mmap_size) != 0) {
	    return iop_status(errno);
	}
	return os_file::close();
    }
    return ok;
}

file::iop_status mmap_file::set_size(fsize_t size)
{
    if (opened) {
	size_t new_size = size_t(size);
	assert(new_size == size);
	if (new_size > mmap_size) {
	    void* new_addr = NULL;
	    if (munmap(mmap_addr, mmap_size) != ok
	        || ftruncate(fd, new_size) != ok
		|| (new_addr = (char*)mmap(NULL, new_size,
					   mode == fa_read ? PROT_READ :
					   mode == fa_write ? PROT_WRITE :
					   PROT_READ|PROT_WRITE,
					   MAP_FILE|MAP_SHARED,
					   fd, 0)) == (char*)MAP_FAILED)
	    {
		return iop_status(errno);
	    }
	    mmap_addr = (char*)new_addr;
	} else if (new_size < mmap_size) {
	    int page_size = getpagesize();
	    new_size = DOALIGN(new_size, page_size);
	    if (new_size < mmap_size) {
		if (munmap((char*)mmap_addr+new_size, mmap_size-new_size) != 0
		    || ftruncate(fd, new_size) != 0 )
		{
		    return iop_status(errno);
		}
	    }
	}
	mmap_size = new_size;
	return ok;
    } else {
	return not_opened;
    }
}

file::iop_status mmap_file::get_size(fsize_t& size) const
{
    if (opened) {
	size = mmap_size;
	return ok;
    } else {
	return not_opened;
    }
}

file::iop_status mmap_file::flush()
{
    if (opened) {
	if (msync(mmap_addr, mmap_size, MS_SYNC) == 0) {
	    return ok;
	} else {
	    return iop_status(errno);
	}
    } else {
	return not_opened;
    }
}



