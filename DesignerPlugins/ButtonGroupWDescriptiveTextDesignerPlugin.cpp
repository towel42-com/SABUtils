// The MIT License( MIT )
//
// Copyright( c ) 2026 Towel 42 Development, LLC and Scott Aron Bloom
// SPDX-License-Identifier : MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sub-license, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
#include "T42-Utils/ButtonGroupWDescriptiveText.h"
#include "ButtonGroupWDescriptiveTextDesignerPlugin.h"

#include <QtPlugin>

using namespace Qt::StringLiterals;

CButtonGroupWDescriptiveTextDesignerPlugin::CButtonGroupWDescriptiveTextDesignerPlugin( QObject *parent ) :
    QObject( parent )
{
}

void CButtonGroupWDescriptiveTextDesignerPlugin::initialize( QDesignerFormEditorInterface * /* core */ )
{
    if ( fInitialized )
        return;

    fInitialized = true;
}

bool CButtonGroupWDescriptiveTextDesignerPlugin::isInitialized() const
{
    return fInitialized;
}

QWidget *CButtonGroupWDescriptiveTextDesignerPlugin::createWidget( QWidget *parent )
{
    return new NTowel42Utils::CButtonGroupWDescriptiveText( parent );
}

QString CButtonGroupWDescriptiveTextDesignerPlugin::name() const
{
    return u"NTowel42Utils::CButtonGroupWDescriptiveText"_s;
}

QString CButtonGroupWDescriptiveTextDesignerPlugin::group() const
{
    return u"Towel 42 Utilities"_s;
}

QIcon CButtonGroupWDescriptiveTextDesignerPlugin::icon() const
{
    return {};
}

QString CButtonGroupWDescriptiveTextDesignerPlugin::toolTip() const
{
    return u"NTowel42Utils::CButtonGroupWDescriptiveText"_s;
}

QString CButtonGroupWDescriptiveTextDesignerPlugin::whatsThis() const
{
    return u"NTowel42Utils::CButtonGroupWDescriptiveText"_s;
}

bool CButtonGroupWDescriptiveTextDesignerPlugin::isContainer() const
{
    return false;
}

QString CButtonGroupWDescriptiveTextDesignerPlugin::domXml() const
{
    return uR"(
<ui language="c++">
  <widget class="NTowel42Utils::CButtonGroupWDescriptiveText" name="buttonGroupWDescriptiveText">
)"
           R"(
    <property name="geometry">
      <rect>
        <x>0</x>
        <y>0</y>
        <width>200</width>
        <height>250</height>
      </rect>
    </property>
")
R"(
    <property name="toolTip">
      <string>Button Group with Descriptive Text</string>
    </property>
    <property name="whatsThis">
      <string>The widget creates a button group that shows missing data as well as needs an explanation.</string>
    </property>
  </widget>
</ui>
)"_s;
}

QString CButtonGroupWDescriptiveTextDesignerPlugin::includeFile() const
{
    return u"T42-Utils/ButtonGroupWDescriptiveText.h"_s;
}
