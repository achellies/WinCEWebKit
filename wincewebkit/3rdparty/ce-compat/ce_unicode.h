#ifndef UNICODE_WINCE_H__
#define UNICODE_WINCE_H__

#include <stdint.h>
typedef wchar_t UChar;
typedef unsigned __int32 UChar32;

namespace WTF {

    namespace UnicodeCE {

        enum IcuCharDirection {
            U_LEFT_TO_RIGHT               = 0,
            U_RIGHT_TO_LEFT               = 1,
            U_EUROPEAN_NUMBER             = 2,
            U_EUROPEAN_NUMBER_SEPARATOR   = 3,
            U_EUROPEAN_NUMBER_TERMINATOR  = 4,
            U_ARABIC_NUMBER               = 5,
            U_COMMON_NUMBER_SEPARATOR     = 6,
            U_BLOCK_SEPARATOR             = 7,
            U_SEGMENT_SEPARATOR           = 8,
            U_WHITE_SPACE_NEUTRAL         = 9,
            U_OTHER_NEUTRAL               = 10,
            U_LEFT_TO_RIGHT_EMBEDDING     = 11,
            U_LEFT_TO_RIGHT_OVERRIDE      = 12,
            U_RIGHT_TO_LEFT_ARABIC        = 13,
            U_RIGHT_TO_LEFT_EMBEDDING     = 14,
            U_RIGHT_TO_LEFT_OVERRIDE      = 15,
            U_POP_DIRECTIONAL_FORMAT      = 16,
            U_DIR_NON_SPACING_MARK        = 17,
            U_BOUNDARY_NEUTRAL            = 18,
            U_CHAR_DIRECTION_COUNT
        };
        enum IcuDecompositionType {
            U_DT_NONE,              /*[none]*/ /*See note !!*/
            U_DT_CANONICAL,         /*[can]*/
            U_DT_COMPAT,            /*[com]*/
            U_DT_CIRCLE,            /*[enc]*/
            U_DT_FINAL,             /*[fin]*/
            U_DT_FONT,              /*[font]*/
            U_DT_FRACTION,          /*[fra]*/
            U_DT_INITIAL,           /*[init]*/
            U_DT_ISOLATED,          /*[iso]*/
            U_DT_MEDIAL,            /*[med]*/
            U_DT_NARROW,            /*[nar]*/
            U_DT_NOBREAK,           /*[nb]*/
            U_DT_SMALL,             /*[sml]*/
            U_DT_SQUARE,            /*[sqr]*/
            U_DT_SUB,               /*[sub]*/
            U_DT_SUPER,             /*[sup]*/
            U_DT_VERTICAL,          /*[vert]*/
            U_DT_WIDE,              /*[wide]*/
            U_DT_COUNT /* 18 */
        };
        enum IcuCharCategory
        {
            U_UNASSIGNED              = 0,
            U_GENERAL_OTHER_TYPES     = 0,
            U_UPPERCASE_LETTER        = 1,
            U_LOWERCASE_LETTER        = 2,
            U_TITLECASE_LETTER        = 3,
            U_MODIFIER_LETTER         = 4,
            U_OTHER_LETTER            = 5,
            U_NON_SPACING_MARK        = 6,
            U_ENCLOSING_MARK          = 7,
            U_COMBINING_SPACING_MARK  = 8,
            U_DECIMAL_DIGIT_NUMBER    = 9,
            U_LETTER_NUMBER           = 10,
            U_OTHER_NUMBER            = 11,
            U_SPACE_SEPARATOR         = 12,
            U_LINE_SEPARATOR          = 13,
            U_PARAGRAPH_SEPARATOR     = 14,
            U_CONTROL_CHAR            = 15,
            U_FORMAT_CHAR             = 16,
            U_PRIVATE_USE_CHAR        = 17,
            U_SURROGATE               = 18,
            U_DASH_PUNCTUATION        = 19,
            U_START_PUNCTUATION       = 20,
            U_END_PUNCTUATION         = 21,
            U_CONNECTOR_PUNCTUATION   = 22,
            U_OTHER_PUNCTUATION       = 23,
            U_MATH_SYMBOL             = 24,
            U_CURRENCY_SYMBOL         = 25,
            U_MODIFIER_SYMBOL         = 26,
            U_OTHER_SYMBOL            = 27,
            U_INITIAL_PUNCTUATION     = 28,
            U_FINAL_PUNCTUATION       = 29,
            U_CHAR_CATEGORY_COUNT
        };

        uint32_t direction(UChar32 c);
        uint32_t category(unsigned int c);
        uint32_t decompositionType(UChar32 c);
        unsigned char combiningClass(UChar32 c);
        wchar_t mirroredChar(UChar32 c);
        int digitValue(wchar_t c);
    }
}

#endif
