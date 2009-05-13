//-< CONSOLE.H >-----------------------------------------------------*--------*
// SAL                       Version 1.0         (c) 1997  GARRET    *     ?  *
// (System Abstraction Layer)                                        *   /\|  *
//                                                                   *  /  \  *
//                          Created:      7-Jan-97    K.A. Knizhnik  * / [] \ *
//                          Last update: 21-Nov-98    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Handle information and error messages at virtual console.
//-------------------------------------------------------------------*--------*

#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include "stdtp.h"

class console { 
  protected:
    virtual void output_data(boolean error, const char* msg, va_list args);
    virtual boolean input_data(char* buf, size_t buf_size);
 
  public:
    static console* active_console; 
    //
    // Output information at console.
    //
    static void output(const char* msg, ...); 
    //
    // Output message at console and terminate program. 
    //
    static void error(const char* msg, ...);       

    //
    // Input information from console
    //
    static boolean input(char* buf, size_t buf_size);

    console();
}; 

#endif



