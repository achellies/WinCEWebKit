/*
    This file is part of the WebKit open source project.
    This file has been generated by generate-bindings.pl. DO NOT MODIFY!

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef WEB_KIT_DOM_TEST_CALLBACK_PRIVATE_H
#define WEB_KIT_DOM_TEST_CALLBACK_PRIVATE_H

#include <glib-object.h>
#include <webkit/WebKitDOMObject.h>
#include "TestCallback.h"
namespace WebKit {
    WebKitDOMTestCallback *
    wrapTestCallback(WebCore::TestCallback *coreObject);

    WebCore::TestCallback *
    core(WebKitDOMTestCallback *request);

    gpointer
    kit(WebCore::TestCallback* node);

} // namespace WebKit

#endif /* WEB_KIT_DOM_TEST_CALLBACK_PRIVATE_H */
