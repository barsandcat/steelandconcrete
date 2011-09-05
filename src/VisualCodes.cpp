#include <pch.h>
#include <VisualCodes.h>
using namespace VC;

Ogre::String GetMesh(uint32 aVisualCode)
{
    switch (aVisualCode & KIND_MASK)
    {
    case LIVE:
        switch (aVisualCode & KINGDOM_MASK)
        {
        case PLANT:
            return "Grass.mesh";
        case ANIMAL:
            switch (aVisualCode & PHYLUM_MASK)
            {
            case HUMAN:
                return "robot.mesh";
            case HERBIVORES:
                return "zebra.mesh";
            }
        }
    case NOT_LIVE:
        ;
    }
    return "";
}
