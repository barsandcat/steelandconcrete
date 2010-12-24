#ifndef _PASSCONTROLLER_H_
#define _PASSCONTROLLER_H_

#include <list>

#include "OgreBlendMode.h"
#include "OgreCommon.h"
#include "OgreLight.h"
#include "OgrePrerequisites.h"

#include "EventContainer.h"

namespace Ogre
{
	class ColourValue;
	class Pass;
}

class TechniqueController;

using namespace Ogre;

class PassController : public EventContainer
{
public:
	PassController(Pass* pass);
	PassController(TechniqueController* parent, Pass* pass);
	virtual ~PassController();

	TechniqueController* getParentController() const;
	Pass* getPass() const;

protected:
	Pass* mPass;
	TechniqueController* mParentController;
};

#endif // _PASSCONTROLLER_H_
