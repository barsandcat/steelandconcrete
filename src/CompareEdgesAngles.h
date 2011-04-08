#ifndef COMPAREEDGESANGLES_H_INCLUDED
#define COMPAREEDGESANGLES_H_INCLUDED

template <typename T>
class CompareEdgesAngles
{
public:
    CompareEdgesAngles(const Ogre::Vector3& aRoot, const Ogre::Vector3&  aPole): mRoot(aRoot), mPole(aPole) {}
    bool operator()(T* a, T* b)
    {
        return CalcAngle(a->GetPosition()) < CalcAngle(b->GetPosition());
    }

    Ogre::Radian CalcAngle(const Ogre::Vector3& u) const
    {
        // Two vectors that forms angle between pole and neighbour
        Ogre::Vector3 a = mRoot.crossProduct(u);
        Ogre::Vector3 b = mRoot.crossProduct(mPole);

        // Angle
        Ogre::Real C = a.angleBetween(b).valueRadians();

        // Rotation axis
        Ogre::Vector3 normal(a.crossProduct(b).normalisedCopy());

        // Correct angle, if rotation axis is not equal to our positon vector -
        if (normal != Ogre::Vector3::ZERO)
        {
            Ogre::Vector3 delta(mRoot - normal);
            if (delta.squaredLength() > 0.1)
            {
                C = Ogre::Math::TWO_PI - C;
            }
        }

        assert(C >= 0 && C <= Ogre::Math::TWO_PI);
        return Ogre::Radian(C);
    }
private:
    const Ogre::Vector3&  mRoot;
    const Ogre::Vector3&  mPole;
};


#endif // COMPAREEDGESANGLES_H_INCLUDED
