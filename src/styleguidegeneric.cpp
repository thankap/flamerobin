/*
  The contents of this file are subject to the Initial Developer's Public
  License Version 1.0 (the "License"); you may not use this file except in
  compliance with the License. You may obtain a copy of the License here:
  http://www.flamerobin.org/license.html.

  Software distributed under the License is distributed on an "AS IS"
  basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
  License for the specific language governing rights and limitations under
  the License.

  The Original Code is FlameRobin (TM).

  The Initial Developer of the Original Code is Michael Hieke.

  Portions created by the original developer
  are Copyright (C) 2004 Michael Hieke.

  All Rights Reserved.

  $Id$

  Contributor(s):
*/

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#if defined(__WXMSW__) || defined(__WXMAC__)
#error Must not be included in project for Windows or OS X!!!
#endif

#include "styleguide.h"
//------------------------------------------------------------------------------
class YStyleGuideGeneric: public YxStyleGuide
{
public:
    YStyleGuideGeneric();
    virtual wxSizer* createButtonSizer(wxButton* button_ok, wxButton* button_cancel);
    virtual int getBetweenButtonsMargin(wxOrientation orientation);
    virtual int getBrowseButtonMargin();
    virtual int getCheckboxSpacing();
    virtual int getControlLabelMargin();
    virtual int getDialogMargin(wxDirection direction);
    virtual int getFrameMargin(wxDirection direction);
    virtual int getRelatedControlMargin(wxOrientation orientation);
    virtual int getUnrelatedControlMargin(wxOrientation orientation);
};
//------------------------------------------------------------------------------
YStyleGuideGeneric::YStyleGuideGeneric()
{
}
//------------------------------------------------------------------------------
wxSizer* YStyleGuideGeneric::createButtonSizer(wxButton* button_ok, wxButton* button_cancel)
{
    wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    // center-align
    sizer->Add(0, 0, 1, wxEXPAND);
    if (button_ok != 0)
        sizer->Add(button_ok);
    if (button_ok != 0 && button_cancel != 0)
        sizer->Add(getBetweenButtonsMargin(wxHORIZONTAL), 0);
    if (button_cancel != 0)
        sizer->Add(button_cancel);
    sizer->Add(0, 0, 1, wxEXPAND);
    return sizer;
}
//------------------------------------------------------------------------------
int YStyleGuideGeneric::getBetweenButtonsMargin(wxOrientation WXUNUSED(orientation))
{
    return 12;
}
//------------------------------------------------------------------------------
int YStyleGuideGeneric::getBrowseButtonMargin()
{
    return 5;
}
//------------------------------------------------------------------------------
int YStyleGuideGeneric::getCheckboxSpacing()
{
    return 4;
}
//------------------------------------------------------------------------------
int YStyleGuideGeneric::getControlLabelMargin()
{
    return 5;
}
//------------------------------------------------------------------------------
int YStyleGuideGeneric::getDialogMargin(wxDirection WXUNUSED(direction))
{
    return 20;
}
//------------------------------------------------------------------------------
int YStyleGuideGeneric::getFrameMargin(wxDirection direction)
{
    return 10;
}
//------------------------------------------------------------------------------
int YStyleGuideGeneric::getRelatedControlMargin(wxOrientation WXUNUSED(orientation))
{
    return 10;
}
//------------------------------------------------------------------------------
int YStyleGuideGeneric::getUnrelatedControlMargin(wxOrientation WXUNUSED(orientation))
{
    return 15;
}
//------------------------------------------------------------------------------
YxStyleGuide& styleguide()
{
    static YStyleGuideGeneric guide;
    return guide;
}
//------------------------------------------------------------------------------
