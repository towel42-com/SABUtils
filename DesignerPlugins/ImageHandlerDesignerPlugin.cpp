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
#include "../ImageHandler.h"
#include "ImageHandlerDesignerPlugin.h"

#include <QtPlugin>

using namespace Qt::StringLiterals;

CImageHandlerDesignerPlugin::CImageHandlerDesignerPlugin( QObject *parent ) :
    QObject( parent )
{
}

void CImageHandlerDesignerPlugin::initialize( QDesignerFormEditorInterface * /* core */ )
{
    if ( fInitialized )
        return;

    fInitialized = true;
}

bool CImageHandlerDesignerPlugin::isInitialized() const
{
    return fInitialized;
}

QWidget *CImageHandlerDesignerPlugin::createWidget( QWidget *parent )
{
    return new NTowel42Utils::CImageHandler( parent );
}

QString CImageHandlerDesignerPlugin::name() const
{
    return u"NTowel42Utils::CImageHandler"_s;
}

QString CImageHandlerDesignerPlugin::group() const
{
    return u"Towel 42 Utilities"_s;
}

QIcon CImageHandlerDesignerPlugin::icon() const
{
    return {};
}

QString CImageHandlerDesignerPlugin::toolTip() const
{
    return u"NTowel42Utils::CImageHandler"_s;
}

QString CImageHandlerDesignerPlugin::whatsThis() const
{
    return u"NTowel42Utils::CImageHandler"_s;
}

bool CImageHandlerDesignerPlugin::isContainer() const
{
    return false;
}

QString CImageHandlerDesignerPlugin::domXml() const
{
    return uR"(
<ui language="c++">
  <widget class="NTowel42Utils::CImageHandler" name="imageHandler">
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
      <string>Image Handler</string>
    </property>
    <property name="whatsThis">
      <string>The widget allows the user to select an image or avatar.</string>
    </property>
  </widget>
</ui>
)"_s;
}

QString CImageHandlerDesignerPlugin::includeFile() const
{
    return u"T42-Utils/ImageHandler.h"_s;
}
