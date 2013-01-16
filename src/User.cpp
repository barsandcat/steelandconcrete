#include <pch.h>
#include <User.h>

#include <MindList.h>
#include <openssl/srp.h>

DEFINE_string(srp_default_gN, "1024", "Name of preset g and N for SRP");

User::User(const char* aName, const char* aPassword):
    mName(aName), mSalt(NULL), mVerifier(NULL)
{
    SRP_gN *GN = SRP_get_default_gN(FLAGS_srp_default_gN.c_str());
    if(GN == NULL)
    {
        boost::throw_exception(std::runtime_error("Error in SRP_get_default_gN"));
    }

    if (!SRP_create_verifier_BN(aName, aPassword, &mSalt, &mVerifier, GN->N, GN->g))
    {
        boost::throw_exception(std::runtime_error("Error in SRP_create_verifier_BN"));
    }

    mMind = MindList::GetFreeMind();
    mMind->SetFree(false);
}


User::~User()
{
    BN_clear_free(mSalt);
    BN_clear_free(mVerifier);
    mMind->SetFree(true);
}
