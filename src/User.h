#ifndef USER_H
#define USER_H

#include <openssl/bn.h>
#include <Mind.h>

DECLARE_string(srp_default_gN);

class User
{
public:
    User(const char* aName, const char* aPassword);
    ~User();
    UnitId GetUnitId() const { return mMind->GetUnitId(); }
    Mind* GetMind() const { return mMind; }
    BIGNUM* GetSalt() const { return mSalt; }
    BIGNUM* GetVerifier() const { return mVerifier; }
private:
    Mind* mMind;
    BIGNUM* mSalt;
    BIGNUM* mVerifier;
    Ogre::String mName;

};

#endif // USER_H
