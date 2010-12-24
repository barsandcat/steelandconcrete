/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2006 Torus Knot Software Ltd
Also see acknowledgements in Readme.html

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.

You may alternatively use this source under the terms of a specific version of
the OGRE Unrestricted License provided you have obtained such a license from
Torus Knot Software Ltd.
-----------------------------------------------------------------------------
*/
#include "PassController.h"

#include "OgrePass.h"
#include "TechniqueController.h"

PassController::PassController(Pass* pass)
: mParentController(NULL), mPass(pass)
{
}

PassController::PassController(TechniqueController* parent, Pass* pass)
: mParentController(parent), mPass(pass)
{
}

PassController::~PassController()
{
}

TechniqueController* PassController::getParentController() const
{
	return mParentController;
}

Pass* PassController::getPass() const
{
	return mPass;
}

void PassController::setName(const String& name)
{
	mPass->setName(name);
}

void PassController::setAmbient(Real red, Real green, Real blue)
{
	mPass->setAmbient(red, green, blue);
}

void PassController::setAmbient(const ColourValue& ambient)
{
	mPass->setAmbient(ambient);
}

void PassController::setDiffuse(Real red, Real green, Real blue, Real alpha)
{
	mPass->setDiffuse(red, green, blue, alpha);
}

void PassController::setDiffuse(const ColourValue &diffuse)
{
	mPass->setDiffuse(diffuse);
}

void PassController::setSpecular(Real red, Real green, Real blue, Real alpha)
{
	mPass->setSpecular(red, green, blue, alpha);
}

void PassController::setSpecular(const ColourValue &specular)
{
	mPass->setSpecular(specular);
}

void PassController::setShininess(Real val)
{
	mPass->setShininess(val);
}

void PassController::setSelfIllumination(Real red, Real green, Real blue)
{
	mPass->setSelfIllumination(red, green, blue);
}

void PassController::setSelfIllumination(const ColourValue& selfIllum)
{
	mPass->setSelfIllumination(selfIllum);
}

void PassController::setVertexColourTracking(TrackVertexColourType tracking)
{
	mPass->setVertexColourTracking(tracking);
}

void PassController::setPointSize(Real ps)
{
	mPass->setPointSize(ps);
}

void PassController::setPointSpritesEnabled(bool enabled)
{
	mPass->setPointSpritesEnabled(enabled);
}

void PassController::setPointAttenuation(bool enabled, Real constant /* =0.0f */, Real linear /* =1.0f */, Real quadratic /* =0.0f */)
{
	mPass->setPointAttenuation(enabled, constant, linear, quadratic);
}

void PassController::setPointMinSize(Real min)
{
	mPass->setPointMinSize(min);
}

void PassController::setPointMaxSize(Real max)
{
	mPass->setPointMaxSize(max);
}

void PassController::setSceneBlending(const SceneBlendType sbt)
{
	mPass->setSceneBlending(sbt);
}

void PassController::setSceneBlending(const SceneBlendFactor sourceFactor, const SceneBlendFactor destFactor)
{
	mPass->setSceneBlending(sourceFactor, destFactor);
}

void PassController::setDepthCheckEnabled(bool enabled)
{
	mPass->setDepthCheckEnabled(enabled);
}

void PassController::setDepthWriteEnabled(bool enabled)
{
	mPass->setDepthWriteEnabled(enabled);
}

void PassController::setDepthFunction(CompareFunction func)
{
	mPass->setDepthFunction(func);
}

void PassController::setColourWriteEnabled(bool enabled)
{
	mPass->setColourWriteEnabled(enabled);
}

void PassController::setCullingMode(CullingMode mode)
{
	mPass->setCullingMode(mode);
}

void PassController::setManualCullingMode(ManualCullingMode mode)
{
	mPass->setManualCullingMode(mode);
}

void PassController::setLightingEnabled(bool enabled)
{
	mPass->setLightingEnabled(enabled);
}

void PassController::setMaxSimultaneousLights(unsigned short maxLights)
{
	mPass->setMaxSimultaneousLights(maxLights);
}

void PassController::setStartLight(unsigned short startLight)
{
	mPass->setStartLight(startLight);
}

void PassController::setShadingMode(ShadeOptions mode)
{
	mPass->setShadingMode(mode);
}

void PassController::setPolygonMode(PolygonMode mode)
{
	mPass->setPolygonMode(mode);
}

void PassController::setFog(bool overrideScene, FogMode mode /* =FOG_NONE */, const ColourValue& colour /* =ColourValue::White */, Real expDensity /* =0.001 */, Real linearStart /* =0.0 */, Real linearEnd /* =1.0 */)
{
	mPass->setFog(overrideScene, mode, colour, expDensity, linearStart, linearEnd);
}

void PassController::setDepthBias(float constantBias, float slopeScaleBias /* =0.0f */)
{
	mPass->setDepthBias(constantBias, slopeScaleBias);
}

void PassController::setAlphaRejectSettings(CompareFunction func, unsigned char value)
{
	mPass->setAlphaRejectSettings(func, value);
}

void PassController::setAlphaRejectFunction(CompareFunction func)
{
	mPass->setAlphaRejectFunction(func);
}

void PassController::setAlphaRejectValue(unsigned char val)
{
	mPass->setAlphaRejectValue(val);
}

void PassController::setIteratePerLight(bool enabled, bool onlyForOneLightType /* =true */, Light::LightTypes lightType /*=Light::LT_POINT */)
{
	mPass->setIteratePerLight(enabled, onlyForOneLightType, lightType);
}

void PassController::setLightCountPerIteration(unsigned short c)
{
	mPass->setLightCountPerIteration(c);
}

void PassController::setVertexProgram(const String& name, bool resetParams /* =true */)
{
	mPass->setVertexProgram(name, resetParams);
}

void PassController::setVertexProgramParameters(GpuProgramParametersSharedPtr params)
{
	mPass->setVertexProgramParameters(params);

	// TODO: Fire event
}

void PassController::setShadowCasterVertexProgram(const String& name)
{
	mPass->setShadowCasterVertexProgram(name);
}

void PassController::setShadowCasterVertexProgramParameters(GpuProgramParametersSharedPtr params)
{
	mPass->setShadowCasterVertexProgramParameters(params);
}

void PassController::setShadowReceiverVertexProgram(const String& name)
{
	mPass->setShadowReceiverVertexProgram(name);
}

void PassController::setShadowReceiverVertexProgramParameters(GpuProgramParametersSharedPtr params)
{
	mPass->setShadowReceiverVertexProgramParameters(params);
}

void PassController::setShadowReceiverFragmentProgram(const String& name)
{
	mPass->setShadowReceiverFragmentProgram(name);
}

void PassController::setShadowReceiverFragmentProgramParameters(GpuProgramParametersSharedPtr params)
{
	mPass->setShadowReceiverFragmentProgramParameters(params);
}

void PassController::setFragmentProgram(const String& name, bool resetParams /* =true */)
{
	mPass->setFragmentProgram(name, resetParams);
}

void PassController::setFragmentProgramParameters(GpuProgramParametersSharedPtr params)
{
	mPass->setFragmentProgramParameters(params);
}

void PassController::setTextureFiltering(TextureFilterOptions filterType)
{
	mPass->setTextureFiltering(filterType);
}

void PassController::setTextureAnisotropy(unsigned int maxAniso)
{
	mPass->setTextureAnisotropy(maxAniso);
}
