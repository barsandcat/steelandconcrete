#ifndef CHANGE_H
#define CHANGE_H

#include <Typedefs.h>
#include <ChangeInterface.h>
#include <ChangeList.pb.h>

template <typename Type, int id>
class Change: public ChangeInterface
{
public:
    Change(Type aValue): mValue(aValue) {}

    void FillMsg(FieldDiffMsg& aMsg)
    {
        aMsg.set_id(id);
        SetValue(aMsg, mValue);
    }

    ~Change() {}
private:
    Change(const Change& other) {}
    Change& operator=(const Change& other) {}
private:
    Type mValue;
    void SetValue(FieldDiffMsg& aMsg, uint32 aValue)
    {
        aMsg.set__int(aValue);
    }

    void SetValue(FieldDiffMsg& aMsg, bool aValue)
    {
        aMsg.set__bool(aValue);
    }

    void SetValue(FieldDiffMsg& aMsg, float aValue)
    {
        aMsg.set__float(aValue);
    }

};

#endif // CHANGE_H
