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
#include "../ImageListWidget.h"
#include "ImageListWidgetDesignerPlugin.h"

#include <QtPlugin>

using namespace Qt::StringLiterals;

CImageListWidgetDesignerPlugin::CImageListWidgetDesignerPlugin( QObject *parent ) :
    QObject( parent )
{
}

void CImageListWidgetDesignerPlugin::initialize( QDesignerFormEditorInterface * /* core */ )
{
    if ( fInitialized )
        return;

    fInitialized = true;
}

bool CImageListWidgetDesignerPlugin::isInitialized() const
{
    return fInitialized;
}

QWidget *CImageListWidgetDesignerPlugin::createWidget( QWidget *parent )
{
    return new NTowel42Utils::CImageListWidget( parent );
}

QString CImageListWidgetDesignerPlugin::name() const
{
    return u"NTowel42Utils::CImageListWidget"_s;
}

QString CImageListWidgetDesignerPlugin::group() const
{
    return u"Towel 42 Utilities"_s;
}

QIcon CImageListWidgetDesignerPlugin::icon() const
{
    return {};
}

QString CImageListWidgetDesignerPlugin::toolTip() const
{
    return u"NTowel42Utils::CImageListWidget"_s;
}

QString CImageListWidgetDesignerPlugin::whatsThis() const
{
    return u"NTowel42Utils::CImageListWidget"_s;
}

bool CImageListWidgetDesignerPlugin::isContainer() const
{
    return false;
}

QString CImageListWidgetDesignerPlugin::domXml() const
{
    return uR"(
<ui language="c++">
  <widget class="NTowel42Utils::CImageListWidget" name="imageListWidget">
    <property name="geometry">
      <rect>
        <x>0</x>
        <y>0</y>
        <width>200</width>
        <height>250</height>
      </rect>
    </property>
  </widget>
</ui>
)"_s;
}


QString CImageListWidgetDesignerPlugin::includeFile() const
{
    return u"T42-Utils/ImageListWidget.h"_s;
}
