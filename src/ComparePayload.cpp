#include <pch.h>

#include <ComparePayload.h>
#include <vector>
#include <google/protobuf/descriptor.h>

bool operator==(const google::protobuf::Message& aMsg1, const google::protobuf::Message& aMsg2)
{
    const google::protobuf::Reflection* rfl1 = aMsg1.GetReflection();
    const google::protobuf::Reflection* rfl2 = aMsg2.GetReflection();

    std::vector< const google::protobuf::FieldDescriptor * > aMsg1Fields;
    rfl1->ListFields(aMsg1, &aMsg1Fields);

    std::vector< const google::protobuf::FieldDescriptor * > aMsg2Fields;
    rfl2->ListFields(aMsg2, &aMsg2Fields);

    if (aMsg2Fields.size() != aMsg1Fields.size())
    {
        return false;
    }

    for (size_t i = 0; i < aMsg1Fields.size(); ++i)
    {
        const google::protobuf::FieldDescriptor* field1 = aMsg1Fields.at(i);
        const google::protobuf::FieldDescriptor* field2 = aMsg2Fields.at(i);
        if (field1->type() != field2->type())
        {
            return false;
        }

        if (field1->is_repeated())
        {
            if (rfl1->FieldSize(aMsg1, field1) != rfl2->FieldSize(aMsg2, field2))
                return false;

            const int fieldSize = rfl1->FieldSize(aMsg1, field1);

            for (int y = 0; y < fieldSize; ++y)
            {
                switch (field1->cpp_type())
                {
                case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
                    if (rfl1->GetRepeatedInt32(aMsg1, field1, y) != rfl2->GetRepeatedInt32(aMsg2, field2, y))
                        return false;
                    break;
                case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
                    if (rfl1->GetRepeatedInt64(aMsg1, field1, y) != rfl2->GetRepeatedInt64(aMsg2, field2, y))
                        return false;
                    break;

                case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
                    if (rfl1->GetRepeatedUInt32(aMsg1, field1, y) != rfl2->GetRepeatedUInt32(aMsg2, field2, y))
                        return false;
                    break;

                case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
                    if (rfl1->GetRepeatedUInt64(aMsg1, field1, y) != rfl2->GetRepeatedUInt64(aMsg2, field2, y))
                        return false;
                    break;

                case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
                    if (rfl1->GetRepeatedDouble(aMsg1, field1, y) != rfl2->GetRepeatedDouble(aMsg2, field2, y))
                        return false;
                    break;

                case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
                    if (rfl1->GetRepeatedFloat(aMsg1, field1, y) != rfl2->GetRepeatedFloat(aMsg2, field2, y))
                        return false;
                    break;
                case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
                    if (rfl1->GetRepeatedBool(aMsg1, field1, y) != rfl2->GetRepeatedBool(aMsg2, field2, y))
                        return false;
                    break;
                case google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
                    if (rfl1->GetRepeatedEnum(aMsg1, field1, y) != rfl2->GetRepeatedEnum(aMsg2, field2, y))
                        return false;
                    break;
                case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
                    if (rfl1->GetRepeatedString(aMsg1, field1, y) != rfl2->GetRepeatedString(aMsg2, field2, y))
                        return false;
                    break;

                case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
                    if (!(rfl1->GetRepeatedMessage(aMsg1, field1, y) == rfl2->GetRepeatedMessage(aMsg2, field2, y)))
                        return false;
                    break;
                }
            }
        }
        else
        {
            switch (field1->cpp_type())
            {
            case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
                if (rfl1->GetInt32(aMsg1, field1) != rfl2->GetInt32(aMsg2, field2))
                    return false;
                break;
            case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
                if (rfl1->GetInt64(aMsg1, field1) != rfl2->GetInt64(aMsg2, field2))
                    return false;
                break;

            case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
                if (rfl1->GetUInt32(aMsg1, field1) != rfl2->GetUInt32(aMsg2, field2))
                    return false;
                break;

            case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
                if (rfl1->GetUInt64(aMsg1, field1) != rfl2->GetUInt64(aMsg2, field2))
                    return false;
                break;

            case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
                if (rfl1->GetDouble(aMsg1, field1) != rfl2->GetDouble(aMsg2, field2))
                    return false;
                break;

            case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
                if (rfl1->GetFloat(aMsg1, field1) != rfl2->GetFloat(aMsg2, field2))
                    return false;
                break;
            case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
                if (rfl1->GetBool(aMsg1, field1) != rfl2->GetBool(aMsg2, field2))
                    return false;
                break;
            case google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
                if (rfl1->GetEnum(aMsg1, field1) != rfl2->GetEnum(aMsg2, field2))
                    return false;
                break;
            case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
                if (rfl1->GetString(aMsg1, field1) != rfl2->GetString(aMsg2, field2))
                    return false;
                break;

            case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
                if (!(rfl1->GetMessage(aMsg1, field1) == rfl2->GetMessage(aMsg2, field2)))
                    return false;
                break;
            }
        }
    }

    return true;
}
