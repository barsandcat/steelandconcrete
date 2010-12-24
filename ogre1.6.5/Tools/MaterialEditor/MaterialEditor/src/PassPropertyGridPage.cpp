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
#pragma warning(disable:4800)

#include "PassPropertyGridPage.h"

#include <wx/propgrid/advprops.h>

#include "OgreBlendMode.h"
#include "OgreCommon.h"
#include "OgrePass.h"
#include "OgrePrerequisites.h"

#include "PassController.h"

using namespace Ogre;

BEGIN_EVENT_TABLE(PassPropertyGridPage, wxPropertyGridPage)
	EVT_PG_CHANGED(-1, PassPropertyGridPage::propertyChanged)
END_EVENT_TABLE()

PassPropertyGridPage::PassPropertyGridPage(PassController* controller)
: mController(controller)
{
}

PassPropertyGridPage::~PassPropertyGridPage()
{
}

void PassPropertyGridPage::populate()
{
	createGeneralCategory();
	createReflectanceCategory();
	createPointCategory();
	createSceneBlendingCategory();
	createDepthCategory();
	createCullingCategory();
	createIlluminationCategory();
	createFogCategory();
	createAlphaRejectionCategory();
	createMiscCategory();
}

void PassPropertyGridPage::createGeneralCategory()
{
	const Pass* pass = mController->getPass();

	Append(new wxPropertyCategory(wxT("General")));

	// Name
	mNameId = Append(new wxStringProperty(wxT("Name"), wxPG_LABEL, wxString(pass->getName().c_str(), wxConvUTF8)));
	SetPropertyHelpString(mNameId, wxT("Name of this Pass"));
}

void PassPropertyGridPage::createReflectanceCategory()
{
	const Pass* pass = mController->getPass();

	Append(new wxPropertyCategory(wxT("Reflectance")));

	// Ambient
	ColourValue ambient = pass->getAmbient();
	mAmbientId = Append(new wxColourProperty(wxT("Ambient"), wxPG_LABEL, wxColour((int)(255 * ambient.r), (int)(255 * ambient.g), (int)(255 * ambient.b))));
	SetPropertyHelpString(mAmbientId, wxT("Ambient colour reflectance"));

	// Diffuse
	ColourValue diffuse = pass->getDiffuse();
	mDiffuseId = Append(new wxColourProperty(wxT("Diffuse"), wxPG_LABEL, wxColour((int)(255 * diffuse.r), (int)(255 * diffuse.g), (int)(255 * diffuse.b))));
	SetPropertyHelpString(mDiffuseId, wxT("Diffuse colour reflectance"));

	// Specular
	ColourValue specular = pass->getSpecular();
	mSpecularId = Append(new wxColourProperty(wxT("Specular"), wxPG_LABEL, wxColour((int)(255 * specular.r), (int)(255 * specular.g), (int)(255 * specular.b))));
	SetPropertyHelpString(mSpecularId, wxT("Specular colour reflectance"));

	// Shininess
	mShininessId = Append(new wxFloatProperty(wxT("Shininess"), wxPG_LABEL, pass->getShininess()));
	SetPropertyHelpString(mShininessId, wxT("Shininess, affecting the size of specular highlights"));
}


void PassPropertyGridPage::createPointCategory()
{
	const Pass* pass = mController->getPass();

	Append(new wxPropertyCategory(wxT("Point"), wxPG_LABEL));

	// Point Size
	wxPGId pointSize = Append(new wxParentProperty(wxT("Size"), wxPG_LABEL));
	mPointSizeId = AppendIn(pointSize, new wxFloatProperty(wxT("Size"), wxPG_LABEL, pass->getPointSize()));
	SetPropertyHelpString(mPointSizeId, wxT("Point size, affecting the size of points when rendering a point list, or a list of point sprites"));

	// Point Sprites
	mPointSpritesId = Append(new wxBoolProperty(wxT("Point Sprites"), wxPG_LABEL, pass->getPointSpritesEnabled()));

	// Point Attenuation
	mAttenuationId = Append(new wxParentProperty(wxT("Attenuation"),wxPG_LABEL));
	SetPropertyHelpString(mAttenuationId, wxT("Determines how points are attenuated with distance"));
	mPointAttenuationId = AppendIn(mAttenuationId, new wxBoolProperty(wxT("Enabled"), wxPG_LABEL, pass->isPointAttenuationEnabled()));
	mPointMinSizeId = AppendIn(mAttenuationId, new wxFloatProperty(wxT("Min"), wxPG_LABEL, pass->getPointMinSize()));
	SetPropertyHelpString(mPointMinSizeId, wxT("Minimum point size, when point attenuation is in use"));
	mPointMaxSizeId = AppendIn(mAttenuationId, new wxFloatProperty(wxT("Max"), wxPG_LABEL, pass->getPointMaxSize()));
	SetPropertyHelpString(mAttenuationId, wxT("Maximum point size, when point attenuation is in use"));
	mPointAttenuationConstantId = AppendIn(mAttenuationId, new wxFloatProperty(wxT("Constant"), wxPG_LABEL, pass->getPointAttenuationConstant()));
	SetPropertyHelpString(mPointAttenuationConstantId, wxT("Constant coefficient of the point attenuation"));
	mPointAttenuationLinearId = AppendIn(mAttenuationId, new wxFloatProperty(wxT("Linear"), wxPG_LABEL, pass->getPointAttenuationLinear()));
	SetPropertyHelpString(mPointAttenuationLinearId, wxT("Linear coefficient of the point attenuation"));
	mPointAttenuationQuadraticId = AppendIn(mAttenuationId, new wxFloatProperty(wxT("Quadratic"), wxPG_LABEL, pass->getPointAttenuationQuadratic()));
	SetPropertyHelpString(mPointAttenuationQuadraticId, wxT("Quadratic coefficient of the point attenuation"));
}


void PassPropertyGridPage::createSceneBlendingCategory()
{
	const Pass* pass = mController->getPass();

	Append(new wxPropertyCategory(wxT("Scene Blending")));

	wxPGChoices sbtChoices;
	sbtChoices.Add(wxT("N/A") -1);
	sbtChoices.Add(wxT("Transparent Alpha"), SBT_TRANSPARENT_ALPHA);
	sbtChoices.Add(wxT("Transparent Colour"), SBT_TRANSPARENT_COLOUR);
	sbtChoices.Add(wxT("Add"), SBT_ADD);
	sbtChoices.Add(wxT("Modulate"), SBT_MODULATE);
	sbtChoices.Add(wxT("Replace"), SBT_REPLACE);

	wxPGChoices sbfChoices;
	sbfChoices.Add(wxT("One"), SBF_ONE);
	sbfChoices.Add(wxT("Zero"), SBF_ZERO);
	sbfChoices.Add(wxT("Dest Colour"), SBF_DEST_COLOUR);
	sbfChoices.Add(wxT("Src Colour"), SBF_SOURCE_COLOUR);
	sbfChoices.Add(wxT("One Minus Dest Colour"), SBF_ONE_MINUS_DEST_COLOUR);
	sbfChoices.Add(wxT("One Minus Src Colour"), SBF_ONE_MINUS_SOURCE_COLOUR);
	sbfChoices.Add(wxT("Dest Alpha"), SBF_DEST_ALPHA);
	sbfChoices.Add(wxT("Source Alpha"), SBF_SOURCE_ALPHA);
	sbfChoices.Add(wxT("One Minus Dest Alpha"), SBF_ONE_MINUS_DEST_ALPHA);
	sbfChoices.Add(wxT("One Minus Source Alpha"), SBF_ONE_MINUS_SOURCE_ALPHA);

	// Scene Blend Type
	bool type = true;
	SceneBlendType blendType;
	SceneBlendFactor srcFactor = pass->getSourceBlendFactor();
	SceneBlendFactor destFactor = pass->getDestBlendFactor();
	if(srcFactor == SBF_SOURCE_ALPHA && destFactor == SBF_ONE_MINUS_SOURCE_ALPHA)
		blendType = SBT_TRANSPARENT_ALPHA;
	else if(srcFactor == SBF_SOURCE_COLOUR && destFactor == SBF_ONE_MINUS_SOURCE_COLOUR)
		blendType = SBT_TRANSPARENT_COLOUR;
	else if(srcFactor == SBF_DEST_COLOUR && destFactor == SBF_ZERO)
		blendType = SBT_MODULATE;
	else if(srcFactor == SBF_ONE && destFactor == SBF_ONE)
		blendType = SBT_ADD;
	else if(srcFactor == SBF_ONE && destFactor == SBF_ZERO)
		blendType= SBT_REPLACE;
	else type = false;

	mSceneBlendTypeId = Append(new wxEnumProperty(wxT("Type"), wxPG_LABEL, sbtChoices, (type) ? blendType : 0));

	// Source Scene Blend Type
	mSrcSceneBlendTypeId = Append(new wxEnumProperty(wxT("Src Factor"), wxPG_LABEL, sbfChoices, srcFactor));

	// Destination Scene Blend Type
	mDestSceneBlendTypeId = Append(new wxEnumProperty(wxT("Dest Factor"), wxPG_LABEL, sbfChoices, destFactor));
}


void PassPropertyGridPage::createDepthCategory()
{
	const Pass* pass = mController->getPass();

	Append(new wxPropertyCategory(wxT("Depth")));

	// Depth Check
	mDepthCheckId = Append(new wxBoolProperty(wxT("Depth Check"), wxPG_LABEL, pass->getDepthCheckEnabled()));

	// Depth Write
	mDepthWriteId = Append(new wxBoolProperty(wxT("Depth Write"), wxPG_LABEL, pass->getDepthWriteEnabled()));

	//  Depth Function
	wxPGChoices compareFuncChoices;
	compareFuncChoices.Add(wxT("Fail"), CMPF_ALWAYS_FAIL);
	compareFuncChoices.Add(wxT("Pass"), CMPF_ALWAYS_PASS);
	compareFuncChoices.Add(wxT("<"), CMPF_LESS);
	compareFuncChoices.Add(wxT("<="), CMPF_LESS_EQUAL);
	compareFuncChoices.Add(wxT("=="), CMPF_EQUAL);
	compareFuncChoices.Add(wxT("!="), CMPF_NOT_EQUAL);
	compareFuncChoices.Add(wxT(">="), CMPF_GREATER_EQUAL);
	compareFuncChoices.Add(wxT(">"), CMPF_GREATER);

	mDepthFunctionId = Append(new wxEnumProperty(wxT("Depth Function"), wxPG_LABEL, compareFuncChoices, pass->getDepthFunction()));

	mDepthBiasId = Append(new wxParentProperty(wxT("Depth Bias"), wxPG_LABEL));

	// Constant Bias
	mDepthBiasConstantId = AppendIn(mDepthBiasId, new wxFloatProperty(wxT("Constant"), wxPG_LABEL, pass->getDepthBiasConstant()));

	// Slope Bias
	mDepthBiasSlopeId = AppendIn(mDepthBiasId, new wxFloatProperty(wxT("Slope Scale"), wxPG_LABEL, pass->getDepthBiasSlopeScale()));
}

void PassPropertyGridPage::createCullingCategory()
{
	const Pass* pass = mController->getPass();

	Append(new wxPropertyCategory(wxT("Culling")));

	// Culling Mode
	wxPGChoices cullingModeChoices;
	cullingModeChoices.Add(wxT("None"), CULL_NONE);
	cullingModeChoices.Add(wxT("Clockwise"), CULL_CLOCKWISE);
	cullingModeChoices.Add(wxT("Counterclockwise"), CULL_ANTICLOCKWISE);

	Append(new wxEnumProperty(wxT("Culling Mode"), wxPG_LABEL, cullingModeChoices, pass->getDepthFunction()));

	// Manual Culling Mode
	wxPGChoices manualCullingModeChoices;
	manualCullingModeChoices.Add(wxT("None"), MANUAL_CULL_NONE);
	manualCullingModeChoices.Add(wxT("Back"), MANUAL_CULL_BACK);
	manualCullingModeChoices.Add(wxT("Front"), MANUAL_CULL_FRONT);

	mManualCullingModeId = Append(new wxEnumProperty(wxT("Manual Culling Mode"), wxPG_LABEL, manualCullingModeChoices, pass->getManualCullingMode()));
}

void PassPropertyGridPage::createIlluminationCategory()
{
	const Pass* pass = mController->getPass();

	Append(new wxPropertyCategory(wxT("Illumination")));

	mLightingId = Append(new wxBoolProperty(wxT("Lighting"), wxPG_LABEL, pass->getLightingEnabled()));

	// Max Simultaneous Lights
	mMaxLightsId = Append(new wxIntProperty(wxT("Max Lights"), wxPG_LABEL, pass->getMaxSimultaneousLights()));

	// Start Light
	mStartLightId = Append(new wxIntProperty(wxT("Start Light"), wxPG_LABEL, pass->getStartLight()));

	// Light Iteration
	mIterationId = Append(new wxBoolProperty(wxT("Iteration"), wxPG_LABEL, pass->getIteratePerLight()));

	// Shading Mode
	wxPGChoices shadingModeChoices;
	shadingModeChoices.Add(wxT("Flat"), SO_FLAT);
	shadingModeChoices.Add(wxT("Gouraud"), SO_GOURAUD);
	shadingModeChoices.Add(wxT("Phong"), SO_PHONG);

	mShadingModeId = Append(new wxEnumProperty(wxT("Shading Mode"), wxPG_LABEL, shadingModeChoices, pass->getShadingMode()));

	// Self Illumination
	ColourValue selfIllum = pass->getSelfIllumination();
	mSelfIlluminationId = Append(new wxColourProperty(wxT("Self Illumination"), wxPG_LABEL, wxColour((int)(255 * selfIllum.r), (int)(255 * selfIllum.g), (int)(255 * selfIllum.b))));

}

void PassPropertyGridPage::createFogCategory()
{
	const Pass* pass = mController->getPass();

	mOverrideSceneId = Append(new wxPropertyCategory(wxT("Fog")));

	// Fog Enabled
	mFogOverrideId = Append(new wxBoolProperty(wxT("Override Scene"), wxPG_LABEL, pass->getFogOverride()));

	// Fog Mode
	wxPGChoices fogModeChoices;
	fogModeChoices.Add(wxT("None"), FOG_NONE);
	fogModeChoices.Add(wxT("EXP"), FOG_EXP);
	fogModeChoices.Add(wxT("EXP2"), FOG_EXP2);
	fogModeChoices.Add(wxT("Linear"), FOG_LINEAR);

	mFogModeId = Append(new wxEnumProperty(wxT("Fog Mode"), wxPG_LABEL, fogModeChoices, pass->getFogMode()));
}

// Possibly better as a wxParentProperty within Misc?
void PassPropertyGridPage::createAlphaRejectionCategory()
{
	const Pass* pass = mController->getPass();
	Append(new wxPropertyCategory(wxT("Alpha Rejection"), wxPG_LABEL));

	// Alpha Reject Func
	wxPGChoices compareFuncChoices;
	compareFuncChoices.Add(wxT("Fail"), CMPF_ALWAYS_FAIL);
	compareFuncChoices.Add(wxT("Pass"), CMPF_ALWAYS_PASS);
	compareFuncChoices.Add(wxT("<"), CMPF_LESS);
	compareFuncChoices.Add(wxT("<="), CMPF_LESS_EQUAL);
	compareFuncChoices.Add(wxT("=="), CMPF_EQUAL);
	compareFuncChoices.Add(wxT("!="), CMPF_NOT_EQUAL);
	compareFuncChoices.Add(wxT(">="), CMPF_GREATER_EQUAL);
	compareFuncChoices.Add(wxT(">"), CMPF_GREATER);
	mAlphaRejectFuncId = Append(new wxEnumProperty(wxT("Function"), wxPG_LABEL, compareFuncChoices, pass->getAlphaRejectFunction()));

	// Alpha Reject Value
	mAlphaRejectValueId = Append(new wxIntProperty(wxT("Value"), wxPG_LABEL, pass->getAlphaRejectValue()));
}

void PassPropertyGridPage::createMiscCategory()
{
	const Pass* pass = mController->getPass();

	Append(new wxPropertyCategory(wxT("Misc")));

	// Colour Write
	mColourWriteId = Append(new wxBoolProperty(wxT("Colour Write"), wxPG_LABEL, pass->getColourWriteEnabled()));

	// Polygon Mode
	wxPGChoices polygonModeChoices;
	polygonModeChoices.Add(wxT("Points"), PM_POINTS);
	polygonModeChoices.Add(wxT("Wireframe"), PM_WIREFRAME);
	polygonModeChoices.Add(wxT("Solid"), PM_SOLID);

	mPolygonModeId = Append(new wxEnumProperty(wxT("Polygon Mode"), wxPG_LABEL, polygonModeChoices, pass->getPolygonMode()));

	// Track Vertex Colour Type
	wxPGChoices vertexColourTypeChoices;
	vertexColourTypeChoices.Add(wxT("None"), TVC_NONE);
	vertexColourTypeChoices.Add(wxT("Ambient"), TVC_AMBIENT);
	vertexColourTypeChoices.Add(wxT("Diffuse"), TVC_DIFFUSE);
	vertexColourTypeChoices.Add(wxT("Specular"), TVC_SPECULAR);
	vertexColourTypeChoices.Add(wxT("Emissive"), TVC_EMISSIVE);

	mTrackVertexColourTypeId = Append(new wxEnumProperty(wxT("Track Vertex Colour Type"), wxPG_LABEL, vertexColourTypeChoices, pass->getVertexColourTracking()));
}

void PassPropertyGridPage::propertyChanged(wxPropertyGridEvent& event)
{
	wxPGId id = event.GetProperty();
	if(id == mNameId)
	{
		mController->getPass()->setName(Ogre::String(event.GetPropertyValueAsString().mb_str()));
	}
	else if(id == mAmbientId)
	{
		// TODO
	}
	else if(id == mDiffuseId)
	{
		// TODO
	}
	else if(id == mSpecularId)
	{
		// TODO
	}
	else if(id == mShininessId)
	{
		mController->getPass()->setShininess((Real)event.GetPropertyValueAsDouble());
	}
	else if(id == mPointSizeId)
	{
		mController->getPass()->setPointSize((Real)event.GetPropertyValueAsDouble());
	}
	else if(id == mPointSpritesId)
	{
		mController->getPass()->setPointSpritesEnabled(event.GetPropertyValueAsBool());
	}
	//else if(id == mAttenuationId)
	//{
	//}
	else if(id == mPointAttenuationId)
	{
		const Pass* pass = mController->getPass();
		mController->getPass()->setPointAttenuation(event.GetPropertyValueAsBool(),
			pass->getPointAttenuationConstant(), pass->getPointAttenuationLinear(),
			pass->getPointAttenuationQuadratic());
	}
	else if(id == mPointMinSizeId)
	{
		mController->getPass()->setPointMinSize((Real)event.GetPropertyValueAsDouble());
	}
	else if(id == mPointMaxSizeId)
	{
		mController->getPass()->setPointMaxSize((Real)event.GetPropertyValueAsDouble());
	}
	else if(id == mPointAttenuationConstantId)
	{
		const Pass* pass = mController->getPass();
		mController->getPass()->setPointAttenuation(pass->isPointAttenuationEnabled(),
			(Real)event.GetPropertyValueAsDouble(), pass->getPointAttenuationLinear(),
			pass->getPointAttenuationQuadratic());
	}
	else if(id == mPointAttenuationLinearId)
	{
		const Pass* pass = mController->getPass();
		mController->getPass()->setPointAttenuation(pass->isPointAttenuationEnabled(),
			pass->getPointAttenuationConstant(), (Real)event.GetPropertyValueAsDouble(),
			pass->getPointAttenuationQuadratic());
	}
	else if(id == mPointAttenuationQuadraticId)
	{
		const Pass* pass = mController->getPass();
		mController->getPass()->setPointAttenuation(pass->isPointAttenuationEnabled(),
			pass->getPointAttenuationConstant(), pass->getPointAttenuationLinear(),
			(Real)event.GetPropertyValueAsDouble());
	}
	else if(id == mSceneBlendTypeId)
	{
		mController->getPass()->setSceneBlending((SceneBlendType)event.GetPropertyValueAsInt());
	}
	else if(id == mSrcSceneBlendTypeId)
	{
		mController->getPass()->setSceneBlending((SceneBlendFactor)event.GetPropertyValueAsInt(),
			mController->getPass()->getDestBlendFactor());
	}
	else if(id == mDestSceneBlendTypeId)
	{
		mController->getPass()->setSceneBlending(mController->getPass()->getSourceBlendFactor(),
			(SceneBlendFactor)event.GetPropertyValueAsInt());
	}
	else if(id == mDepthCheckId)
	{
		mController->getPass()->setDepthCheckEnabled(event.GetPropertyValueAsBool());
	}
	else if(id == mDepthWriteId)
	{
		mController->getPass()->setDepthWriteEnabled(event.GetPropertyValueAsBool());
	}
	else if(id == mDepthFunctionId)
	{
		mController->getPass()->setDepthFunction((CompareFunction)event.GetPropertyValueAsInt());
	}
	//else if(id == mDepthBiasId)
	//{
	//}
	else if(id == mDepthBiasConstantId)
	{
		mController->getPass()->setDepthBias(event.GetPropertyValueAsDouble(), mController->getPass()->getDepthBiasSlopeScale());
	}
	else if(id == mDepthBiasSlopeId)
	{
		mController->getPass()->setDepthBias(mController->getPass()->getDepthBiasConstant(), event.GetPropertyValueAsDouble());
	}
	else if(id == mManualCullingModeId)
	{
		mController->getPass()->setManualCullingMode((ManualCullingMode)event.GetPropertyValueAsInt());
	}
	else if(id == mLightingId)
	{
		mController->getPass()->setLightingEnabled(event.GetPropertyValueAsBool());
	}
	else if(id == mMaxLightsId)
	{
		mController->getPass()->setMaxSimultaneousLights(event.GetPropertyValueAsInt());
	}
	else if(id == mStartLightId)
	{
		mController->getPass()->setStartLight(event.GetPropertyValueAsInt());
	}
	else if(id == mIterationId)
	{
		mController->getPass()->setLightCountPerIteration(event.GetPropertyValueAsInt());
	}
	else if(id == mShadingModeId)
	{
		mController->getPass()->setShadingMode((ShadeOptions)event.GetPropertyValueAsInt());
	}
	else if(id == mSelfIlluminationId)
	{
		// TODO
	}
	else if(id == mOverrideSceneId)
	{
		const Pass* pass = mController->getPass();
		mController->getPass()->setFog(event.GetPropertyValueAsBool(), pass->getFogMode(), pass->getFogColour(),
			pass->getFogDensity(), pass->getFogStart(), pass->getFogEnd());
	}
	else if(id == mFogOverrideId)
	{
		const Pass* pass = mController->getPass();
		mController->getPass()->setFog(event.GetPropertyValueAsBool(), pass->getFogMode(), pass->getFogColour(),
			pass->getFogDensity(), pass->getFogStart(), pass->getFogEnd());
	}
	else if(id == mFogModeId)
	{
		const Pass* pass = mController->getPass();
		mController->getPass()->setFog(pass->getFogOverride(), (FogMode)event.GetPropertyValueAsInt(), pass->getFogColour(),
			pass->getFogDensity(), pass->getFogStart(), pass->getFogEnd());
	}
	else if(id == mAlphaRejectFuncId)
	{
		mController->getPass()->setAlphaRejectFunction((CompareFunction)event.GetPropertyValueAsInt());
	}
	else if(id == mAlphaRejectValueId)
	{
		mController->getPass()->setAlphaRejectValue(event.GetPropertyValueAsInt());
	}
	else if(id == mColourWriteId)
	{
		mController->getPass()->setColourWriteEnabled(event.GetPropertyValueAsBool());
	}
	else if(id == mPolygonModeId)
	{
		mController->getPass()->setPolygonMode((PolygonMode)event.GetPropertyValueAsInt());
	}
	else if(id == mTrackVertexColourTypeId)
	{
		mController->getPass()->setVertexColourTracking((TrackVertexColourType)event.GetPropertyValueAsInt());
	}
}
















