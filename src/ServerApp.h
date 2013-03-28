#ifndef SERVERAPP_H
#define SERVERAPP_H

#include <gflags/gflags.h>

DECLARE_string(address);

void Run(int argc, char **argv, const bool& aContinue);

#endif
