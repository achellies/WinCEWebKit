/*
 * Copyright (C) 2010 Igalia S.L
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef PlatformVideoWindow_h
#define PlatformVideoWindow_h
#if USE(GSTREAMER)

#include "Widget.h"
#include <wtf/PassRefPtr.h>
#include <wtf/RefCounted.h>

typedef struct _GstMessage GstMessage;

namespace WebCore {

class PlatformVideoWindow : public RefCounted<PlatformVideoWindow> {
    public:
        static PassRefPtr<PlatformVideoWindow> createWindow() { return adoptRef(new PlatformVideoWindow()); }

        PlatformVideoWindow();
        ~PlatformVideoWindow();


        void prepareForOverlay(GstMessage*);
        PlatformWidget window() const { return m_window; }
        unsigned long videoWindowId() const { return m_videoWindowId; }

    private:
        unsigned long m_videoWindowId;
        PlatformWidget m_videoWindow;
        PlatformWidget m_window;
    };
}

#endif // USE(GSTREAMER)
#endif
