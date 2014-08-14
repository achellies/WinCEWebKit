/*
 * Copyright (C) 2004, 2005, 2006, 2007, 2008 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005 Rob Buis <buis@kde.org>
 * Copyright (C) Research In Motion Limited 2010. All rights reserved.
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

#ifndef SVGAnimatedPropertyMacros_h
#define SVGAnimatedPropertyMacros_h

#if ENABLE(SVG)
#include "SVGAnimatedListPropertyTearOff.h"
#include "SVGAnimatedStaticPropertyTearOff.h"
#include "SVGAnimatedPropertySynchronizer.h"
#include "SVGAnimatedPropertyTearOff.h"
#include "SVGAnimatedTransformListPropertyTearOff.h"
#include "SVGNames.h" // FIXME: Temporary hack, until we expand the macros in all files, so we don't need a global SVGNames.h include
#include "SVGPropertyTraits.h"

namespace WebCore {

class SVGElement;

// GetOwnerElementForType implementation
template<typename OwnerType, bool isDerivedFromSVGElement>
struct GetOwnerElementForType;

template<typename OwnerType>
struct GetOwnerElementForType<OwnerType, true> {
    static SVGElement* ownerElement(OwnerType* type)
    {
        return type;
    }
};

template<typename OwnerType>
struct GetOwnerElementForType<OwnerType, false> {    
    static SVGElement* ownerElement(OwnerType* type)
    {
        SVGElement* context = type->contextElement();
        ASSERT(context);
        return context;
    }
};

// IsDerivedFromSVGElement implementation
template<typename OwnerType>
struct IsDerivedFromSVGElement {
    static const bool value = true;
};

class SVGViewSpec;
template<>
struct IsDerivedFromSVGElement<SVGViewSpec> {
    static const bool value = false;
};

template<typename PropertyType>
struct SVGSynchronizableAnimatedProperty {
    SVGSynchronizableAnimatedProperty()
        : value(SVGPropertyTraits<PropertyType>::initialValue())
        , shouldSynchronize(false)
    {
    }

    template<typename ConstructorParameter1>
    SVGSynchronizableAnimatedProperty(const ConstructorParameter1& value1)
        : value(value1)
        , shouldSynchronize(false)
    {
    }

    template<typename ConstructorParameter1, typename ConstructorParameter2>
    SVGSynchronizableAnimatedProperty(const ConstructorParameter1& value1, const ConstructorParameter2& value2)
        : value(value1, value2)
        , shouldSynchronize(false)
    {
    }

    PropertyType value;
    bool shouldSynchronize : 1;
};

// FIXME: These macros should be removed, after the transition to the new SVGAnimatedProperty concept is finished.
#define DECLARE_ANIMATED_PROPERTY_NEW_SHARED(OwnerType, DOMAttribute, SVGDOMAttributeIdentifier, TearOffType, PropertyType, UpperProperty, LowerProperty) \
public: \
PropertyType& LowerProperty() const \
{ \
    return m_##LowerProperty.value; \
} \
\
PropertyType& LowerProperty##BaseValue() const \
{ \
    return m_##LowerProperty.value; \
} \
\
void set##UpperProperty##BaseValue(const PropertyType& type) \
{ \
    m_##LowerProperty.value = type; \
} \
\
void synchronize##UpperProperty() \
{ \
    if (!m_##LowerProperty.shouldSynchronize) \
         return; \
    AtomicString value(SVGPropertyTraits<PropertyType>::toString(LowerProperty##BaseValue())); \
    SVGElement* contextElement = GetOwnerElementForType<OwnerType, IsDerivedFromSVGElement<OwnerType>::value>::ownerElement(this); \
    SVGAnimatedPropertySynchronizer<IsDerivedFromSVGElement<OwnerType>::value>::synchronize(contextElement, DOMAttribute, value); \
} \
\
PassRefPtr<TearOffType> LowerProperty##Animated() \
{ \
    m_##LowerProperty.shouldSynchronize = true; \
    SVGElement* contextElement = GetOwnerElementForType<OwnerType, IsDerivedFromSVGElement<OwnerType>::value>::ownerElement(this); \
    return SVGAnimatedProperty::lookupOrCreateWrapper<TearOffType, PropertyType>(contextElement, DOMAttribute, SVGDOMAttributeIdentifier, m_##LowerProperty.value); \
} \
private: \
    mutable SVGSynchronizableAnimatedProperty<PropertyType> m_##LowerProperty;

#define DECLARE_ANIMATED_PROPERTY_NEW(OwnerType, DOMAttribute, PropertyType, UpperProperty, LowerProperty) \
DECLARE_ANIMATED_PROPERTY_NEW_SHARED(OwnerType, DOMAttribute, DOMAttribute.localName(), SVGAnimatedPropertyTearOff<PropertyType>, PropertyType, UpperProperty, LowerProperty)

#define DECLARE_ANIMATED_PROPERTY_MULTIPLE_WRAPPERS_NEW(OwnerType, DOMAttribute, SVGDOMAttributeIdentifier, PropertyType, UpperProperty, LowerProperty) \
DECLARE_ANIMATED_PROPERTY_NEW_SHARED(OwnerType, DOMAttribute, SVGDOMAttributeIdentifier, SVGAnimatedPropertyTearOff<PropertyType>, PropertyType, UpperProperty, LowerProperty)

#define DECLARE_ANIMATED_STATIC_PROPERTY_MULTIPLE_WRAPPERS_NEW(OwnerType, DOMAttribute, SVGDOMAttributeIdentifier, PropertyType, UpperProperty, LowerProperty) \
DECLARE_ANIMATED_PROPERTY_NEW_SHARED(OwnerType, DOMAttribute, SVGDOMAttributeIdentifier, SVGAnimatedStaticPropertyTearOff<PropertyType>, PropertyType, UpperProperty, LowerProperty)

#define DECLARE_ANIMATED_STATIC_PROPERTY_NEW(OwnerType, DOMAttribute, PropertyType, UpperProperty, LowerProperty) \
DECLARE_ANIMATED_PROPERTY_NEW_SHARED(OwnerType, DOMAttribute, DOMAttribute.localName(), SVGAnimatedStaticPropertyTearOff<PropertyType>, PropertyType, UpperProperty, LowerProperty)

#define DECLARE_ANIMATED_LIST_PROPERTY_NEW(OwnerType, DOMAttribute, PropertyType, UpperProperty, LowerProperty) \
DECLARE_ANIMATED_PROPERTY_NEW_SHARED(OwnerType, DOMAttribute, DOMAttribute.localName(), SVGAnimatedListPropertyTearOff<PropertyType>, PropertyType, UpperProperty, LowerProperty) \
\
void detachAnimated##UpperProperty##ListWrappers(unsigned newListSize) \
{ \
    SVGElement* contextElement = GetOwnerElementForType<OwnerType, IsDerivedFromSVGElement<OwnerType>::value>::ownerElement(this); \
    SVGAnimatedProperty* wrapper = SVGAnimatedProperty::lookupWrapper<SVGAnimatedListPropertyTearOff<PropertyType> >(contextElement, DOMAttribute.localName()); \
    if (!wrapper) \
        return; \
    static_cast<SVGAnimatedListPropertyTearOff<PropertyType>*>(wrapper)->detachListWrappers(newListSize); \
}

#define DECLARE_ANIMATED_TRANSFORM_LIST_PROPERTY_NEW(OwnerType, DOMAttribute, PropertyType, UpperProperty, LowerProperty) \
DECLARE_ANIMATED_PROPERTY_NEW_SHARED(OwnerType, DOMAttribute, DOMAttribute.localName(), SVGAnimatedTransformListPropertyTearOff, PropertyType, UpperProperty, LowerProperty) \
\
void detachAnimated##UpperProperty##ListWrappers(unsigned newListSize) \
{ \
    SVGElement* contextElement = GetOwnerElementForType<OwnerType, IsDerivedFromSVGElement<OwnerType>::value>::ownerElement(this); \
    SVGAnimatedProperty* wrapper = SVGAnimatedProperty::lookupWrapper<SVGAnimatedTransformListPropertyTearOff>(contextElement, DOMAttribute.localName()); \
    if (!wrapper) \
        return; \
    static_cast<SVGAnimatedTransformListPropertyTearOff*>(wrapper)->detachListWrappers(newListSize); \
}


}

#endif // ENABLE(SVG)
#endif // SVGAnimatedPropertyMacros_h
