//-< WINFILE.CXX >---------------------------------------------------*--------*
// SAL                       Version 1.0         (c) 1997  GARRET    *     ?  *
// (System Abstraction Layer)                                        *   /\|  *
//                                                                   *  /  \  *
//                          Created:      7-Jan-97    K.A. Knizhnik  * / [] \ *
//                          Last update: 21-Nov-98    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// File implementation for Windows
//-------------------------------------------------------------------*--------*

#include "mmapfile.h"

#define BAD_POS 0xFFFFFFFF // returned by SetFilePointer and GetFileSize

local int win_access_mode[] = {
    GENERIC_READ, GENERIC_WRITE, GENERIC_READ | GENERIC_WRITE 
};

local int win_open_flags[] = {
    OPEN_EXISTING, TRUNCATE_EXISTING, OPEN_ALWAYS, CREATE_ALWAYS
};

local int win_open_attrs[] = {
    FILE_FLAG_SEQUENTIAL_SCAN,
    FILE_FLAG_SEQUENTIAL_SCAN|FILE_FLAG_WRITE_THROUGH, 
    FILE_FLAG_RANDOM_ACCESS,
    FILE_FLAG_RANDOM_ACCESS|FILE_FLAG_WRITE_THROUGH,
};

local int win_page_access[] = {
    PAGE_READONLY, PAGE_READWRITE, PAGE_READWRITE 
};

local int win_map_access[] = {
    FILE_MAP_READ, FILE_MAP_WRITE, FILE_MAP_ALL_ACCESS 
};

class OS_info : public OSVERSIONINFO { 
  public: 
    OS_info() { 
	dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(this);
    }
};

static OS_info osinfo;
	

local file::iop_status get_system_error()
{
    int error = GetLastError();
    switch (error) { 
      case ERROR_HANDLE_EOF:
        return file::end_of_file;
      case NO_ERROR:
        return file::ok;
    }
    return file::iop_status(error);
} 

file::iop_status os_file::set_position(fposi_t pos)
{
    if (opened) { 
	LONG high_pos = nat8_high_part(pos); 
        if (SetFilePointer(fd, nat8_low_part(pos), &high_pos, FILE_BEGIN)
            == BAD_POS)
        {
            return get_system_error();
        }
        return ok;
    }
    return not_opened; 
}

file::iop_status os_file::get_position(fposi_t& pos)
{
    if (opened) { 
        LONG  high_pos = 0;
        DWORD low_pos = SetFilePointer(fd, 0, &high_pos, FILE_CURRENT);
	pos = cons_nat8(high_pos, low_pos);	
        if (low_pos == BAD_POS) {
            return get_system_error();
        }
        return ok;
    }
    return not_opened; 
} 

file::iop_status os_file::read(void* buf, size_t size)
{  
    if (opened) { 
	DWORD read_bytes;
        return ReadFile(fd, buf, size, &read_bytes, NULL)
            ? size == read_bytes ? ok : end_of_file
            : get_system_error();
    } else {
	return not_opened;
    }
}

file::iop_status os_file::write(void const* buf, size_t size)
{  
    if (opened) {  
	DWORD written_bytes;
	return WriteFile(fd,buf, size, &written_bytes, NULL) 
	    ? (DWORD)size == written_bytes ? ok : end_of_file  
	    : get_system_error();
    } else { 
	return not_opened;
    }
}

file::iop_status os_file::read(fposi_t pos, void* buf, size_t size) 
{
    if (opened) { 
	DWORD read_bytes;
	if (osinfo.dwPlatformId == VER_PLATFORM_WIN32_NT) { 
	    OVERLAPPED Overlapped;
	    Overlapped.Offset = nat8_low_part(pos);
	    Overlapped.OffsetHigh = nat8_high_part(pos);
            Overlapped.hEvent = NULL;
	    return ReadFile(fd, buf, size, &read_bytes, &Overlapped)
		? size == read_bytes ? ok : end_of_file
		: get_system_error(); 
	} else { 
	    critical_section cs(guard);
	    LONG high_pos = nat8_high_part(pos); 
            if (SetFilePointer(fd, nat8_low_part(pos), &high_pos, FILE_BEGIN)
                == BAD_POS)
            {
                iop_status rc = get_system_error();
                if (rc != ok) {
                    return rc;
                }
	    }
            boolean success = ReadFile(fd, buf, size, &read_bytes, NULL);
	    return success ? size == read_bytes ? ok : end_of_file
		: get_system_error();
	}
    } else { 
	return not_opened;
    }
}

file::iop_status os_file::write(fposi_t pos, void const* buf, size_t size) 
{
    if (opened) { 
	DWORD written_bytes;
	if (osinfo.dwPlatformId == VER_PLATFORM_WIN32_NT) {
	    OVERLAPPED Overlapped;
	    Overlapped.Offset = nat8_low_part(pos);
	    Overlapped.OffsetHigh = nat8_high_part(pos);
	    Overlapped.hEvent = NULL;
	    return WriteFile(fd, buf, size, &written_bytes, &Overlapped)
		? size == written_bytes ? ok : end_of_file
		: get_system_error(); 
	} else { 
	    critical_section cs(guard);
	    LONG high_pos = nat8_high_part(pos); 
            if (SetFilePointer(fd, nat8_low_part(pos), &high_pos, FILE_BEGIN)
                == BAD_POS)
            {
                iop_status rc = get_system_error();
                if (rc != ok) {
                    return rc;
                }
	    }
            boolean success = WriteFile(fd, buf, size, &written_bytes, NULL);
	    return success ? size == written_bytes ? ok : end_of_file
		: get_system_error();
	}
    } else { 
	return not_opened;
    }
}

file::iop_status os_file::close()
{
    if (opened) { 
	opened = False;
        return CloseHandle(fd) ? ok : get_system_error();
    } 
    return ok;
}

file::iop_status os_file::open(access_mode mode, int flags)
{
    close();
    assert(name != NULL); 
    fd = CreateFile(name, win_access_mode[mode],
		    (flags & fo_exclusive) ? 0 : (flags & fo_shared) 
		    ? FILE_SHARE_READ : FILE_SHARE_READ|FILE_SHARE_WRITE, 
		    NULL, win_open_flags[flags & (fo_truncate|fo_create)],
		    win_open_attrs[(flags & (fo_sync|fo_random)) >> 2],
		    NULL); 
    this->mode = mode;
    this->flags = flags;
    if (fd == INVALID_HANDLE_VALUE) {
        return get_system_error();
    } else { 
	opened = True;
	return ok;
    }
}

file::iop_status os_file::remove()
{
    close();
    return DeleteFile(name) 
	? ok : get_system_error();
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
                    if (MoveFile(name, new_name)) {
			delete[] name;
			name = strdup(new_name);
		    } else {
			return get_system_error();
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
	DWORD high_size;
        DWORD low_size = GetFileSize(fd, &high_size);
	size = cons_nat8(high_size, low_size);
        if (low_size == BAD_POS) {
            return get_system_error();
        }
	return ok;
    } else {
	return not_opened;
    }
}

file::iop_status os_file::set_size(fsize_t size)
{
    if (opened) { 
	LONG high_part = nat8_high_part(size); 
        if (SetFilePointer(fd, nat8_low_part(size), &high_part, FILE_BEGIN)
            != BAD_POS || get_system_error() == ok)
        {
            if (SetEndOfFile(fd)) {
		return ok;
	    }
	}
	return get_system_error();
    } else {
        return not_opened;
    }
}

file::iop_status os_file::flush()
{
    if (opened) { 
	return FlushFileBuffers(fd) ? ok : get_system_error();
    } else {
	return not_opened;
    }
}

void os_file::get_error_text(iop_status code, char* buf, size_t buf_size) const
{
    int len;
    switch (code) { 
      case ok:
	strncpy(buf, "Ok", buf_size);
	break;
      case not_opened:
        strncpy(buf, "file not opened", buf_size);
	break;
      case end_of_file:
	strncpy(buf, "operation not completly finished", buf_size);
	break;
      default:
        len = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
		      NULL,
		      code,
		      0,
		      buf,
		      buf_size,
		      NULL);
	if (len == 0) { 
	    char errcode[64];
	    sprintf(errcode, "unknown error code %u", code);
	    strncpy(buf, errcode, buf_size);
	}
    }
}

size_t os_file::get_disk_block_size()
{
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    return sysinfo.dwPageSize;
}

void* os_file::allocate_disk_buffer(size_t size)
{
    return VirtualAlloc(NULL, size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
}

void  os_file::free_disk_buffer(void* buf)
{
    VirtualFree(buf, 0, MEM_RELEASE);
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

file::iop_status mmap_file::open(access_mode mode, int flags)
{ 
    iop_status status = os_file::open(mode, flags);
    if (status == ok) { 
	DWORD high_size;
        DWORD size = GetFileSize(fd, &high_size);
        if (size == BAD_POS && (status = get_system_error()) != ok) {
            os_file::close();
	    return status;
        }
        assert(high_size == 0);

        mmap_size = (size < init_size) ? init_size : size;
        md = CreateFileMapping(fd, NULL,
			       win_page_access[mode],
                               0, mmap_size, NULL);
	if (md == NULL) { 
	    status = get_system_error();
	    os_file::close();
	    return status;
	}
	mmap_addr = (char*)MapViewOfFile(md, win_map_access[mode], 0, 0, 0);
	if (mmap_addr == NULL) { 
	    status = get_system_error();
	    os_file::close();
	    return status;
	}
	if (osinfo.dwPlatformId != VER_PLATFORM_WIN32_NT) { 
	    // Windows 95 doesn't initialize pages
	    memset(mmap_addr+size, 0, mmap_size - size);
	}
    }
    return status; 
}

file::iop_status mmap_file::close()
{
    if (opened) { 
	if (!UnmapViewOfFile(mmap_addr) || !CloseHandle(md)) { 
	    return get_system_error();
	} 
	return os_file::close();
    }
    return ok;
}

file::iop_status mmap_file::set_size(fsize_t size)
{
    if (opened) { 
	iop_status status;
	size_t new_size = size_t(size);
	assert(size == new_size); // no truncation 

	if (new_size > mmap_size) { 
	    if (!UnmapViewOfFile(mmap_addr) || !CloseHandle(md)) { 
		return get_system_error();
	    } 
	    md = CreateFileMapping(fd, NULL, 
				   win_page_access[mode],
				   0, new_size, NULL);
	    if (md == NULL) { 
		status = get_system_error();
		os_file::close();
		return status;
	    }
	    mmap_addr = (char*)MapViewOfFile(md, win_map_access[mode], 
					     0, 0, 0);
	    if (mmap_addr == NULL) { 
		status = get_system_error();
		os_file::close();
		return status;
	    }
	    if (osinfo.dwPlatformId != VER_PLATFORM_WIN32_NT) {
		// Windows 95 doesn't initialize pages
		memset(mmap_addr+mmap_size, 0, new_size - mmap_size);
	    } 
	    mmap_size = new_size;
	} 
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
	return FlushViewOfFile(mmap_addr, mmap_size)
	    ? ok : get_system_error();
    } else { 
	return not_opened;
    }	
}

