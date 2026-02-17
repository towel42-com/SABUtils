#include "CustomerData.h"
#include <optional>
#include <string>
#include <algorithm>
#include <utility>
#include <string_view>
#include <unordered_set>
#include <tuple>
#include <regex>

#ifdef TOWEL42_QCORE_SUPPORT
    #include <QString>
    #include <QFile>
    #include <QStringView>
    #include <QStringList>
    #include <QTemporaryFile>
    #ifdef TOWEL42_QSQL_SUPPORT
        #include "DBUtils.h"
        #include <QSqlDatabase>
        #include <QSqlQuery>
    #endif
#endif

namespace NTowel42Utils
{
    static bool isValidOneDigitCountryCode( std::string_view text )
    {
        if ( text.empty() )
            return false;
        if ( text[ 0 ] == '1' )
            return true;
        return text.length() == 10;
    }

    static bool isValidTwoDigitCountryCode( std::string_view text )
    {
        if ( text.length() <= 2 )
            return false;

        // clang-format off
    static std::unordered_set< std::string > sValidTwoDigit =   //
        {
            "20", "27",   //
            "30", "31", "32", "33", "34", "36", "39",   //
            "40", "41", "43", "44", "45", "46", "47", "48", "49",   //
            "51", "52", "53", "54", "55", "56", "57", "58",   //
            "60", "61", "62", "63", "64", "65", "66"   //
            "70", "71", "72", "73", "74", "75", "76", "77", "78", "79",   //
            "81", "82", "84", "86", //
            "90", "91", "92", "93", "94", "95", "98",   //
        };
        // clang-format on
        auto pos = sValidTwoDigit.find( std::string( text ) );
        return ( pos != sValidTwoDigit.end() );
    }

    static bool isValidThreeDigitCountryCode( std::string_view text )
    {
        if ( text.length() <= 3 )
            return false;

        // https://en.wikipedia.org/wiki/List_of_telephone_country_codes
        // clang-format off
    static std::unordered_set< std::string > sValidThreeDigit = 
        {
                   "211", "212", "213",               "216",        "218",
            "220", "221", "222", "223", "224", "225", "226", "227", "228", "229",
            "230", "231", "232", "233", "234", "235", "236", "237", "238", "239",
            "240", "241", "242", "243", "244", "245", "246", "247", "248", "249",
            "250", "251", "252", "253", "254", "255", "256", "257", "258",
            "260", "261", "262", "263", "264", "265", "266", "267", "268", "269",
            "290", "291",                                    "297", "298", "299",

            "350", "351", "352", "353", "354", "355", "356", "357", "358", "359",
            "370", "371", "372", "373", "374", "375", "376", "377", "378", "379",
            "380", "381", "382", "383",        "385", "386", "387",        "389",

            "420", "421", "423",
            
            "500", "501", "502", "503", "504", "505", "506", "507", "508", "509",
            "590", "591", "592", "593", "594", "595", "596", "597", "598", "599",

        // 67, 68, 69
            "670",        "672", "673", "674", "675", "676", "677", "678", "679",
            "680", "681", "682", "683",        "685", "686", "687", "688", "689",
            "690", "691", "692"

        //80, 85, 87, 88
            "800", "801", "802", "803", "804", "805", "806", "807", "808", "809",
            "850",        "852", "853",        "855", "856",
            "870",                                                  "878"
            "880", "881", "882", "883",               "886",               "889",

        //96, 97, 99
            "960", "961", "962", "963", "964", "965", "966", "967", "968",
            "970", "971", "972", "973", "974", "975", "976", "977",        "979",
                   "991", "992", "993", "994", "995", "996",        "998"
        };
        // clang-format on
        auto pos = sValidThreeDigit.find( std::string( text ) );
        return ( pos != sValidThreeDigit.end() );
    }

    // all non digits must be removed
    std::optional< std::pair< std::string, std::string > > splitByCountryCode( std::string_view text )
    {
        if ( text.empty() )
            return {};

        if ( isValidOneDigitCountryCode( text ) )
        {
            if ( text.length() == 10 )
                return std::make_pair( std::string(), std::string( text ) );
            else
                return std::make_pair( std::string( text.substr( 0, 1 ) ), std::string( text.substr( 1 ) ) );
        }

        if ( text[ 0 ] == '+' )
            text.remove_prefix( 1 );

        // remaining countries have 2 or more digits to the country code
        if ( isValidTwoDigitCountryCode( text ) )
        {
            return std::make_pair( std::string( text.substr( 0, 2 ) ), std::string( text.substr( 2 ) ) );
        }

        if ( isValidThreeDigitCountryCode( text ) )
        {
            return std::make_pair( std::string( text.substr( 0, 3 ) ), std::string( text.substr( 3 ) ) );
        }
        return {};
    }

    std::optional< std::string > fixupPhoneNumber( const std::string &currText )
    {
        //auto regEx = R"__(^[\+]?[(]?[0-9]{3}[)]?[-\s\.]?[0-9]{3}[-\s\.]?[0-9]{4,6}$)__";

        auto retVal = currText;

        retVal.erase(
            std::remove_if(
                retVal.begin(), retVal.end(),   //
                []( char curr )   //
                {
                    if ( ( ( curr >= '0' ) && ( curr <= '9' ) ) || ( curr == 'x' ) )
                        return false;
                    return true;
                } ),
            retVal.end() );

        std::string extension;
        auto pos = retVal.find( 'x' );
        if ( pos != std::string::npos )
        {
            extension = retVal.substr( pos );
            retVal = retVal.substr( 0, pos );
        }
        if ( retVal.empty() || ( retVal.length() > 16 ) )
            return {};

        auto validCountryCode = splitByCountryCode( retVal );
        if ( !validCountryCode.has_value() )
            return {};

        auto &&countryCode = validCountryCode.value().first;
        auto &&number = validCountryCode.value().second;

        retVal.clear();
        if ( countryCode.empty() || countryCode == "1" )
        {
            // us
            auto areaCode = number.substr( 0, 3 );
            auto prefix = number.substr( 3, 3 );
            number = number.substr( 6 );

            if ( countryCode == "1" )
                retVal += "+1 ";
            retVal += "(" + areaCode + ") ";
            retVal += prefix + "-" + number;
        }
        else
            retVal = "+" + countryCode + " " + number;
        if ( retVal.empty() )
            return {};
        if ( !extension.empty() )
            retVal += extension;
        return retVal;
    }

    bool isValidEmail( const std::string &email )
    {
        std::regex regEx( R"__([^@ \s]+@[^@ \s]+\.[^@ \s]+)__" );
        std::smatch matches;
        bool retVal = std::regex_search( email, matches, regEx );
        return retVal;
    }

#ifdef TOWEL42_QCORE_SUPPORT
    std::optional< QString > fixupPhoneNumber( const QString &phoneNumber )
    {
        auto retVal = fixupPhoneNumber( phoneNumber.toStdString() );
        if ( retVal.has_value() )
            return QString::fromStdString( retVal.value() );
        return {};
    }

    std::optional< std::pair< QString, QString > > splitByCountryCode( const QString &phoneNumber )
    {
        auto retVal = splitByCountryCode( phoneNumber.toStdString() );
        if ( retVal.has_value() )
        {
            auto first = QString::fromStdString( retVal.value().first );
            auto second = QString::fromStdString( retVal.value().second );
            return std::make_pair( first, second );
        }
        return {};
    }

    bool isValidEmail( const QString &email )
    {
        return isValidEmail( email.toStdString() );
    }

    #ifdef TOWEL42_QSQL_SUPPORT

    std::shared_ptr< QTemporaryFile > getCustomerDataFile()
    {
        static std::optional< std::shared_ptr< QTemporaryFile > > sRetVal;
        if ( !sRetVal.has_value() )
        {
            auto inputFile = QFile( ":/Towel42UtilsResources/CustomerData.db" );
            if ( !inputFile.open( QFile::OpenModeFlag::ReadOnly ) )
                return {};

            auto nativeFile = QTemporaryFile::createNativeFile( inputFile );
            if ( !nativeFile )
                return {};
            nativeFile->setAutoRemove( true );

            sRetVal = std::shared_ptr< QTemporaryFile >( nativeFile );
        }
        return sRetVal.value();
    }

    QSqlDatabase getCustomerDataDB()
    {
        static std::optional< QSqlDatabase > sRetVal;
        if ( !sRetVal.has_value() )
        {
            auto file = getCustomerDataFile();
            if ( !file )
                return {};

            if ( !validateSQLITEInstalled( nullptr ) )
                return {};

            QSqlDatabase db = QSqlDatabase::addDatabase( "QSQLITE", "customerDataDB" );
            db.setDatabaseName( file->fileName() );
            if ( !db.open() )
                return {};
            sRetVal = db;
        }
        return sRetVal.value();
    }

    QStringList stateList()
    {
        static std::optional< QStringList > retVal;
        if ( retVal.has_value() )
            return retVal.value();

        auto db = getCustomerDataDB();
        if ( !db.isOpen() )
            return {};

        QSqlQuery query( db );

        if ( !runCmd( query, "select State from States ORDER BY State" ) )
            return {};

        QStringList tmp;
        while ( query.next() )
        {
            auto state = query.value( 0 ).toString();
            tmp << state;
        }
        retVal = tmp;
        return tmp;
    }

    TOWEL42_UTILS_EXPORT std::optional< QString > getStateNameForAbbreviation( const QString &abbr )
    {
        auto db = getCustomerDataDB();
        if ( !db.isOpen() )
            return {};

        QSqlQuery query( db );

        if ( !runCmd( query, "select State from States WHERE CODE LIKE :abbr", abbr ) )
            return {};
        if ( !query.next() )
            return {};

        return query.value( 0 ).toString();
    }

    bool isValidZipCode( const QString &zipCode )
    {
        if ( ( zipCode.isEmpty() ) || ( ( zipCode.length() > 5 ) && ( zipCode[ 5 ] != '-' ) ) )
            return false;

        auto zip = zipCode.left( 5 );
        QString zip4;
        if ( zipCode.length() > 5 )
            zip4 = zipCode.mid( 6 );
        if ( zip4.length() != 4 )
            return false;

        auto db = getCustomerDataDB();
        if ( !db.isOpen() )
            return {};

        QSqlQuery query( db );

        QVariantList params;
        params << zip;
        QString cmd = "SELECT * FROM ZipCodes WHERE ( ZipCode = :zipcode )";
        if(!zip4.isEmpty())
        {
            cmd += " AND ( Zip4Code = :zip4code )";
            params << zip4;
        }

        if ( !runCmd( query, cmd, params ) )
            return {};

        return query.next();
    }

    QString cleanCityName( QStringView cityName )
    {
        auto retVal = QString( cityName ).trimmed();
        bool first = false;
        for ( int ii = 1; ii < retVal.length(); ++ii )
        {
            if ( !first )
                retVal[ ii ] = retVal[ ii ].toLower();
            first = retVal[ ii ] == ' ';
        }
        return retVal;
    }

    std::optional< TCityStateZip > getCityStateForZip( const QString &zipCode, bool useStateAbbreviation )
    {
        auto db = getCustomerDataDB();
        if ( !db.isOpen() )
            return {};

        QSqlQuery query( db );

        if ( !runCmd( query, "SELECT City, State, ZipCode, Zip4Code FROM ZipCodes WHERE ZipCode LIKE :zipcode", zipCode + '%' ) )
            return {};

        if ( query.next() )
        {
            auto city = query.value( 0 ).toString();
            city = cleanCityName( city );
            auto state = query.value( 1 ).toString();
            if ( !useStateAbbreviation )
            {
                auto stateName = getStateNameForAbbreviation( state );
                if ( !stateName.has_value() )
                    return {};
                state = stateName.value();
            }
            auto zipCode = query.value( 2 ).toString();
            auto zip4Code = query.value( 3 ).toString();
            if ( !zipCode.isEmpty() && !zip4Code.isEmpty() )
                zipCode += "-" + zip4Code;
            return TCityStateZip( city, state, zipCode );
        }
        return {};
    }

    #endif
#endif
}