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
#include "GLSLEditor.h"

BEGIN_EVENT_TABLE(GLSLEditor, ScintillaEditor)
	// Scintilla
	EVT_SCI_CHARADDED (-1,   GLSLEditor::OnCharAdded)
END_EVENT_TABLE()

GLSLEditor::GLSLEditor(wxWindow* parent, wxWindowID id /*= -1*/,
		const wxPoint& pos /*= wxDefaultPosition*/,
		const wxSize& size /*= wxDefaultSize*/,
		long style /*= wxVSCROLL*/
		) : ScintillaEditor(parent, id, pos, size, style)
{
	initialize();
}

GLSLEditor::~GLSLEditor()
{
}

void GLSLEditor::initialize()
{
	StyleClearAll();
	SetLexer(wxSCI_LEX_CPP);

	// Load keywords
	std::string path = "res/lexers/glsl/keywords";
	loadKeywords(path);

	// Load call tips
	path = "res/lexers/glsl/calltips";
	getCallTipManager().load(path);
	wxChar trigger('(');
	getCallTipManager().addTrigger(trigger);

	// Set styles
	StyleSetForeground(wxSCI_C_COMMENT, wxColour(0, 128, 0));
	StyleSetFontAttr(wxSCI_C_COMMENT, 10, wxT("Courier New"), false, false, false);
	StyleSetForeground(wxSCI_C_COMMENTLINE, wxColour(0, 128, 0));
	StyleSetFontAttr(wxSCI_C_COMMENTLINE, 10, wxT("Courier New"), false, false, false);
	StyleSetForeground(wxSCI_C_NUMBER, wxColour(0, 0, 128));
	StyleSetFontAttr(wxSCI_C_NUMBER, 10, wxT("Courier New"), false, false, false);
	StyleSetForeground(wxSCI_C_STRING, wxColour(200, 200, 200));
	StyleSetFontAttr(wxSCI_C_STRING, 10, wxT("Courier New"), false, false, false);
	StyleSetForeground(wxSCI_C_WORD, wxColour(0, 0, 255));
	StyleSetFontAttr(wxSCI_C_WORD, 10, wxT("Courier New"), false, false, false);
	StyleSetForeground(wxSCI_C_WORD2, wxColour(136, 0, 0));
	StyleSetFontAttr(wxSCI_C_WORD2, 10, wxT("Courier New"), false, false, false);
}

void GLSLEditor::OnCharAdded(wxScintillaEvent &event)
{
	char ch = event.GetKey();
	if(getCallTipManager().isTrigger(ch))
	{
		int lineNum = GetCurrentLine();
		if(lineNum != -1)
		{
			wxString line = GetLine(lineNum);
			int pos = GetCurrentPos() - 1;

			wxString word;
			wxChar ch;
			while(pos)
			{
				ch = GetCharAt(--pos);
				if(ch != ' ' || ch != '\n') word.Prepend(ch);
				else break;
			}

			wxString* tips = getCallTipManager().find(word);
			if(tips != NULL)
			{
				CallTipShow(pos, *tips);
			}
		}
	}
}
