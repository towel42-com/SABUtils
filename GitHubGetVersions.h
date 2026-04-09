// The MIT License( MIT )
// The MIT License( MIT )
//
// Copyright( c ) 2022-2026 Towel 42 Development, LLC and Scott Aron Bloom
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

#ifndef __GITHUBGETVERSIONS_H
#define __GITHUBGETVERSIONS_H

#include "Towel42UtilsExport.h"
#include "VersionInfoData.h"
#include <QObject>
#include <QUrl>
#include <QDateTime>
#include <QSslError>
#include <optional>
#include <memory>

class QEventLoop;
class QNetworkAccessManager;
class QNetworkReply;
class QAuthenticator;
class QJsonArray;
class QSslPreSharedKeyAuthenticator;
class QNetworkProxy;

namespace NTowel42Utils
{
    class CVersionInfoData;

    struct SVersion;
    struct TOWEL42_UTILS_EXPORT SGitHubAsset
    {
        SGitHubAsset( const QJsonObject &assetInfo );
        QString getSize() const;

        bool supportsOS() const;
        std::pair< QString, QUrl > fUrl;
        QString fName;
        QString fContentType;
        int fSize{ 0 };
        bool fAOK{ false };
    };

    struct TOWEL42_UTILS_EXPORT SGitHubRelease
    {
        SGitHubRelease( const SGitHubRelease &rhs );
        SGitHubRelease( SGitHubRelease &&rhs );
        SGitHubRelease( const QJsonObject &version );

        QString getTitle() const { return QStringLiteral( "%1 - %2" ).arg( fTagName ).arg( fDescription ); }

        bool operator>( const SGitHubRelease &rhs ) const;
        bool operator<( const SGitHubRelease &rhs ) const;
        bool operator==( const SGitHubRelease &rhs ) const;

        bool supportsOS() const;
        std::list< std::shared_ptr< SGitHubRelease > > splitAssets() const;

        std::shared_ptr< SGitHubAsset > getAssetForOS() const;

        QString fTagName;   // vMajor.Minor
        QString fDescription;
        bool fPreRelease{ false };
        std::unique_ptr< SVersion > fVersion;

        bool fAOK{ false };
        std::list< std::shared_ptr< SGitHubAsset > > fAssets;
    };

    class TOWEL42_UTILS_EXPORT CGitHubGetVersions : public QObject
    {
        Q_OBJECT;

    public:
        CGitHubGetVersions( QObject *parent = nullptr );   // uses determineReleasePath
        CGitHubGetVersions( const QString &baseGitURL, QObject *parent = nullptr );
        ~CGitHubGetVersions();

        void setCurrentVersion( std::shared_ptr< CVersionInfoData > versionInfo );
        void requestLatestVersion();

        QUrl githubReleaseUrl() const;   // uses QApplication::organizationDomain or baseGitURL

        bool hasUpdate() const;
        QString updateVersion() const;
        std::shared_ptr< SGitHubRelease > updateRelease() const;

        bool hasError() const { return fHasError; }
        QString errorString() const { return fErrorString; }

        void setForcedCheck( bool forcedCheck ) { fForcedCheck = forcedCheck; }
        bool forcedCheck() const { return fForcedCheck ; }

        void setTestMode( bool testMode ) { fTestMode = testMode; }
        bool testMode() const { return fTestMode; }
    private Q_SLOTS:
        void slotFinished( QNetworkReply *reply );
        void slotAuthenticationRequired( QNetworkReply *reply, QAuthenticator *authenticator );
        void slotEncrypted( QNetworkReply *reply );
        void slotPreSharedKeyAuthenticationRequired( QNetworkReply *reply, QSslPreSharedKeyAuthenticator *authenticator );
        void slotProxyAuthenticationRequired( const QNetworkProxy &proxy, QAuthenticator *authenticator );
        void slotSSlErrors( QNetworkReply *reply, const QList< QSslError > &errors );

    Q_SIGNALS:
        void sigVersionsDownloaded();
        void sigLogMessage( const QString &message );

    private:
        int getTimeOutDelay() const;
        void loadResults( const QJsonArray &results );
        bool loadResult( const QJsonObject &result, bool topLevelResult );
        void postLoadResults();

        std::optional< SVersion > fCurrentVersion;
        QString fBaseGitURL;
        QByteArray fGitHubToken;

        QNetworkAccessManager *fManager{ nullptr };
        bool fHasError{ false };
        QString fErrorString;

        std::optional< std::pair< QString, std::shared_ptr< SGitHubRelease > > > fLatestUpdate;
        std::list< std::shared_ptr< SGitHubRelease > > fReleases;

        bool fGetLatestVersionOnly{ false };
        bool fForcedCheck{ false };
        bool fTestMode{ false };
    };
}

#endif
