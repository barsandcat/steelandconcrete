//-< MMAPFILE.CXX >--------------------------------------------------*--------*
// SAL                       Version 1.0         (c) 1997  GARRET    *     ?  *
// (System Abstraction Layer)                                        *   /\|  *
//                                                                   *  /  \  *
//                          Created:      7-Jan-97    K.A. Knizhnik  * / [] \ *
//                          Last update: 21-Nov-98    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Mapped on memory file abstraction
//-------------------------------------------------------------------*--------*

#ifndef __MMAPFILE_H__
#define __MMAPFILE_H__

#include "osfile.h"

class mmap_file : public os_file { 
  protected:
    descriptor_t md; 
    size_t       init_size; // initial size of memory map for created file
    size_t       mmap_size;
    char*        mmap_addr;

  public:
    char*              get_mmap_addr() const { return mmap_addr; }

    virtual iop_status get_size(fsize_t& size) const;
    virtual iop_status set_size(fsize_t new_size);

    virtual iop_status flush();
    virtual iop_status close();
    virtual iop_status open(access_mode mode, int flags=0);

    mmap_file(const char* name, size_t init_size) : os_file(name) {
	mmap_addr = NULL;
	this->init_size = init_size; 
    }
};


#endif
