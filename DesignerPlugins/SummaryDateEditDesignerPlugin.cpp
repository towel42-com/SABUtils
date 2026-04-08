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
#include "../SummaryDateEdit.h"
#include "SummaryDateEditDesignerPlugin.h"

#include <QtPlugin>

using namespace Qt::StringLiterals;

CSummaryDateEditDesignerPlugin::CSummaryDateEditDesignerPlugin( QObject *parent ) :
    QObject( parent )
{
}

void CSummaryDateEditDesignerPlugin::initialize( QDesignerFormEditorInterface * /* core */ )
{
    if ( fInitialized )
        return;

    fInitialized = true;
}

bool CSummaryDateEditDesignerPlugin::isInitialized() const
{
    return fInitialized;
}

QWidget *CSummaryDateEditDesignerPlugin::createWidget( QWidget *parent )
{
    return new NTowel42Utils::CSummaryDateEdit( parent );
}

QString CSummaryDateEditDesignerPlugin::name() const
{
    return u"NTowel42Utils::CSummaryDateEdit"_s;
}

QString CSummaryDateEditDesignerPlugin::group() const
{
    return u"Towel 42 Utilities"_s;
}

QIcon CSummaryDateEditDesignerPlugin::icon() const
{
    return {};
}

QString CSummaryDateEditDesignerPlugin::toolTip() const
{
    return u"NTowel42Utils::CSummaryDateEdit"_s;
}

QString CSummaryDateEditDesignerPlugin::whatsThis() const
{
    return u"NTowel42Utils::CSummaryDateEdit"_s;
}

bool CSummaryDateEditDesignerPlugin::isContainer() const
{
    return false;
}

QString CSummaryDateEditDesignerPlugin::domXml() const
{
    return uR"(
<ui language="c++">
  <widget class="NTowel42Utils::CSummaryDateEdit" name="summaryDateEdit">
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

QString CSummaryDateEditDesignerPlugin::includeFile() const
{
    return u"T42-Utils/SummaryDateEdit.h"_s;
}
