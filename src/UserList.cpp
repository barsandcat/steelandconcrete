#include <pch.h>

#include <UserList.h>

typedef boost::ptr_map<Ogre::String, User> UserMap;


boost::shared_mutex theUserListMutex;
UserMap theUserList;

void AddUser(const char* aUserName, const char* aPassword)
{
    std::auto_ptr<User> user(new User(aUserName, aPassword));
    {
        boost::lock_guard<boost::shared_mutex> lg(theUserListMutex);
        theUserList.insert(aUserName, user);
    }
}

const User* GetUser(const char* aUser)
{
    boost::shared_lock<boost::shared_mutex> lg(theUserListMutex);

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
