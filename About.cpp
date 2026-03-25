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

#include "About.h"
#include <QWidget>

#include "uiUtils.h"

#include <QMessageBox>
#include <QPushButton>

namespace NTowel42Utils
{
    void showThirdPartyLicenses( std::shared_ptr< CVersionInfoData > versionInfo, QWidget *parent )
    {
        auto title = QObject::tr( "3rd Party Licenses" );
        auto caption = QObject::tr( "<h3>%1 - 3rd Party Licenses</h3>" ).arg( versionInfo->appName() );
        auto aboutText = QObject::tr( "<p>%1 uses multiple 3rd Party software mostly covered under the Qt distribution</p>"
                                      "<p>However the following license(s) are not part of Qt.</p>"
                                      R"(<hr style="width:50%;text-align:left;margin-left:0">)"
                                      R"(<table border="1">)"
                                      R"(<tr>)"
                                      R"(<th>Company</th>)"
                                      R"(<th>Product</th>)"
                                      R"(<th>License</th>)"
                                      R"(<th>Source Location</th>)"
                                      R"(<th>Patches</th>)"
                                      R"(</tr>)" )
                             .arg( versionInfo->vendor() );

        for ( auto &&ii : versionInfo->thirdPartyData() )
        {
            aboutText += ii.row();
        }
        aboutText += "</table>";

        auto msgBox = new QMessageBox( parent );
        msgBox->setAttribute( Qt::WA_DeleteOnClose );
        msgBox->setWindowTitle( title );
        msgBox->setText( caption );
        msgBox->setInformativeText( aboutText );
        msgBox->setTextInteractionFlags( Qt::TextBrowserInteraction );

        msgBox->exec();
    }

    void showAbout( std::shared_ptr< CVersionInfoData > versionInfo, QWidget *parent )
    {
        auto title = QObject::tr( "About %1" ).arg( versionInfo->appName() );
        auto caption = QObject::tr( "<h3>About %1</h3>"
                                    "<p>%1</p><p>Version: %2</p><p>Build Date: %3</p>" )
                           .arg( versionInfo->appName() )
                           .arg( versionInfo->getVersionText( true ) )
                           .arg( versionInfo->getBuildDateText( true, false, false ) );

        auto homePage = versionInfo->homePage();
        if ( !homePage.startsWith( "http" ) )
            homePage = QStringLiteral( "https://" ) + homePage;

        auto aboutText = QObject::tr( R"__(
<p>%1</p>
<p><a href="%2">%2</a>.</p>
<hr style="width:50%;text-align:left;margin-left:0">
<p>%4</p>)__" )   //
                .arg( versionInfo->aboutText() )
                .arg( homePage )
                .arg( versionInfo->copyright() );

        auto msgBox = new QMessageBox( parent );
        msgBox->setAttribute( Qt::WA_DeleteOnClose );
        msgBox->setWindowTitle( title );
        msgBox->setText( caption );
        msgBox->setInformativeText( aboutText );
        msgBox->setTextInteractionFlags( Qt::TextBrowserInteraction );
        auto btn = msgBox->addButton( QObject::tr( "&About Qt" ), QMessageBox::HelpRole );
        QObject::disconnect( btn, &QPushButton::clicked, nullptr, nullptr );
        QObject::connect( btn, &QPushButton::clicked, [ = ]() { QMessageBox::aboutQt( msgBox ); } );

        if ( !versionInfo->thirdPartyData().empty() )
        {
            btn = msgBox->addButton( QObject::tr( "&3rd Party Licenses" ), QMessageBox::HelpRole );
            QObject::disconnect( btn, &QPushButton::clicked, nullptr, nullptr );
            QObject::connect( btn, &QPushButton::clicked, [ = ]() { showThirdPartyLicenses( versionInfo, msgBox ); } );
        }

        msgBox->setEscapeButton( msgBox->addButton( QMessageBox::Ok ) );
        auto img = NTowel42Utils::pixmapForImageFile( versionInfo->avatarPath(), QSize( 128, 128 ) );
        if ( img.has_value() )
            msgBox->setIconPixmap( img.value() );

        msgBox->exec();
    }
}