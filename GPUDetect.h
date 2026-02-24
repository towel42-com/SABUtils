// The MIT License( MIT )
//
// Copyright( c ) 2020-2026 Towel 42 Development, LLC and Scott Aron Bloom
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

#ifndef __GPUDETECT_H
#define __GPUDETECT_H

#include "Towel42UtilsExport.h"

#include <string>
#include <list>
#include <memory>
#include <cinttypes>

struct IWbemClassObject;
namespace NTowel42Utils
{
    class TOWEL42_UTILS_EXPORT CGPUInfo
    {
    public:
        CGPUInfo() {};
        CGPUInfo( IWbemClassObject *gpuInfo );

        bool isIntelGPU() const;
        bool isNVidiaGPU() const;
        bool isAMDGPU() const;

    private:
        std::wstring fAcceleratorCapabilities;
        std::wstring fAdapterCompatibility;
        std::wstring fAdapterDACType;
        uint32_t fAdapterRAM{ 0 };
        uint32_t fAvailability{ 0 };
        std::wstring fCapabilityDescriptions;
        std::wstring fCaption;
        std::wstring fColorTableEntries;
        uint32_t fConfigManagerErrorCode{ 0 };
        bool fConfigManagerUserConfig{ false };
        std::wstring fCreationClassName;
        uint32_t fCurrentBitsPerPixel{ 0 };
        uint32_t fCurrentHorizontalResolution{ 0 };
        std::wstring fCurrentNumberOfColors;
        uint32_t fCurrentNumberOfColumns{ 0 };
        uint32_t fCurrentNumberOfRows{ 0 };
        uint32_t fCurrentRefreshRate{ 0 };
        uint32_t fCurrentScanMode{ 0 };
        uint32_t fCurrentVerticalResolution{ 0 };
        std::wstring fDescription;
        std::wstring fDeviceID;
        std::wstring fDeviceSpecificPens;
        uint32_t fDitherType{ 0 };
        std::wstring fDriverDate;
        std::wstring fDriverVersion;
        std::wstring fErrorCleared;
        std::wstring fErrorDescription;
        std::wstring fICMIntent;
        std::wstring fICMMethod;
        std::wstring fInfFilename;
        std::wstring fInfSection;
        std::wstring fInstallDate;
        std::wstring fInstalledDisplayDrivers;
        std::wstring fLastErrorCode;
        std::wstring fMaxMemorySupported;
        std::wstring fMaxNumberControlled;
        uint32_t fMaxRefreshRate{ 0 };
        uint32_t fMinRefreshRate{ 0 };
        bool fMonochrome{ false };
        std::wstring fName;
        std::wstring fNumberOfColorPlanes;
        std::wstring fNumberOfVideoPages;
        std::wstring fPNPDeviceID;
        std::wstring fPowerManagementCapabilities;
        std::wstring fPowerManagementSupported;
        std::wstring fProtocolSupported;
        std::wstring fReservedSystemPaletteEntries;
        std::wstring fSpecificationVersion;
        std::wstring fStatus;
        std::wstring fStatusInfo;
        std::wstring fSystemCreationClassName;
        std::wstring fSystemName;
        std::wstring fSystemPaletteEntries;
        std::wstring fTimeOfLastReset;
        uint32_t fVideoArchitecture{ 0 };
        uint32_t fVideoMemoryType{ 0 };
        std::wstring fVideoMode;
        std::wstring fVideoModeDescription;
        std::wstring fVideoProcessor;
    };

    TOWEL42_UTILS_EXPORT std::list< std::shared_ptr< CGPUInfo > > detectGPUs( std::wstring *errorMsg = nullptr );
    struct TOWEL42_UTILS_EXPORT SGPUInfo
    {
        SGPUInfo( std::wstring *errorMsg = nullptr );
        bool fIntel{ false };
        bool fAMD{ false };
        bool fNVidia{ false };
    };

};

#endif
