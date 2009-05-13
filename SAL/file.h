//-< FILE.H >--------------------------------------------------------*--------*
// SAL                       Version 1.0         (c) 1997  GARRET    *     ?  *
// (System Abstraction Layer)                                        *   /\|  *
//                                                                   *  /  \  *
//                          Created:      7-Jan-97    K.A. Knizhnik  * / [] \ *
//                          Last update: 17-May-97    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// File abstraction
//-------------------------------------------------------------------*--------*

#ifndef __FILE_H__
#define __FILE_H__

#include "stdtp.h"

//
// Abstraction of file 
//

typedef nat8 fsize_t; 
typedef nat8 fposi_t; 

#define MAX_FSIZE MAX_NAT8

class file { 
  public:
    enum access_mode { fa_read, fa_write, fa_readwrite }; 

    enum open_mode { 
	fo_truncate = 0x01, // reset length of file to 0
        fo_create   = 0x02, // create file if not existed
	fo_sync     = 0x04, // wait completion of write operations
	fo_random   = 0x08, // optimize file for random access
	fo_exclusive= 0x10, // prevent file from openning by another process
	fo_shared   = 0x20  // prevent concurrent write access to the file
    };

    //
    // This operation status codes together with operating system
    // dependent codes are used as return value by all file methods  
    //
    enum iop_status { 
	ok          =  0,
	not_opened  = -1, // file was not previously opened
	end_of_file = -2, // read beyond end of file or no space to extend file
	lock_error  = -3  // file is used by another program
    }; 
      
    virtual void        get_error_text(iop_status code,  
				       char* buf, size_t buf_size) const = 0; 

    virtual iop_status  read(fposi_t pos, void* buf, size_t size) = 0;
    virtual iop_status  write(fposi_t pos, void const* buf, size_t size) = 0;

    virtual iop_status  set_position(fposi_t pos) = 0; 
    virtual iop_status  get_position(fposi_t& pos) = 0; 

    virtual iop_status  read(void* buf, size_t size) = 0;
    virtual iop_status  write(void const* buf, size_t size) = 0;

    virtual iop_status  flush() = 0;

    virtual iop_status  open(access_mode mode, int flags=0) = 0;
    virtual iop_status  close() = 0;
    virtual iop_status  remove() = 0; 

    virtual char const* get_name() const = 0;
    virtual iop_status  set_name(char const* new_name) = 0;

    virtual iop_status  get_size(fsize_t& size) const = 0;
    virtual iop_status  set_size(fsize_t new_size) = 0;
    
    virtual~file() {} 

  protected: 
    access_mode mode;
    int         flags;
};

#endif

