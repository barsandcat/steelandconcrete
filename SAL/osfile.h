//-< OSFILE.H >------------------------------------------------------*--------*
// SAL                       Version 1.0         (c) 1997  GARRET    *     ?  *
// (System Abstraction Layer)                                        *   /\|  *
//                                                                   *  /  \  *
//                          Created:      7-Jan-97    K.A. Knizhnik  * / [] \ *
//                          Last update: 21-Nov-98    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Interface to operation system dependent file implementation
//-------------------------------------------------------------------*--------*

#ifndef __OSFILE_H__
#define __OSFILE_H__

#include "task.h"
#include "file.h"

//
// Operation system supported file
//

class os_file : public file { 
  protected:
    descriptor_t  fd; 
    char*         name;
    boolean       opened; // is file opened ?

    mutex         guard; // mutex for synchronization of IO operations

  public:                                              
    virtual void        get_error_text(iop_status code,  
				       char* buf, size_t buf_size) const; 

    virtual iop_status  read(fposi_t pos, void* buf, size_t size);
    virtual iop_status  write(fposi_t pos, void const* buf, size_t size);

    virtual iop_status  get_position(fposi_t& pos); 
    virtual iop_status  set_position(fposi_t pos); 

    virtual iop_status  read(void* buf, size_t size);
    virtual iop_status  write(void const* buf, size_t size);

    virtual iop_status  open(access_mode mode, int flags=0);
    virtual iop_status  flush();
    virtual iop_status  close();
    virtual iop_status  remove(); 

    virtual char const* get_name() const;
    virtual iop_status  set_name(char const* new_name);

    virtual iop_status  get_size(fsize_t& size) const;
    virtual iop_status  set_size(fsize_t new_size);
    
    //
    // Return file system system dependent size of disk block
    //
    static  size_t      get_disk_block_size();
    //
    // Allocate buffer for disk io operation aligned to disk block size
    //
    static  void*       allocate_disk_buffer(size_t size); 
    //
    // Free buffer allocated by "allocate_disk_buffer" function
    //
    static  void        free_disk_buffer(void* buf); 

    os_file(const char* name = NULL);
    ~os_file();
};


#endif
