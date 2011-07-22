#ifndef VISUALCODES_H_INCLUDED
#define VISUALCODES_H_INCLUDED
#include <OgreString.h>
#include <Typedefs.h>

namespace VC
{
    // 0000 0000  0000 0000  0000 0000  0000 0000
    const uint32 KIND_SIZE = 1;
    const uint32 KINGDOM_SIZE = 2;
    const uint32 PHYLUM_SIZE = 2;

    const uint32 KINGDOM_OFFSET = KIND_SIZE;
    const uint32 PHYLUM_OFFSET = KINGDOM_OFFSET + KINGDOM_SIZE;

    const uint32 KIND_MASK = 1;
    const uint32 KINGDOM_MASK = 6;
    const uint32 PHYLUM_MASK = 24;

    const uint32 NOT_LIVE = 0;
    const uint32 LIVE = 1;

    const uint32 PLANT = 0 << KINGDOM_OFFSET;
    const uint32 ANIMAL = 1 << KINGDOM_OFFSET;

    const uint32 HUMAN = 0 << PHYLUM_OFFSET;
    const uint32 HERBIVORES = 1 << PHYLUM_OFFSET;
}

Ogre::String GetMesh(uint32 aVisualCode);


#endif // VISUALCODES_H_INCLUDED
