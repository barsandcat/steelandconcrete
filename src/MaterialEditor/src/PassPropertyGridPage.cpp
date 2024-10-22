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


using namespace Ogre;

BEGIN_EVENT_TABLE(PassPropertyGridPage, wxPropertyGridPage)
	EVT_PG_CHANGED(-1, PassPropertyGridPage::propertyChanged)
END_EVENT_TABLE()

PassPropertyGridPage::PassPropertyGridPage(Ogre::Pass* controller)
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
	Append(new wxPropertyCategory(wxT("General")));

	// Name
	mNameId = Append(new wxStringProperty(wxT("Name"), wxPG_LABEL, wxString(mController->getName().c_str(), wxConvUTF8)));
	SetPropertyHelpString(mNameId, wxT("Name of this Pass"));
}

void PassPropertyGridPage::createReflectanceCategory()
{
	Append(new wxPropertyCategory(wxT("Reflectance")));

	// Ambient
	ColourValue ambient = mController->getAmbient();
	mAmbientId = Append(new wxColourProperty(wxT("Ambient"), wxPG_LABEL, wxColour((int)(255 * ambient.r), (int)(255 * ambient.g), (int)(255 * ambient.b))));
	SetPropertyHelpString(mAmbientId, wxT("Ambient colour reflectance"));

	// Diffuse
	ColourValue diffuse = mController->getDiffuse();
	mDiffuseId = Append(new wxColourProperty(wxT("Diffuse"), wxPG_LABEL, wxColour((int)(255 * diffuse.r), (int)(255 * diffuse.g), (int)(255 * diffuse.b))));
	SetPropertyHelpString(mDiffuseId, wxT("Diffuse colour reflectance"));

	// Specular
	ColourValue specular = mController->getSpecular();
	mSpecularId = Append(new wxColourProperty(wxT("Specular"), wxPG_LABEL, wxColour((int)(255 * specular.r), (int)(255 * specular.g), (int)(255 * specular.b))));
	SetPropertyHelpString(mSpecularId, wxT("Specular colour reflectance"));

	// Shininess
	mShininessId = Append(new wxFloatProperty(wxT("Shininess"), wxPG_LABEL, mController->getShininess()));
	SetPropertyHelpString(mShininessId, wxT("Shininess, affecting the size of specular highlights"));
}


void PassPropertyGridPage::createPointCategory()
{
	Append(new wxPropertyCategory(wxT("Point"), wxPG_LABEL));

	// Point Size
	wxPGId pointSize = Append(new wxParentProperty(wxT("Size"), wxPG_LABEL));
	mPointSizeId = AppendIn(pointSize, new wxFloatProperty(wxT("Size"), wxPG_LABEL, mController->getPointSize()));
	SetPropertyHelpString(mPointSizeId, wxT("Point size, affecting the size of points when rendering a point list, or a list of point sprites"));

	// Point Sprites
	mPointSpritesId = Append(new wxBoolProperty(wxT("Point Sprites"), wxPG_LABEL, mController->getPointSpritesEnabled()));

	// Point Attenuation
	mAttenuationId = Append(new wxParentProperty(wxT("Attenuation"),wxPG_LABEL));
	SetPropertyHelpString(mAttenuationId, wxT("Determines how points are attenuated with distance"));
	mPointAttenuationId = AppendIn(mAttenuationId, new wxBoolProperty(wxT("Enabled"), wxPG_LABEL, mController->isPointAttenuationEnabled()));
	mPointMinSizeId = AppendIn(mAttenuationId, new wxFloatProperty(wxT("Min"), wxPG_LABEL, mController->getPointMinSize()));
	SetPropertyHelpString(mPointMinSizeId, wxT("Minimum point size, when point attenuation is in use"));
	mPointMaxSizeId = AppendIn(mAttenuationId, new wxFloatProperty(wxT("Max"), wxPG_LABEL, mController->getPointMaxSize()));
	SetPropertyHelpString(mAttenuationId, wxT("Maximum point size, when point attenuation is in use"));
	mPointAttenuationConstantId = AppendIn(mAttenuationId, new wxFloatProperty(wxT("Constant"), wxPG_LABEL, mController->getPointAttenuationConstant()));
	SetPropertyHelpString(mPointAttenuationConstantId, wxT("Constant coefficient of the point attenuation"));
	mPointAttenuationLinearId = AppendIn(mAttenuationId, new wxFloatProperty(wxT("Linear"), wxPG_LABEL, mController->getPointAttenuationLinear()));
	SetPropertyHelpString(mPointAttenuationLinearId, wxT("Linear coefficient of the point attenuation"));
	mPointAttenuationQuadraticId = AppendIn(mAttenuationId, new wxFloatProperty(wxT("Quadratic"), wxPG_LABEL, mController->getPointAttenuationQuadratic()));
	SetPropertyHelpString(mPointAttenuationQuadraticId, wxT("Quadratic coefficient of the point attenuation"));
}


void PassPropertyGridPage::createSceneBlendingCategory()
{
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
	SceneBlendFactor srcFactor = mController->getSourceBlendFactor();
	SceneBlendFactor destFactor = mController->getDestBlendFactor();
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
	Append(new wxPropertyCategory(wxT("Depth")));

	// Depth Check
	mDepthCheckId = Append(new wxBoolProperty(wxT("Depth Check"), wxPG_LABEL, mController->getDepthCheckEnabled()));

	// Depth Write
	mDepthWriteId = Append(new wxBoolProperty(wxT("Depth Write"), wxPG_LABEL, mController->getDepthWriteEnabled()));

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

	mDepthFunctionId = Append(new wxEnumProperty(wxT("Depth Function"), wxPG_LABEL, compareFuncChoices, mController->getDepthFunction()));

	mDepthBiasId = Append(new wxParentProperty(wxT("Depth Bias"), wxPG_LABEL));

	// Constant Bias
	mDepthBiasConstantId = AppendIn(mDepthBiasId, new wxFloatProperty(wxT("Constant"), wxPG_LABEL, mController->getDepthBiasConstant()));

	// Slope Bias
	mDepthBiasSlopeId = AppendIn(mDepthBiasId, new wxFloatProperty(wxT("Slope Scale"), wxPG_LABEL, mController->getDepthBiasSlopeScale()));
}

void PassPropertyGridPage::createCullingCategory()
{
	Append(new wxPropertyCategory(wxT("Culling")));

	// Culling Mode
	wxPGChoices cullingModeChoices;
	cullingModeChoices.Add(wxT("None"), CULL_NONE);
	cullingModeChoices.Add(wxT("Clockwise"), CULL_CLOCKWISE);
	cullingModeChoices.Add(wxT("Counterclockwise"), CULL_ANTICLOCKWISE);

	Append(new wxEnumProperty(wxT("Culling Mode"), wxPG_LABEL, cullingModeChoices, mController->getDepthFunction()));

	// Manual Culling Mode
	wxPGChoices manualCullingModeChoices;
	manualCullingModeChoices.Add(wxT("None"), MANUAL_CULL_NONE);
	manualCullingModeChoices.Add(wxT("Back"), MANUAL_CULL_BACK);
	manualCullingModeChoices.Add(wxT("Front"), MANUAL_CULL_FRONT);

	mManualCullingModeId = Append(new wxEnumProperty(wxT("Manual Culling Mode"), wxPG_LABEL, manualCullingModeChoices, mController->getManualCullingMode()));
}

void PassPropertyGridPage::createIlluminationCategory()
{
	Append(new wxPropertyCategory(wxT("Illumination")));

	mLightingId = Append(new wxBoolProperty(wxT("Lighting"), wxPG_LABEL, mController->getLightingEnabled()));

	// Max Simultaneous Lights
	mMaxLightsId = Append(new wxIntProperty(wxT("Max Lights"), wxPG_LABEL, mController->getMaxSimultaneousLights()));

	// Start Light
	mStartLightId = Append(new wxIntProperty(wxT("Start Light"), wxPG_LABEL, mController->getStartLight()));

	// Light Iteration
	mIterationId = Append(new wxBoolProperty(wxT("Iteration"), wxPG_LABEL, mController->getIteratePerLight()));

	// Shading Mode
	wxPGChoices shadingModeChoices;
	shadingModeChoices.Add(wxT("Flat"), SO_FLAT);
	shadingModeChoices.Add(wxT("Gouraud"), SO_GOURAUD);
	shadingModeChoices.Add(wxT("Phong"), SO_PHONG);

	mShadingModeId = Append(new wxEnumProperty(wxT("Shading Mode"), wxPG_LABEL, shadingModeChoices, mController->getShadingMode()));

	// Self Illumination
	ColourValue selfIllum = mController->getSelfIllumination();
	mSelfIlluminationId = Append(new wxColourProperty(wxT("Self Illumination"), wxPG_LABEL, wxColour((int)(255 * selfIllum.r), (int)(255 * selfIllum.g), (int)(255 * selfIllum.b))));

}

void PassPropertyGridPage::createFogCategory()
{
	mOverrideSceneId = Append(new wxPropertyCategory(wxT("Fog")));

	// Fog Enabled
	mFogOverrideId = Append(new wxBoolProperty(wxT("Override Scene"), wxPG_LABEL, mController->getFogOverride()));

	// Fog Mode
	wxPGChoices fogModeChoices;
	fogModeChoices.Add(wxT("None"), FOG_NONE);
	fogModeChoices.Add(wxT("EXP"), FOG_EXP);
	fogModeChoices.Add(wxT("EXP2"), FOG_EXP2);
	fogModeChoices.Add(wxT("Linear"), FOG_LINEAR);

	mFogModeId = Append(new wxEnumProperty(wxT("Fog Mode"), wxPG_LABEL, fogModeChoices, mController->getFogMode()));
}

// Possibly better as a wxParentProperty within Misc?
void PassPropertyGridPage::createAlphaRejectionCategory()
{
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
	mAlphaRejectFuncId = Append(new wxEnumProperty(wxT("Function"), wxPG_LABEL, compareFuncChoices, mController->getAlphaRejectFunction()));

	// Alpha Reject Value
	mAlphaRejectValueId = Append(new wxIntProperty(wxT("Value"), wxPG_LABEL, mController->getAlphaRejectValue()));
}

void PassPropertyGridPage::createMiscCategory()
{
	Append(new wxPropertyCategory(wxT("Misc")));

	// Colour Write
	mColourWriteId = Append(new wxBoolProperty(wxT("Colour Write"), wxPG_LABEL, mController->getColourWriteEnabled()));

	// Polygon Mode
	wxPGChoices polygonModeChoices;
	polygonModeChoices.Add(wxT("Points"), PM_POINTS);
	polygonModeChoices.Add(wxT("Wireframe"), PM_WIREFRAME);
	polygonModeChoices.Add(wxT("Solid"), PM_SOLID);

	mPolygonModeId = Append(new wxEnumProperty(wxT("Polygon Mode"), wxPG_LABEL, polygonModeChoices, mController->getPolygonMode()));

	// Track Vertex Colour Type
	wxPGChoices vertexColourTypeChoices;
	vertexColourTypeChoices.Add(wxT("None"), TVC_NONE);
	vertexColourTypeChoices.Add(wxT("Ambient"), TVC_AMBIENT);
	vertexColourTypeChoices.Add(wxT("Diffuse"), TVC_DIFFUSE);
	vertexColourTypeChoices.Add(wxT("Specular"), TVC_SPECULAR);
	vertexColourTypeChoices.Add(wxT("Emissive"), TVC_EMISSIVE);

	mTrackVertexColourTypeId = Append(new wxEnumProperty(wxT("Track Vertex Colour Type"), wxPG_LABEL, vertexColourTypeChoices, mController->getVertexColourTracking()));
}

void PassPropertyGridPage::propertyChanged(wxPropertyGridEvent& event)
{
	wxPGId id = event.GetProperty();
	if(id == mNameId)
	{
		mController->setName(Ogre::String(event.GetPropertyValueAsString().mb_str()));
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
		mController->setShininess((Real)event.GetPropertyValueAsDouble());
	}
	else if(id == mPointSizeId)
	{
		mController->setPointSize((Real)event.GetPropertyValueAsDouble());
	}
	else if(id == mPointSpritesId)
	{
		mController->setPointSpritesEnabled(event.GetPropertyValueAsBool());
	}
	//else if(id == mAttenuationId)
	//{
	//}
	else if(id == mPointAttenuationId)
	{
		mController->setPointAttenuation(event.GetPropertyValueAsBool(),
			mController->getPointAttenuationConstant(), mController->getPointAttenuationLinear(),
			mController->getPointAttenuationQuadratic());
	}
	else if(id == mPointMinSizeId)
	{
		mController->setPointMinSize((Real)event.GetPropertyValueAsDouble());
	}
	else if(id == mPointMaxSizeId)
	{
		mController->setPointMaxSize((Real)event.GetPropertyValueAsDouble());
	}
	else if(id == mPointAttenuationConstantId)
	{
		mController->setPointAttenuation(mController->isPointAttenuationEnabled(),
			(Real)event.GetPropertyValueAsDouble(), mController->getPointAttenuationLinear(),
			mController->getPointAttenuationQuadratic());
	}
	else if(id == mPointAttenuationLinearId)
	{
		mController->setPointAttenuation(mController->isPointAttenuationEnabled(),
			mController->getPointAttenuationConstant(), (Real)event.GetPropertyValueAsDouble(),
			mController->getPointAttenuationQuadratic());
	}
	else if(id == mPointAttenuationQuadraticId)
	{
		mController->setPointAttenuation(mController->isPointAttenuationEnabled(),
			mController->getPointAttenuationConstant(), mController->getPointAttenuationLinear(),
			(Real)event.GetPropertyValueAsDouble());
	}
	else if(id == mSceneBlendTypeId)
	{
		mController->setSceneBlending((SceneBlendType)event.GetPropertyValueAsInt());
	}
	else if(id == mSrcSceneBlendTypeId)
	{
		mController->setSceneBlending((SceneBlendFactor)event.GetPropertyValueAsInt(),
			mController->getDestBlendFactor());
	}
	else if(id == mDestSceneBlendTypeId)
	{
		mController->setSceneBlending(mController->getSourceBlendFactor(),
			(SceneBlendFactor)event.GetPropertyValueAsInt());
	}
	else if(id == mDepthCheckId)
	{
		mController->setDepthCheckEnabled(event.GetPropertyValueAsBool());
	}
	else if(id == mDepthWriteId)
	{
		mController->setDepthWriteEnabled(event.GetPropertyValueAsBool());
	}
	else if(id == mDepthFunctionId)
	{
		mController->setDepthFunction((CompareFunction)event.GetPropertyValueAsInt());
	}
	//else if(id == mDepthBiasId)
	//{
	//}
	else if(id == mDepthBiasConstantId)
	{
		mController->setDepthBias(event.GetPropertyValueAsDouble(), mController->getDepthBiasSlopeScale());
	}
	else if(id == mDepthBiasSlopeId)
	{
		mController->setDepthBias(mController->getDepthBiasConstant(), event.GetPropertyValueAsDouble());
	}
	else if(id == mManualCullingModeId)
	{
		mController->setManualCullingMode((ManualCullingMode)event.GetPropertyValueAsInt());
	}
	else if(id == mLightingId)
	{
		mController->setLightingEnabled(event.GetPropertyValueAsBool());
	}
	else if(id == mMaxLightsId)
	{
		mController->setMaxSimultaneousLights(event.GetPropertyValueAsInt());
	}
	else if(id == mStartLightId)
	{
		mController->setStartLight(event.GetPropertyValueAsInt());
	}
	else if(id == mIterationId)
	{
		mController->setLightCountPerIteration(event.GetPropertyValueAsInt());
	}
	else if(id == mShadingModeId)
	{
		mController->setShadingMode((ShadeOptions)event.GetPropertyValueAsInt());
	}
	else if(id == mSelfIlluminationId)
	{
		// TODO
	}
	else if(id == mOverrideSceneId)
	{
		mController->setFog(event.GetPropertyValueAsBool(), mController->getFogMode(), mController->getFogColour(),
			mController->getFogDensity(), mController->getFogStart(), mController->getFogEnd());
	}
	else if(id == mFogOverrideId)
	{
		mController->setFog(event.GetPropertyValueAsBool(), mController->getFogMode(), mController->getFogColour(),
			mController->getFogDensity(), mController->getFogStart(), mController->getFogEnd());
	}
	else if(id == mFogModeId)
	{
		mController->setFog(mController->getFogOverride(), (FogMode)event.GetPropertyValueAsInt(), mController->getFogColour(),
			mController->getFogDensity(), mController->getFogStart(), mController->getFogEnd());
	}
	else if(id == mAlphaRejectFuncId)
	{
		mController->setAlphaRejectFunction((CompareFunction)event.GetPropertyValueAsInt());
	}
	else if(id == mAlphaRejectValueId)
	{
		mController->setAlphaRejectValue(event.GetPropertyValueAsInt());
	}
	else if(id == mColourWriteId)
	{
		mController->setColourWriteEnabled(event.GetPropertyValueAsBool());
	}
	else if(id == mPolygonModeId)
	{
		mController->setPolygonMode((PolygonMode)event.GetPropertyValueAsInt());
	}
	else if(id == mTrackVertexColourTypeId)
	{
		mController->setVertexColourTracking((TrackVertexColourType)event.GetPropertyValueAsInt());
	}
}
















