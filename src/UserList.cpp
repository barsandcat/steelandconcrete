#include <pch.h>

#include <UserList.h>

typedef boost::ptr_map<Ogre::String, User> UserMap;

UserMap theUserList;

void AddUser(const char* aUserName, const char* aPassword)
{
    std::auto_ptr<User> user(new User(aUserName, aPassword));
    theUserList.insert(aUserName, user);
}

const User* GetUser(const char* aUser)
{
    UserMap::const_iterator it = theUserList.find(aUser);
    if (it != theUserList.end())
    {
        return it->second;
    }
    else
    {
        return NULL;
    }

}
