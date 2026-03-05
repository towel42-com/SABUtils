// The MIT License( MIT )
//
// Copyright( c ) 2020-2026 Towel 42 Development, LLC and Scott Aron Bloom
// SPDX-License-Identifier: MIT License
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

#ifndef __FILESIDINFO_H
#define __FILESIDINFO_H

#include "Towel42UtilsExport.h"

#include <string>
#include <list>
#include <set>

#ifdef TOWEL42_QCORE_SUPPORT
    #include <QStringList>
class QFileInfo;
class QString;
#endif

namespace NTowel42Utils
{
    namespace NFileUtils
    {
        struct TOWEL42_UTILS_EXPORT SSIDInfo
        {
            SSIDInfo() {}
            SSIDInfo( void *sid );

            std::wstring dispName() const;
            std::wstring fSID;
            std::wstring fDomain;
            std::wstring fOwner;

            bool operator<( const SSIDInfo &rhs ) const { return dispName() < rhs.dispName(); }
            std::pair< bool, std::wstring > fErrorMsg{ false, std::wstring() };
        };

        class TOWEL42_UTILS_EXPORT CSIDFileInfo
        {
        public:
            // if onlyinvalid is true, DACLs will only contained invalid SID data
            CSIDFileInfo( const std::wstring &pathName, bool onlyInvalidDACL = false );
            CSIDFileInfo( const std::string &pathName, bool onlyInvalidDACL = false );
#ifdef TOWEL42_QCORE_SUPPORT
            CSIDFileInfo( const QString &pathName, bool onlyInvalidDACL = false );
            CSIDFileInfo( const QFileInfo &fileInfo, bool onlyInvalidDACL = false );
#endif
            std::wstring daclsString() const;

            bool hasInvalidSID() const;
            bool aOK() const { return fAOK; }

            const SSIDInfo &owner() const { return fOwner; }
            const SSIDInfo &group() const { return fGroup; }
            const std::set< SSIDInfo > &dacls() const { return fDACLs; }

        private:
            bool fAOK{ false };
            bool fOnlyInvalidDACL{ false };
            std::wstring fFileName;
            std::list< std::wstring > fMsg;   // messages found;

            SSIDInfo fOwner;
            SSIDInfo fGroup;
            std::set< SSIDInfo > fDACLs;
        };
    }
}
#endif
