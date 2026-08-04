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

#include "MoveToTrash.h"
#include "FileUtils.h"
#include "WindowsError.h"

#include <windows.h>

#include <Shobjidl.h>   //Required for IFileOperation Interface
#include <shellapi.h>   //Required for Flags set in "SetOperationFlags"
#include <Shlwapi.h>
#pragma comment( lib, "Shlwapi.lib" )

#include <iostream>
#include <functional>
#include <filesystem>

#ifdef QT_CORE_LIB
    #include <QFileInfo>
    #include <QString>
#endif

namespace NTowel42Utils
{
    namespace NFileUtils
    {
        class CFileOpProgSinkApp : public IFileOperationProgressSink
        {
        public:
            CFileOpProgSinkApp( std::shared_ptr< SRecycleOptions > options ) :
                _cRef( 1 ),
                _hwnd( NULL ),
                _hwndLV( NULL ),
                fOptions( options )
            {
            }

            // IUnknown
            IFACEMETHODIMP QueryInterface( REFIID riid, void **ppv )
            {
                static const QITAB qit[] = {
                    QITABENT( CFileOpProgSinkApp, IFileOperationProgressSink ),
                    { 0 },
                };
                return QISearch( this, qit, riid, ppv );
            }

            IFACEMETHODIMP_( ULONG ) AddRef() { return InterlockedIncrement( &_cRef ); }

            IFACEMETHODIMP_( ULONG ) Release()
            {
                ULONG cRef = InterlockedDecrement( &_cRef );
                if ( 0 == cRef )
                {
                    delete this;
                }
                return cRef;
            }

            std::filesystem::path getPathForItem( IShellItem *psiItem ) const
            {
                if ( !psiItem )
                    return {};
                std::wstring retVal;
                PWSTR pszItem;
                HRESULT hr = psiItem->GetDisplayName( SIGDN_FILESYSPATH, &pszItem );
                if ( SUCCEEDED( hr ) )
                    retVal = pszItem;
                CoTaskMemFree( pszItem );
                return retVal;
            }

            bool verbose() const { return fOptions && fOptions->fVerbose; }
            const char *statusPrefix( HRESULT hrRename ) const { return ( SUCCEEDED( hrRename ) ? "" : "Error: " ); }

            std::wstring replaceSource( const std::wstring &msg, IShellItem *psiItem ) const
            {
                auto pathName = getPathForItem( psiItem ).wstring();
                if ( !pathName.empty() )
                {
                    pathName += L" '" + std::wstring( std::filesystem::is_regular_file( pathName ) ? L"file" : L"directory" );
                }
                auto realMsg = msg;
                auto idx = realMsg.find( L"<SOURCE>" );
                if ( idx != std::wstring::npos )
                {
                    realMsg.replace( idx, 8, pathName );
                }
                return realMsg;
            }

            IFACEMETHODIMP startStatus( const std::wstring &msg, IShellItem *srcItem = nullptr ) const
            {
                if ( verbose() )
                    std::wcout << replaceSource( msg, srcItem ) << std::endl;
                return S_OK;
            }

            IFACEMETHODIMP returnFinishedStatus( HRESULT hr, const std::wstring &msg, IShellItem *srcItem = nullptr ) const
            {
                if ( verbose() )
                {
                    ( SUCCEEDED( hr ) ? std::wcout : std::wcerr ) << statusPrefix( hr ) << replaceSource( msg, srcItem ) << std::endl;
                    if ( !SUCCEEDED( hr ) )
                    {
                        ( SUCCEEDED( hr ) ? std::wcout : std::wcerr ) << statusPrefix( hr ) << "    " << getWindowsErrorStd( hr ) << std::endl;
                    }
                }
                return hr;
            }

            IFACEMETHODIMP StartOperations();
            IFACEMETHODIMP FinishOperations( HRESULT hrResult );

            IFACEMETHODIMP PreRenameItem( DWORD /*dwFlags*/, IShellItem * /*psiItem*/, PCWSTR /*pszNewName*/ );
            IFACEMETHODIMP PostRenameItem( DWORD /*dwFlags*/, IShellItem * /*psiItem*/, PCWSTR /*pszNewName*/, HRESULT /*hrRename*/, IShellItem * /*psiNewlyCreated*/ );

            IFACEMETHODIMP PreMoveItem( DWORD /*dwFlags*/, IShellItem * /*psiItem*/, IShellItem * /*psiDestinationFolder*/, PCWSTR /*pszNewName*/ );
            IFACEMETHODIMP PostMoveItem( DWORD /*dwFlags*/, IShellItem * /*psiItem*/, IShellItem * /*psiDestinationFolder*/, PCWSTR /*pszNewName*/, HRESULT /*hrNewName*/, IShellItem * /*psiNewlyCreated*/ );

            IFACEMETHODIMP PreCopyItem( DWORD dwFlags, IShellItem *psiItem, IShellItem *psiDestinationFolder, PCWSTR pszNewName );
            IFACEMETHODIMP PostCopyItem( DWORD dwFlags, IShellItem *psiItem, IShellItem *psiDestinationFolder, PCWSTR pwszNewName, HRESULT hrCopy, IShellItem *psiNewlyCreated );

            IFACEMETHODIMP PreDeleteItem( DWORD /*dwFlags*/, IShellItem *psiItem );
            IFACEMETHODIMP PostDeleteItem( DWORD /*dwFlags*/, IShellItem * /*psiItem*/, HRESULT /*hrDelete*/, IShellItem * /*psiNewlyCreated*/ );

            IFACEMETHODIMP PreNewItem( DWORD /*dwFlags*/, IShellItem * /*psiDestinationFolder*/, PCWSTR /*pszNewName*/ );
            IFACEMETHODIMP PostNewItem( DWORD /*dwFlags*/, IShellItem * /*psiDestinationFolder*/, PCWSTR /*pszNewName*/, PCWSTR /*pszTemplateName*/, DWORD /*dwFileAttributes*/, HRESULT /*hrNew*/, IShellItem * /*psiNewItem*/ );

            IFACEMETHODIMP UpdateProgress( UINT iWorkTotal, UINT iWorkSoFar );

            IFACEMETHODIMP ResetTimer();
            IFACEMETHODIMP PauseTimer();
            IFACEMETHODIMP ResumeTimer();

        private:
            ~CFileOpProgSinkApp() {}
            std::shared_ptr< SRecycleOptions > fOptions;

            long _cRef;
            HWND _hwnd;
            HWND _hwndLV;
        };

        IFACEMETHODIMP CFileOpProgSinkApp::StartOperations()
        {
            return S_OK;   // startStatus( "Starting operation" );
        }

        IFACEMETHODIMP CFileOpProgSinkApp::FinishOperations( HRESULT hr )
        {
            return hr;   // returnFinishedStatus( hr, "Finished operation" );
        }

        IFACEMETHODIMP CFileOpProgSinkApp::PreRenameItem( DWORD /*dwFlags*/, IShellItem *psiItem, PCWSTR pszNewName )
        {
            return startStatus( std::wstring( L"Renaming <SOURCE> to '" ) + std::wstring( pszNewName ) + std::wstring( L"'." ), psiItem );
        }

        IFACEMETHODIMP CFileOpProgSinkApp::PostRenameItem( DWORD /*dwFlags*/, IShellItem *psiItem, PCWSTR pszNewName, HRESULT hrRename, IShellItem *psiNewlyCreated )
        {
            auto pathName = getPathForItem( psiItem );
            return returnFinishedStatus( hrRename, std::wstring( L"Renamed <SOURCE> to '" ) + std::wstring( pszNewName ) + L"'.  Final path name: '" + getPathForItem( psiNewlyCreated ).wstring() );
        }

        IFACEMETHODIMP CFileOpProgSinkApp::PreMoveItem( DWORD /*dwFlags*/, IShellItem *psiItem, IShellItem *psiDestinationFolder, PCWSTR pszNewName )
        {
            return startStatus( L"Moving <SOURCE> to directory '" + getPathForItem( psiDestinationFolder ).wstring() + L"' as '" + std::wstring( pszNewName ) + L"'.", psiItem );
        }

        IFACEMETHODIMP CFileOpProgSinkApp::PostMoveItem( DWORD /*dwFlags*/, IShellItem *psiItem, IShellItem *psiDestinationFolder, PCWSTR pszNewName, HRESULT hrNewName, IShellItem *psiNewlyCreated )
        {
            return returnFinishedStatus( hrNewName, L"Moved <SOURCE> to directory '" + getPathForItem( psiDestinationFolder ).wstring() + L"' as '" + std::wstring( pszNewName ) + L"'. Final path name '" + getPathForItem( psiNewlyCreated ).wstring() + L".'", psiItem );
        }

        IFACEMETHODIMP CFileOpProgSinkApp::PreCopyItem( DWORD /*dwFlags*/, IShellItem *psiItem, IShellItem *psiDestinationFolder, PCWSTR pszNewName )
        {
            return startStatus( L"Copying <SOURCE> to directory '" + getPathForItem( psiDestinationFolder ).wstring() + L"' as '" + std::wstring( pszNewName ) + L"'.", psiItem );
        }

        IFACEMETHODIMP CFileOpProgSinkApp::PostCopyItem( DWORD /*dwFlags*/, IShellItem *psiItem, IShellItem *psiDestinationFolder, PCWSTR pszNewName, HRESULT hrCopy, IShellItem *psiNewlyCreated )
        {
            return returnFinishedStatus( hrCopy, L"Copied <SOURCE> to directory '" + getPathForItem( psiDestinationFolder ).wstring() + L"' as '" + std::wstring( pszNewName ) + L"'. Final path name '" + getPathForItem( psiNewlyCreated ).wstring() + L"'.", psiItem );
        }

        IFACEMETHODIMP CFileOpProgSinkApp::PreDeleteItem( DWORD dwFlags, IShellItem *psiItem )
        {
            return startStatus( ( ( dwFlags & TSF_DELETE_RECYCLE_IF_POSSIBLE ) ? std::wstring( L"Recycling" ) : std::wstring( L"Deleting" ) ) + L"<SOURCE>.", psiItem );
        }

        IFACEMETHODIMP CFileOpProgSinkApp::PostDeleteItem( DWORD dwFlags, IShellItem *psiItem, HRESULT hrDelete, IShellItem * /*psiNewlyCreated*/ )
        {
            return returnFinishedStatus( hrDelete, ( ( dwFlags & TSF_DELETE_RECYCLE_IF_POSSIBLE ) ? std::wstring( L"Recycled" ) : std::wstring( L"Deleted" ) ) + L" <SOURCE>.", psiItem );
        }

        IFACEMETHODIMP CFileOpProgSinkApp::PreNewItem( DWORD /*dwFlags*/, IShellItem *psiDestinationFolder, PCWSTR pszNewName )
        {
            return startStatus( L"Creating '" + std::wstring( pszNewName ) + L"' in directory '" + getPathForItem( psiDestinationFolder ).wstring() + L"'." );
        }

        IFACEMETHODIMP CFileOpProgSinkApp::PostNewItem( DWORD /*dwFlags*/, IShellItem *psiDestinationFolder, PCWSTR pszNewName, PCWSTR pszTemplateName, DWORD /*dwFileAttributes*/, HRESULT hrNew, IShellItem *psiNewItem )
        {
            return returnFinishedStatus( hrNew, L"Created '" + std::wstring( pszNewName ) + L"' in directory '" + getPathForItem( psiDestinationFolder ).wstring() + L". Template Name: " + std::wstring( pszTemplateName ) + L". Final Path '" + getPathForItem( psiNewItem ).wstring() + L"." );
        }

        IFACEMETHODIMP CFileOpProgSinkApp::UpdateProgress( UINT iWorkTotal, UINT iWorkSoFar )
        {
            if ( verbose() )
            {
                if ( iWorkTotal )
                    std::cout << "Status " << iWorkSoFar << " of " << iWorkTotal << std::endl;
            }
            return S_OK;
        }

        IFACEMETHODIMP CFileOpProgSinkApp::ResetTimer()
        {
            // if ( verbose() )
            //{
            //     std::cout << "Resetting timer" << std::endl;
            // }
            return S_OK;
        }

        IFACEMETHODIMP CFileOpProgSinkApp::PauseTimer()
        {
            // if ( verbose() )
            //{
            //     std::cout << "Pausing timer" << std::endl;
            // }
            return S_OK;
        }

        IFACEMETHODIMP CFileOpProgSinkApp::ResumeTimer()
        {
            // if ( verbose() )
            //{
            //     std::cout << "Resuming timer" << std::endl;
            // }
            return S_OK;
        }

        std::wstring getFullMsg( const std::wstring &msg, HRESULT code )
        {
            auto fullMessage = msg + L" - " + getWindowsErrorStd( code );
            return fullMessage;
        }

        bool showError( const std::wstring &msg, HRESULT code, std::wstring *fullMessage, bool interactive, std::function< void() > runFunc )
        {
            auto lclMsg = getFullMsg( msg, code );
            if ( fullMessage )
                *fullMessage = lclMsg;
            if ( interactive )
            {
                MessageBox( nullptr, lclMsg.c_str(), L"Error", MB_OK | MB_ICONERROR );
            }
            runFunc();
            return false;
        }

        bool moveToTrashImpl( const std::filesystem::path &relPath, std::wstring *msg, std::shared_ptr< SRecycleOptions > options )
        {
            if ( !std::filesystem::exists( relPath ) )
            {
                if ( msg )
                    *msg = L"File or Directory '" + relPath.wstring() + L"' does not exist.";
                return true;
            }

            HRESULT hr = CoInitializeEx( nullptr, COINIT_MULTITHREADED );
            if ( FAILED( hr ) )
            {
                // Couldn't initialize COM library - clean up and return
                return showError( L"Couldn't initialize COM library", hr, msg, options->fInteractive, []() { CoUninitialize(); } );
            }
            // Initialize the file operation
            IFileOperation *fileOperation;
            hr = CoCreateInstance( CLSID_FileOperation, nullptr, CLSCTX_ALL, IID_PPV_ARGS( &fileOperation ) );
            if ( FAILED( hr ) )
            {
                // Couldn't CoCreateInstance - clean up and return
                return showError( L"Couldn't CoCreateInstance", hr, msg, options->fInteractive, []() { CoUninitialize(); } );
            }
            hr = fileOperation->SetOperationFlags( FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_SILENT | FOF_NO_UI | FOF_NOERRORUI );
            if ( FAILED( hr ) )
            {
                // Couldn't add flags - clean up and return
                return showError(
                    L"Couldn't add flags", hr, msg, options->fInteractive,
                    [ fileOperation ]()
                    {
                        fileOperation->Release();
                        CoUninitialize();
                    } );
            }

            auto path = std::filesystem::absolute( relPath );

            IShellItem *fileOrFolderItem = nullptr;
            hr = SHCreateItemFromParsingName( path.c_str(), nullptr, IID_PPV_ARGS( &fileOrFolderItem ) );
            if ( FAILED( hr ) )
            {
                return showError(
                    L"Couldn't create IShellItem from path", hr, msg, options->fInteractive,
                    [ fileOrFolderItem, fileOperation ]()
                    {
                        if ( fileOrFolderItem )
                            fileOrFolderItem->Release();
                        fileOperation->Release();
                        CoUninitialize();
                    } );
            }

            auto pSync = new CFileOpProgSinkApp( options );
            // DWORD dwCookie = 0;
            // hr = fileOperation->Advise( pSync, &dwCookie );
            // if ( FAILED( hr ) )
            //{
            //     return showError( "Failed to mark file/folder item for deletion", hr,
            //         [ fileOrFolderItem, pSync ]()
            //         {
            //             fileOrFolderItem->Release();
            //             if ( pSync )
            //                 pSync->Release();
            //             CoUninitialize();
            //         } );
            // }

            hr = fileOperation->DeleteItem( fileOrFolderItem, pSync );
            fileOrFolderItem->Release();
            if ( FAILED( hr ) )
            {
                return showError(
                    L"Failed to mark file/folder item for deletion", hr, msg, options->fInteractive,
                    [ fileOrFolderItem, fileOperation, pSync ]()
                    {
                        fileOperation->Release();
                        if ( pSync )
                            pSync->Release();
                        CoUninitialize();
                    } );
            }
            hr = fileOperation->PerformOperations();
            fileOperation->Release();
            if ( pSync )
                pSync->Release();
            CoUninitialize();
            if ( FAILED( hr ) )
            {
                return showError( L"Failed to carry out delete", hr, msg, options->fInteractive, [ fileOrFolderItem, fileOperation ]() {} );
            }
            return true;
        }
    }
}

