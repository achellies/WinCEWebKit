/*
 *  Copyright (C) 2006 George Staikos <staikos@kde.org>
 *  Copyright (C) 2006 Alexey Proskuryakov <ap@nypop.com>
 *  Copyright (C) 2007-2009 Torch Mobile, Inc.
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
 
#if defined(HAVE_CONFIG_H) && HAVE_CONFIG_H
#ifdef BUILDING_WITH_CMAKE
#include "cmakeconfig.h"
#else
#include "autotoolsconfig.h"
#endif
#endif 

#include "ce_unicode.h"

#include <wchar.h>


// Implemented with ICU code

typedef bool UBool;
#define FALSE    false
#define TRUE    true


enum {
    UTRIE_SHIFT=5,
    UTRIE_DATA_BLOCK_LENGTH=1<<UTRIE_SHIFT,
    UTRIE_MASK=UTRIE_DATA_BLOCK_LENGTH-1,
    UTRIE_LEAD_INDEX_DISP=0x2800>>UTRIE_SHIFT,
    UTRIE_INDEX_SHIFT=2,
    UTRIE_DATA_GRANULARITY=1<<UTRIE_INDEX_SHIFT,
    UTRIE_SURROGATE_BLOCK_BITS=10-UTRIE_SHIFT,
    UTRIE_SURROGATE_BLOCK_COUNT=(1<<UTRIE_SURROGATE_BLOCK_BITS),
    UTRIE_BMP_INDEX_LENGTH=0x10000>>UTRIE_SHIFT
};

typedef int32_t UTrieGetFoldingOffset(uint32_t data);

struct UTrie {
    const uint16_t *index;
    const uint32_t *data32; /* NULL if 16b data is used via index */

    /**
     * This function is not used in _FROM_LEAD, _FROM_BMP, and _FROM_OFFSET_TRAIL macros.
     * If convenience macros like _GET16 or _NEXT32 are used, this function must be set.
     *
     * utrie_unserialize() sets a default function which simply returns
     * the lead surrogate's value itself - which is the inverse of the default
     * folding function used by utrie_serialize().
     *
     * @see UTrieGetFoldingOffset
     */
    UTrieGetFoldingOffset *getFoldingOffset;

    int32_t indexLength, dataLength;
    uint32_t initialValue;
    UBool isLatin1Linear;
};

int32_t utrie_defaultGetFoldingOffset(uint32_t data) {
    return (int32_t)data;
}

#define _UTRIE_GET_RAW(trie, data, offset, c16) \
    (trie)->data[ \
        ((int32_t)((trie)->index[(offset)+((c16)>>UTRIE_SHIFT)])<<UTRIE_INDEX_SHIFT)+ \
        ((c16)&UTRIE_MASK) \
    ]

#define _UTRIE_GET_FROM_PAIR(trie, data, c, c2, result, resultType) { \
    int32_t __offset; \
\
    /* get data for lead surrogate */ \
    (result)=_UTRIE_GET_RAW((trie), data, 0, (c)); \
    __offset=(trie)->getFoldingOffset(result); \
\
    /* get the real data from the folded lead/trail units */ \
    if(__offset>0) { \
        (result)=_UTRIE_GET_RAW((trie), data, __offset, (c2)&0x3ff); \
    } else { \
        (result)=(resultType)((trie)->initialValue); \
    } \
}

#define _UTRIE_GET_FROM_BMP(trie, data, c16) \
    _UTRIE_GET_RAW(trie, data, 0xd800<=(c16) && (c16)<=0xdbff ? UTRIE_LEAD_INDEX_DISP : 0, c16);

#define _UTRIE_GET(trie, data, c32, result, resultType) \
    if((uint32_t)(c32)<=0xffff) { \
        /* BMP code points */ \
        (result)=_UTRIE_GET_FROM_BMP(trie, data, c32); \
    } else if((uint32_t)(c32)<=0x10ffff) { \
        /* supplementary code point */ \
        UChar __lead16=(UChar)(((c32)>>10)+0xd7c0); \
        _UTRIE_GET_FROM_PAIR(trie, data, __lead16, c32, result, resultType); \
    } else { \
        /* out of range */ \
        (result)=(resultType)((trie)->initialValue); \
    }

#define UTRIE_GET16(trie, c16, result) _UTRIE_GET(trie, index, c16, result, uint16_t)
#define UTRIE_GET32(trie, c32, result) _UTRIE_GET(trie, data32, c32, result, uint32_t)

#define GET_PROPS(c, result) UTRIE_GET16(&CharProp::propsTrie, c, result);

typedef uint8_t UVersionInfo[4];

enum {
    UPROPS_INDEX_COUNT=16,

    /* general category shift==0                                0 (5 bits) */
    UPROPS_NUMERIC_TYPE_SHIFT=5,                            /*  5 (3 bits) */
    UPROPS_NUMERIC_VALUE_SHIFT=8                            /*  8 (8 bits) */
};

namespace CharProp
{
#include "uchar_props_data.c"
}

#define GET_BIDI_PROPS(bdp, c, result) \
    UTRIE_GET16(&(bdp)->trie, c, result);

enum {
    UBIDI_IX_MIRROR_LENGTH = 3,

    UBIDI_MAX_VALUES_INDEX=15,
    UBIDI_IX_TOP=16,

    /* UBIDI_CLASS_SHIFT=0, */     /* bidi class: 5 bits (4..0) */
    UBIDI_JT_SHIFT=5,           /* joining type: 3 bits (7..5) */

    /* UBIDI__SHIFT=8, reserved: 2 bits (9..8) */

    UBIDI_JOIN_CONTROL_SHIFT=10,
    UBIDI_BIDI_CONTROL_SHIFT=11,

    UBIDI_IS_MIRRORED_SHIFT=12,         /* 'is mirrored' */
    UBIDI_MIRROR_DELTA_SHIFT=13,        /* bidi mirroring delta: 3 bits (15..13) */

    UBIDI_MAX_JG_SHIFT=16,              /* max JG value in indexes[UBIDI_MAX_VALUES_INDEX] bits 23..16 */

    UBIDI_ESC_MIRROR_DELTA=-4,
    UBIDI_MIN_MIRROR_DELTA=-3,
    UBIDI_MAX_MIRROR_DELTA=3,

    /* the source Unicode code point takes 21 bits (20..0) */
    UBIDI_MIRROR_INDEX_SHIFT=21,
    UBIDI_MAX_MIRROR_INDEX=0x7ff
};

#define UBIDI_GET_MIRROR_CODE_POINT(m) (UChar32)((m)&0x1fffff)
#define UBIDI_GET_MIRROR_INDEX(m) ((m)>>UBIDI_MIRROR_INDEX_SHIFT)

#define UBIDI_CLASS_MASK        0x0000001f
#define UBIDI_JT_MASK           0x000000e0
#define UBIDI_MAX_JG_MASK       0x00ff0000
#define UBIDI_GET_CLASS(props) ((props)&UBIDI_CLASS_MASK)
#define UBIDI_GET_FLAG(props, shift) (((props)>>(shift))&1)
#define UPROPS_DT_MASK          0x0000001f

namespace BidiProp
{
struct UBiDiProps {
    void *mem;
    const int32_t *indexes;
    const uint32_t *mirrors;
    const uint8_t *jgArray;

    UTrie trie;
    uint8_t formatVersion[4];
};

#include "ubidi_props_data.c"
}

#define _NORM_MIN_SPECIAL       0xfc000000
#define _NORM_SURROGATES_TOP    0xfff00000
#define _NORM_AUX_FNC_MASK          (uint32_t)(_NORM_AUX_MAX_FNC-1)
#define _NORM_AUX_MAX_FNC           ((int32_t)1<<_NORM_AUX_COMP_EX_SHIFT)

enum {
    _NORM_AUX_COMP_EX_SHIFT=10,
    _NORM_AUX_UNSAFE_SHIFT=11,
    _NORM_AUX_NFC_SKIPPABLE_F_SHIFT=12,

    _NORM_INDEX_TOP=32,                  /* changing this requires a new formatVersion */

    _NORM_CC_SHIFT=8,           /* UnicodeData.txt combining class in bits 15..8 */

    _NORM_EXTRA_SHIFT=16,               /* 16 bits for the index to UChars and other extra data */
};

namespace NormProp
{
    static int32_t getFoldingNormOffset(uint32_t norm32) {
        if (_NORM_MIN_SPECIAL<=norm32 && norm32<_NORM_SURROGATES_TOP) {
            return
                UTRIE_BMP_INDEX_LENGTH+
                    (((int32_t)norm32>>(_NORM_EXTRA_SHIFT-UTRIE_SURROGATE_BLOCK_BITS))&
                     (0x3ff<<UTRIE_SURROGATE_BLOCK_BITS));
        } else {
            return 0;
        }
    }

    static int32_t getFoldingAuxOffset(uint32_t data) {
        return (int32_t)(data&_NORM_AUX_FNC_MASK)<<UTRIE_SURROGATE_BLOCK_BITS;
    }

#include "unorm_props_data.c"
}

uint32_t u_getUnicodeProperties(UChar32 c, int32_t column) {
    uint16_t vecIndex;

    if (column == -1) {
        uint32_t props;
        GET_PROPS(c, props);
        return props;
    } else if (column < 0 || column >= CharProp::propsVectorsColumns) {
        return 0;
    } else {
        UTRIE_GET16(&CharProp::propsVectorsTrie, c, vecIndex);
        return CharProp::propsVectors[vecIndex+column];
    }
}

namespace WTF {
namespace UnicodeCE {

uint32_t direction(UChar32 c)
{
    uint32_t props;
    GET_BIDI_PROPS(&BidiProp::ubidi_props_singleton, c, props);
    return UBIDI_GET_CLASS(props);
}

uint32_t category(unsigned int c)
{
    uint32_t props;
    GET_PROPS(c, props);
    return props&0x1f;
}

uint32_t decompositionType(UChar32 c)
{
    return u_getUnicodeProperties(c, 2)&UPROPS_DT_MASK;
}

unsigned char combiningClass(UChar32 c)
{
    uint32_t norm32;

    UTRIE_GET32(&NormProp::normTrie, c, norm32);
    return (uint8_t)(norm32 >> _NORM_CC_SHIFT);
}

wchar_t mirroredChar(UChar32 c)
{
    const BidiProp::UBiDiProps *bdp = &BidiProp::ubidi_props_singleton;

    uint32_t props;
    int32_t delta;

    GET_BIDI_PROPS(bdp, c, props);
    delta = ((int16_t)props) >> UBIDI_MIRROR_DELTA_SHIFT;
    if (delta != UBIDI_ESC_MIRROR_DELTA) {
        return c+delta;
    } else {
        /* look for mirror code point in the mirrors[] table */
        const uint32_t *mirrors = bdp->mirrors;
        int32_t length = bdp->indexes[UBIDI_IX_MIRROR_LENGTH];

        /* linear search */
        for (int32_t i = 0; i < length; ++i) {
            uint32_t m = mirrors[i];
            UChar32 c2 = UBIDI_GET_MIRROR_CODE_POINT(m);
            if (c == c2) {
                /* found c, return its mirror code point using the index in m */
                return UBIDI_GET_MIRROR_CODE_POINT(mirrors[UBIDI_GET_MIRROR_INDEX(m)]);
            } else if (c < c2) {
                break;
            }
        }

        /* c not found, return it itself */
        return c;
    }
}

int digitValue(wchar_t c)
{
    uint32_t props;
    GET_PROPS(c, props);

    if (((props >> UPROPS_NUMERIC_TYPE_SHIFT) & 7) == 1) {
        return (props >> UPROPS_NUMERIC_VALUE_SHIFT) & 0xff;
    } else {
        return -1;
    }
}

} // namespace Unicode
} // namespace WTF
