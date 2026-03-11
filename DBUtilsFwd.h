// The MIT License( MIT )
//
// Copyright( c ) 2022 Towel 42 Development, LLC and Scott Aron Bloom
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

#ifndef __DBUTILSFWD_H
#define __DBUTILSFWD_H

#include "Towel42UtilsExport.h"

#ifdef TOWEL42_QSQL_SUPPORT
    #include <QString>
    #include <QSqlDatabase>
    #include <unordered_map>
class QVariant;

namespace NTowel42Utils
{
    using TParameterVariantMap = std::unordered_map< QString, QVariant >;
    using TParameterStringMap = std::unordered_map< QString, QString >;
    using TParameter = std::pair< QString, QVariant >;
    using TEnumValue = std::pair< int, QString >;
    using TEnumValueVector = std::vector< TEnumValue >;

    class TOWEL42_UTILS_EXPORT CTransaction
    {
    public:
        CTransaction();
        CTransaction( const QSqlDatabase &db );
        ~CTransaction();

        void setRollback( bool rollback = true ) { fRollback = rollback; }

    private:
        QSqlDatabase fDatabase;
        bool fRollback{ false };
    };

    struct TOWEL42_UTILS_EXPORT SColumnInfo
    {
        QString columnDef() const;
        SColumnInfo() = default;
        SColumnInfo( int colID, const QString &name, const QString &colType, bool notNull, const QString &defValue, bool primKey, const QString &contraint );

        int fColID{ -1 };
        QString fName;
        QString fType;
        bool fNotNull{ false };
        QString fDefaultValue;
        bool fPrimaryKey{ false };
        QString fConstraint;
    };

    struct TOWEL42_UTILS_EXPORT SDBVersion
    {
        SDBVersion() = default;
        SDBVersion( const QString &str );
        int fMajor;
        int fMinor;
        int fPatch;
    };
}

#endif

#endif
