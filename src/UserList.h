#ifndef USERLIST_H_INCLUDED
#define USERLIST_H_INCLUDED

#include <User.h>
#include <Typedefs.h>

void AddUser(const char* aUserName, const char* aPassword);
const User* GetUser(const char* aUser);

#endif // USERLIST_H_INCLUDED
