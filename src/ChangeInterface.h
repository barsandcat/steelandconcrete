#ifndef CHANGEINTERFACE_H
#define CHANGEINTERFACE_H
#include <ChangeList.pb.h>

class ChangeInterface
{
    virtual void FillMsg(FieldDiffMsg& aMsg) = 0;
};

#endif // CHANGEINTERFACE_H
