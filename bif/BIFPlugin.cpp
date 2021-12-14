// The MIT License( MIT )
//
// Copyright( c ) 2020-2021 Scott Aron Bloom
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


#include "BIFPlugin.h"
#include "BIFIOHandler.h"
#include "SABUtils/BIFFile.h"

CBIFPlugin::CBIFPlugin()
{
}

CBIFPlugin::~CBIFPlugin()
{
}

QImageIOPlugin::Capabilities CBIFPlugin::capabilities(QIODevice *device, const QByteArray &format) const
{
    if (format == "bif" || ( device && device->isReadable() && CBIFIOHandler::canRead(device) ) )
        return Capabilities(CanRead);
    return {};
}

namespace NBIFPlugin
{
    static std::list<CBIFIOHandler * > sHandlers;
    static int sLoopCount{ -1 };
    void setLoopCount(int loopCount)
    {
        sLoopCount = loopCount;
        for (auto && ii : sHandlers)
            ii->setLoopCount(loopCount);
    }
}

QImageIOHandler *CBIFPlugin::create(QIODevice *device, const QByteArray &format) const
{
    auto handler = new CBIFIOHandler;
    handler->setDevice(device);
    handler->setFormat(format);
    handler->setLoopCount(NBIFPlugin::sLoopCount);
    NBIFPlugin::sHandlers.push_back(handler);
    return handler;
}
