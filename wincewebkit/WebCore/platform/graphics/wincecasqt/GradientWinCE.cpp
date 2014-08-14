/*
 * Copyright (C) 2009 Torch Mobile, Inc. All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */


#include "config.h"
#include "Gradient.h"

#include "GraphicsContext.h"

namespace WebCore {

void Gradient::platformDestroy()
{
}

static inline bool compareStops(const Gradient::ColorStop& a, const Gradient::ColorStop& b)
{
    return a.stop < b.stop;
}

const Vector<Gradient::ColorStop, 2>& Gradient::getStops() const
{
    if (!m_stopsSorted) {
        if (m_stops.size())
            std::stable_sort(m_stops.begin(), m_stops.end(), compareStops);
        m_stopsSorted = true;
    }
    return m_stops;
}

void Gradient::fill(GraphicsContext* c, const FloatRect& r)
{
    c->fillRect(r, this);
}

PassOwnPtr<HBRUSH> Gradient::createBrush(const FloatRect& rect) const
{
    if (m_lastBrushRect != rect) {
        m_lastBrushRect = rect;

        FloatPoint p0 = m_p0;
        FloatPoint p1 = m_p1;
        IntRect enclosingRect(enclosingIntRect(rect));
        IntSize bitmapSize(enclosingRect.size());

        if (p0.x() == p1.x())
            bitmapSize.setWidth(1);
        if (p0.y() == p1.y())
            bitmapSize.setHeight(1);

        void* pixels = 0;
        BitmapInfo bitmapInfo = BitmapInfo::createBottomUp(bitmapSize);
        m_lastBrushBitmap = adoptPtr(CreateDIBSection(0, &bitmapInfo, DIB_RGB_COLORS, &pixels, 0, 0));

        float gradientX = p1.x() - p0.x();
        float gradientY = p1.y() - p0.y();

        float gradientRadiant = atan2f(gradientX, gradientY);
        float gradientLength = sqrtf(gradientX * gradientX + gradientY * gradientY);

        int index = 0;
        unsigned char *data = reinterpret_cast<unsigned char*>(pixels);
        for (int i = 0; i < bitmapSize.height(); ++i) {
            for (int j = 0; j < bitmapSize.width(); ++j) {
                float x = gradientX ? (static_cast<float>(j) / (bitmapSize.width() - 1) - p0.x()) : 0;
                float y = gradientY ? (static_cast<float>(i) / (bitmapSize.height() - 1) - p0.y()) : 0;
                float value;

                if (!gradientX)
                    value = y / gradientLength;
                else if (!gradientY)
                    value = x / gradientLength;
                else
                    value = sqrtf(x * x + y * y) * cosf(atan2f(x, y) - gradientRadiant) / gradientLength;

                if (m_spreadMethod == SpreadMethodReflect) {
                    while (value < 0)
                        value += 2;
                    while (value > 2)
                        value -= 2;

                    if (value > 1)
                        value = 2 - value;
                } else if (m_spreadMethod == SpreadMethodRepeat) {
                    while (value < 0)
                        value += 1;
                    while (value > 1)
                        value -= 1;
                }

                float r, g, b, a;
                getColor(value, &r, &g, &b, &a);

                data[index + 0] = 0xff * a * b + 0.5;
                data[index + 1] = 0xff * a * g + 0.5;
                data[index + 2] = 0xff * a * r + 0.5;
                data[index + 3] = 0xff * a + 0.5;

                index += 4;
            }
        }
    }

    return CreatePatternBrush(m_lastBrushBitmap.get());
}

} // namespace WebCore
