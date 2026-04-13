#ifndef __VERSIONINFODATA_H
#define __VERSIONINFODATA_H
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

#include "Towel42UtilsExport.h"

#include <cstdint>
#include <QString>
#include <QDateTime>
#include <QSize>
class QWidget;
class QCoreApplication;

namespace NTowel42Utils
{
    struct TOWEL42_UTILS_EXPORT SVersion
    {
        SVersion() {};
        SVersion( const QString &tagName, const QString &createdDate );
        ;
        SVersion( int major, int minor, int patch );
        bool setVersionInfo( const QString &tagName, const QString &createdDate );

        QString toString( bool verbose ) const;
        int fMajor{ -1 };
        int fMinor{ -1 };
        int fPatch{ -1 };
        QDateTime fReleaseDate;
        QString fComment;
    };

    TOWEL42_UTILS_EXPORT int compare( const SVersion &lhs, const SVersion &rhs );

#if __cplusplus >= 202002L
    TOWEL42_UTILS_EXPORT int operator<= > ( const SVersion &lhs, const SVersion &rhs );
#else
    TOWEL42_UTILS_EXPORT bool operator<( const SVersion &lhs, const SVersion &rhs );
    TOWEL42_UTILS_EXPORT bool operator>( const SVersion &lhs, const SVersion &rhs );
    TOWEL42_UTILS_EXPORT bool operator==( const SVersion &lhs, const SVersion &rhs );
    TOWEL42_UTILS_EXPORT bool operator!=( const SVersion &lhs, const SVersion &rhs );
#endif
    struct TOWEL42_UTILS_EXPORT SThirdPartyData
    {
        QString row() const;
        QString fCompany;
        QString fProduct;
        QString fLicense;
        QString fLicenseURL;
        QString fSource;
        QString fSourceURL;
        QString fPatchDesc;
        QString fPatchURL;
    };

    class TOWEL42_UTILS_EXPORT CVersionInfoData
    {
    public:
        enum class EHomePageType
        {
            eNone,
            eProduct,
            eVendor
        };

        virtual int majorVersion() const = 0;
        virtual int minorVersion() const = 0;
        virtual int patchVersion() const = 0;
        virtual QString gitVersion() const = 0;
        virtual uint16_t patchVersionLow() const = 0;
        virtual uint16_t patchVersionHigh() const = 0;
        virtual bool modified() const = 0;
        virtual QString ahead() const = 0;
        virtual QString appName() const = 0;
        virtual QString vendor() const = 0;
        virtual QString vendorHomePage() const = 0;
        virtual QString productHomePage() const = 0;
        virtual QString email() const = 0;
        virtual QString copyright() const = 0;
        virtual QString buildDateUTC() const = 0;
        virtual QString buildTimeUTC() const = 0;
        virtual bool forceMinorVersionTwoDigits() const { return false; }
        virtual QString aboutText() const { return fAboutText; }
        virtual QString logoPath() const { return fLogoPath; }
        virtual QSize logoSize() const { return QSize( 128, 128 ); }
        virtual const std::list< SThirdPartyData > &thirdPartyData() const { return fThirdPartyData; }

        virtual QDateTime buildDateTime( bool localTime ) const;
        virtual QString getBuildDateText( bool localTime, bool includeTZ, bool sortableDate ) const;
        virtual QString getVersionText( bool verbose ) const;   // verbose adds git version and status
        virtual QString getGitVersionAndStatus() const;
        virtual QString getVersionTextEX( bool localTime, bool full, bool sortableDate ) const;
        virtual QString getWindowTitle( bool verbose = true, EHomePageType homePage = EHomePageType::eProduct ) const;
        virtual void setupApplication( EHomePageType homePage = EHomePageType::eProduct ) const;

        void setAboutText( const QString &text ) { fAboutText = text; }
        void setLogoPath( const QString &path ) { fLogoPath = path; }
        void setThirdPartyData( const std::list< SThirdPartyData > &thirdPartyData ) { fThirdPartyData = thirdPartyData; }

    protected:
        QString homePage( EHomePageType homePage ) const;

    private:
        QString fAboutText;
        QString fLogoPath;
        std::list< SThirdPartyData > fThirdPartyData;
    };
}
#endif
