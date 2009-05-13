//-< MULTFILE.CXX >--------------------------------------------------*--------*
// SAL                       Version 1.0         (c) 1997  GARRET    *     ?  *
// (System Abstraction Layer)                                        *   /\|  *
//                                                                   *  /  \  *
//                          Created:     15-Apr-97    K.A. Knizhnik  * / [] \ *
//                          Last update: 21-Nov-97    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// File scattered to several segments
//-------------------------------------------------------------------*--------*

#include "multfile.h"

void multifile::get_error_text(iop_status code,char* buf,size_t buf_size) const
{
    files->get_error_text(code, buf, buf_size);
}

file::iop_status multifile::set_position(fposi_t pos)
{
    cur_pos = pos;
    return ok;
}

file::iop_status multifile::get_position(fposi_t& pos)
{
    pos = cur_pos;
    return ok;
}

file::iop_status multifile::read(void* buf, size_t size)
{
    cs.enter();
    fposi_t pos = cur_pos;
    cur_pos += size;
    cs.leave();
    return read(pos, buf, size);
}

file::iop_status multifile::write(void const* buf, size_t size)
{
    cs.enter();
    fposi_t pos = cur_pos;
    cur_pos += size;
    cs.leave();
    return write(pos, buf, size);
}

file::iop_status multifile::read(fposi_t pos, void* buf, size_t size)
{
    int i = 0;
    int n  = n_files - 1;
    char* dst = (char*)buf; 

    while (True) { 
	if (pos < segments[i].size || i == n) { 
	    if (pos + size <= segments[i].size || i == n) { 
		return files[i].read(pos, dst, size);
	    } else {
		size_t segm_size = size_t(segments[i].size - pos);
                iop_status status = files[i++].read(pos, dst, segm_size);
		if (status == ok) { 
		    pos = 0;
		    dst += segm_size;
		    size -= segm_size;
		} else { 
		    return status;
		}
	    }
	} else { 
	    pos -= segments[i++].size;
	}
    }    
}

file::iop_status multifile::write(fposi_t pos, void const* buf, size_t size)
{
    int i = 0;
    int n  = n_files - 1;
    char* src = (char*)buf; 

    while (True) { 
	if (pos < segments[i].size || i == n) { 
	    if (pos + size <= segments[i].size || i == n) { 
		return files[i].write(pos, src, size);
	    } else {
		size_t segm_size = size_t(segments[i].size - pos);
                iop_status status = files[i++].write(pos, src, segm_size);
		if (status == ok) { 
		    pos = 0;
		    src += segm_size;
		    size -= segm_size;
		} else { 
		    return status;
		}
	    }
	} else { 
	    pos -= segments[i++].size;
	}
    }    
}

file::iop_status multifile::flush()
{
    for (int i = 0; i < n_files; i++) { 
        iop_status status = files[i].flush();
        if (status != ok) {
	    return status; 
	}
    }
    return ok;
}

file::iop_status multifile::open(access_mode mode, int flags)
{
    total_size = 0;
    for (int i = 0; i < n_files; i++) { 
	iop_status status = files[i].open(mode, flags);
	if (status != ok) {
	    while (--i >= 0) {
		files[i].close();
	    } 
	    return status;
	}
	total_size += segments[i].size;
    }
    cur_pos = 0;
    this->mode = mode;
    return ok;
}

file::iop_status multifile::close()
{
    for (int i = 0; i < n_files; i++) { 
	iop_status status = files[i].close();
	if (status != ok) { 
	    return status;
	}
    }
    return ok;
}

file::iop_status multifile::remove()
{
    for (int i = 0; i < n_files; i++) { 
	iop_status status = files[i].remove(); 
	if (status != ok) { 
	    return status;
	}
    }
    return ok;
}

char const* multifile::get_name() const
{
    return files[0].get_name();
}

file::iop_status multifile::set_name(char const*) 
{
    for (int i = 0; i < n_files; i++) { 
	iop_status status = files[i].set_name(segments[i].name); 
	if (status != ok) { 
	    return status;
	}
    }
    return ok;
}

file::iop_status multifile::get_size(fsize_t& size) const
{
    iop_status status = files[n_files-1].get_size(size);
    size += total_size - segments[n_files-1].size; 
    return status;
}

file::iop_status multifile::set_size(fsize_t new_size)
{
    if (new_size > total_size - segments[n_files-1].size) {
	return files[n_files-1].set_size
	    (new_size - total_size + segments[n_files-1].size);
    } 
    return ok;
}
    
multifile::multifile(int n_segments, segment* segm)
{
    assert(n_segments > 0);
    segments = segm;
    n_files = n_segments;
    files = new os_file[n_files]; 
    for (int i = 0; i < n_segments; i++) { 
	files[i].set_name(segments[i].name); 
    }
}

multifile::~multifile()
{
    delete[] files;
}

