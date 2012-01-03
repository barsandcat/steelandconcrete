#include <pch.h>
#include <Mind.h>

#include <ServerUnit.h>
#include <ServerTile.h>
#include <UnitList.h>

Mind::Mind(UnitId aUnitId):mTarget(NULL), mUnitId(aUnitId), mIsFree(true)
{
    //ctor
}

Ogre::Radian CalcDistance3(const Ogre::Vector3& a, const Ogre::Vector3& b)
{
    Ogre::Real lenProduct = a.length() * b.length();

    // Divide by zero check
    if(lenProduct < 1e-6f)
        lenProduct = 1e-6f;

    Ogre::Real f = a.dotProduct(b) / lenProduct;

    f = Ogre::Math::Clamp(f, (Ogre::Real)-1.0, (Ogre::Real)1.0);
    return Ogre::Math::ACos(f);
}

void Mind::Update(GameTime aPeriod)
{
    ServerUnit* unit = UnitList::GetUnit(mUnitId);
    if (mIsFree)
    {
        ServerTile& position = unit->GetUnitTile();
        ServerTile& randomTile = position.GetNeighbour(rand() % position.GetNeighbourCount());
        if(randomTile.CanEnter())
        {
            unit->Move(randomTile);
        }
    }
    else
    {
        if (mTarget)
        {
            ServerTile& currentTile = unit->GetUnitTile();
            ServerTile* currentTarget = &currentTile.GetNeighbour(0);
            Ogre::Radian currentDist = CalcDistance3(currentTarget->GetPosition(), mTarget->GetPosition());
            for (size_t i = 1; i < currentTile.GetNeighbourCount(); ++i)
            {
                Ogre::Radian dist = CalcDistance3(currentTile.GetNeighbour(i).GetPosition(), mTarget->GetPosition());
                if (dist < currentDist)
                {
                    currentTarget = &currentTile.GetNeighbour(i);
                    currentDist = dist;
                }
            }

            if (currentTarget->CanEnter())
            {
                unit->Move(*currentTarget);
                if (mTarget == currentTarget)
                {
                    mTarget = NULL;
                    //ChangeList::AddCommandDone(mUnitId);
                }
            }
        }
    }
}
