/********************************************************
 *                                                       *
 *   Copyright (C) Microsoft. All rights reserved.       *
 *                                                       *
 ********************************************************/

#include "precomp.h"
#include "TextColor.h"

// Method Description:
// - Returns true if this attribute is a legacy style attribute - if it's an
///     index into the color table.
// Arguments:
// - <none>
// Return Value:
// - true if this color is a legacy style attribute
bool TextColor::IsLegacy() const noexcept
{
    return !(IsDefault() || IsRgb());
}

// Method Description:
// - Returns true if this attribute is a "default" attribute. It's up to the
//      terminal to decide how to interpret what a default attribute means.
// Arguments:
// - <none>
// Return Value:
// - true if this color is a default attribute
bool TextColor::IsDefault() const noexcept
{
    return _meta == static_cast<BYTE>(ColorType::IsDefault);
}

// Method Description:
// - Returns true if this attribute is a RGB attribute, containing the full
//      color information in itself.
// Arguments:
// - <none>
// Return Value:
// - true if this color is a RGB attrbute
bool TextColor::IsRgb() const noexcept
{
    return _meta == static_cast<BYTE>(ColorType::IsRgb);
}

// Method Description:
// - Sets the color value of this attribute, and sets this color to be an RGB
//      attribute.
// Arguments:
// - rgbColor: the COLORREF containing the color information for this TextColor
// Return Value:
// - <none>
void TextColor::SetColor(const COLORREF rgbColor)
{
    _meta = static_cast<BYTE>(ColorType::IsRgb);
    _red = GetRValue(rgbColor);
    _green = GetGValue(rgbColor);
    _blue = GetBValue(rgbColor);
}

// Method Description:
// - Sets this TextColor to be a legacy-style index into the color table.
// Arguments:
// - index: the index of the colortable we should use for this TextColor.
// Return Value:
// - <none>
void TextColor::SetIndex(const BYTE index)
{
    _meta = static_cast<BYTE>(ColorType::IsIndex);
    _index = index;
}

// Method Description:
// - Sets this TextColor to be a default text color, who's appearance is
//      controlled by the terminal's implementation of what a default color is.
// Arguments:
// - <none>
// Return Value:
// - <none>
void TextColor::SetDefault()
{
    _meta = static_cast<BYTE>(ColorType::IsDefault);
}

// Method Description:
// - Retrieve the real color value for this TextColor.
//   * If we're an RGB color, we'll use that value.
//   * If we're an indexed color table value, we'll use that index to look up
//     our value in the provided color table.
//     - If brighten is true, and the index is in the "dark" portion of the
//       color table (indicies [0,7]), then we'll look up the bright version of
//       this color (from indicies [8,15]). This should be true for
//       TextAttributes that are "Bold" and we're treating bold as bright
//       (which is the default behavior of most terminals.)
//   * If we're a default color, we'll return the default color provided.
// Arguments:
// - colorTable: The table of colors we should use to look up the value of a
//      legacy attribute from
// - defaultColor: The color value to use if we're a default attribute.
// - brighten: if true, we'll brighten a dark color table index.
// Return Value:
// - a COLORREF containing the real value of this TextColor.
COLORREF TextColor::GetColor(std::basic_string_view<COLORREF> colorTable,
                             const COLORREF defaultColor,
                             bool brighten) const
{
    if (IsDefault())
    {
        return defaultColor;
    }
    else if (IsRgb())
    {
        return _GetRGB();
    }
    else
    {
        FAIL_FAST_IF(colorTable.size() < _index);
        // If the color is already bright (it's in index [8,15] or it's a
        //       256color value [16,255], then boldness does nothing.
        if (brighten && _index < 8)
        {
            FAIL_FAST_IF(colorTable.size() < 16);
            FAIL_FAST_IF((size_t)(_index + 8) > (size_t)(colorTable.size()));
            return colorTable[_index + 8];
        }
        else
        {
            return colorTable[_index];
        }
    }
}

// Method Description:
// - Return a COLORREF containing our stored value. Will return garbage if this
//attribute is not a RGB attribute.
// Arguments:
// - <none>
// Return Value:
// - a COLORREF containing our stored value
COLORREF TextColor::_GetRGB() const
{
    return RGB(_red, _green, _blue);
}
