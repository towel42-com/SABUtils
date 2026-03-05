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
#include <windows.h>
#include <QDate>

#define ADDIN_API __declspec( dllexport )

typedef struct tagDEBUGHELPER
{
    DWORD dwVersion;
    BOOL( WINAPI *ReadDebuggeeMemory )( struct tagDEBUGHELPER *pThis, DWORD dwAddr, DWORD nWant, VOID *pWhere, DWORD *nGot );
    // from here only when dwVersion >= 0x20000
    DWORDLONG( WINAPI *GetRealAddress )( struct tagDEBUGHELPER *pThis );
    BOOL( WINAPI *ReadDebuggeeMemoryEx )( struct tagDEBUGHELPER *pThis, DWORDLONG qwAddr, DWORD nWant, VOID *pWhere, DWORD *nGot );
    int( WINAPI *GetProcessorType )( struct tagDEBUGHELPER *pThis );
} DEBUGHELPER;

extern "C" ADDIN_API HRESULT WINAPI OnShoreCSDbgEE_QDate( DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t max, DWORD reserved )
{
    QDate FAR date;
    DWORD nGot = 0;
    pHelper->ReadDebuggeeMemory( pHelper, dwAddress, sizeof( date ), &date, &nGot );

    char tmp[ 255 ];
    strcpy( tmp, date.toString().toAscii() );
    wsprintf( pResult, "QDate: '%s'", tmp );
    return S_OK;
}

extern "C" ADDIN_API HRESULT WINAPI OnShoreCSDbgEE_QTime( DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t max, DWORD reserved )
{
    QTime FAR time;
    DWORD nGot = 0;
    pHelper->ReadDebuggeeMemory( pHelper, dwAddress, sizeof( time ), &time, &nGot );

    char tmp[ 255 ];
    strcpy( tmp, time.toString().toAscii() );
    wsprintf( pResult, "QTime: '%s'", tmp );
    return S_OK;
}

extern "C" ADDIN_API HRESULT WINAPI OnShoreCSDbgEE_QDateTime( DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t max, DWORD reserved )
{
    QDateTime FAR dt;
    DWORD nGot = 0;
    pHelper->ReadDebuggeeMemory( pHelper, dwAddress, sizeof( dt ), &dt, &nGot );

    char tmp[ 255 ];
    strcpy( tmp, dt.toString().toAscii() );
    wsprintf( pResult, "QDateTime: '%s'", tmp );
    return S_OK;
}
