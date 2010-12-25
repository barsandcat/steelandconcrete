#ifndef _PASSCONTROLLER_H_
#define _PASSCONTROLLER_H_

#include "OgreBlendMode.h"
#include "OgreCommon.h"
#include "OgreLight.h"
#include "OgrePrerequisites.h"

namespace Ogre
{
	class ColourValue;
	class Pass;
}

class TechniqueController;

using namespace Ogre;

class PassController
{
public:
	PassController(Pass* pass);
	virtual ~PassController();

	Pass* getPass() const;

protected:
	Pass* mPass;
};

#endif // _PASSCONTROLLER_H_
