#ifndef __KEY_CODES_H__
#define __KEY_CODES_H__

#include "../../common/Common.h"

#include <string>

namespace prev::input::keyboard {
enum class KeyCode {
    KEY_NONE = 0, // Undefined. (No event)
    KEY_A = 4,
    KEY_B = 5,
    KEY_C = 6,
    KEY_D = 7,
    KEY_E = 8,
    KEY_F = 9,
    KEY_G = 10,
    KEY_H = 11,
    KEY_I = 12,
    KEY_J = 13,
    KEY_K = 14,
    KEY_L = 15,
    KEY_M = 16,
    KEY_N = 17,
    KEY_O = 18,
    KEY_P = 19,
    KEY_Q = 20,
    KEY_R = 21,
    KEY_S = 22,
    KEY_T = 23,
    KEY_U = 24,
    KEY_V = 25,
    KEY_W = 26,
    KEY_X = 27,
    KEY_Y = 28,
    KEY_Z = 29,
    KEY_1 = 30, // 1 and !
    KEY_2 = 31, // 2 and @
    KEY_3 = 32, // 3 and #
    KEY_4 = 33, // 4 and $
    KEY_5 = 34, // 5 and %
    KEY_6 = 35, // 6 and ^
    KEY_7 = 36, // 7 and &
    KEY_8 = 37, // 8 and *
    KEY_9 = 38, // 9 and (
    KEY_0 = 39, // 0 and )
    KEY_Enter = 40, // (Return)
    KEY_Escape = 41,	
    KEY_Delete = 42,
    KEY_Tab = 43,
    KEY_Space = 44,
    KEY_Minus = 45, // - and (underscore)
    KEY_Equals = 46, // = and +
    KEY_LeftBracket = 47, // [ and {
    KEY_RightBracket = 48, // ] and }
    KEY_Backslash = 49, // \ and |
    // KEY_NonUSHash     = 50, // # and ~
    KEY_Semicolon = 51, // ; and :
    KEY_Quote = 52, // ' and "
    KEY_Grave = 53, // § and ~
    KEY_Comma = 54, // , and <
    KEY_Period = 55, // . and >
    KEY_Slash = 56, // / and ?
    KEY_CapsLock = 57,
    KEY_F1 = 58,
    KEY_F2 = 59,
    KEY_F3 = 60,
    KEY_F4 = 61,
    KEY_F5 = 62,
    KEY_F6 = 63,
    KEY_F7 = 64,
    KEY_F8 = 65,
    KEY_F9 = 66,
    KEY_F10 = 67,
    KEY_F11 = 68,
    KEY_F12 = 69,
    KEY_PrintScreen = 70,
    KEY_ScrollLock = 71,
    KEY_Pause = 72,
    KEY_Insert = 73,
    KEY_Home = 74,
    KEY_PageUp = 75,
    KEY_DeleteForward = 76,
    KEY_End = 77,
    KEY_PageDown = 78,
    KEY_Right = 79, // Right arrow
    KEY_Left = 80, // Left arrow
    KEY_Down = 81, // Down arrow
    KEY_Up = 82, // Up arrow
    KP_NumLock = 83,
    KP_Divide = 84,
    KP_Multiply = 85,
    KP_Subtract = 86,
    KP_Add = 87,
    KP_Enter = 88,
    KP_1 = 89,
    KP_2 = 90,
    KP_3 = 91,
    KP_4 = 92,
    KP_5 = 93,
    KP_6 = 94,
    KP_7 = 95,
    KP_8 = 96,
    KP_9 = 97,
    KP_0 = 98,
    KP_Point = 99, // . and Del
    KP_Equals = 103,
    KEY_F13 = 104,
    KEY_F14 = 105,
    KEY_F15 = 106,
    KEY_F16 = 107,
    KEY_F17 = 108,
    KEY_F18 = 109,
    KEY_F19 = 110,
    KEY_F20 = 111,
    KEY_F21 = 112,
    KEY_F22 = 113,
    KEY_F23 = 114,
    KEY_F24 = 115,
    // KEY_Help          = 117,
    KEY_Menu = 118,
    KEY_Mute = 127,
    KEY_VolumeUp = 128,
    KEY_VolumeDown = 129,
    KEY_LeftControl = 224, // WARNING : Android has no Ctrl keys.
    KEY_LeftShift = 225,
    KEY_LeftAlt = 226,
    KEY_LeftGUI = 227,
    KEY_RightControl = 228,
    KEY_RightShift = 229, // WARNING : Win32 fails to send a WM_KEYUP message if both shift keys are pressed, and one released.
    KEY_RightAlt = 230,
    KEY_RightGUI = 231
};

inline std::string KeyCodeToString(const KeyCode code)
{
    switch (code) {
    case KeyCode::KEY_A:
        return STRINGIFY(KeyCode::KEY_A);
    case KeyCode::KEY_B:
        return STRINGIFY(KeyCode::KEY_B);
    case KeyCode::KEY_C:
        return STRINGIFY(KeyCode::KEY_C);
    case KeyCode::KEY_D:
        return STRINGIFY(KeyCode::KEY_D);
    case KeyCode::KEY_E:
        return STRINGIFY(KeyCode::KEY_E);
    case KeyCode::KEY_F:
        return STRINGIFY(KeyCode::KEY_F);
    case KeyCode::KEY_G:
        return STRINGIFY(KeyCode::KEY_G);
    case KeyCode::KEY_H:
        return STRINGIFY(KeyCode::KEY_H);
    case KeyCode::KEY_I:
        return STRINGIFY(KeyCode::KEY_I);
    case KeyCode::KEY_J:
        return STRINGIFY(KeyCode::KEY_J);
    case KeyCode::KEY_K:
        return STRINGIFY(KeyCode::KEY_K);
    case KeyCode::KEY_L:
        return STRINGIFY(KeyCode::KEY_L);
    case KeyCode::KEY_M:
        return STRINGIFY(KeyCode::KEY_M);
    case KeyCode::KEY_N:
        return STRINGIFY(KeyCode::KEY_N);
    case KeyCode::KEY_O:
        return STRINGIFY(KeyCode::KEY_O);
    case KeyCode::KEY_P:
        return STRINGIFY(KeyCode::KEY_P);
    case KeyCode::KEY_Q:
        return STRINGIFY(KeyCode::KEY_Q);
    case KeyCode::KEY_R:
        return STRINGIFY(KeyCode::KEY_R);
    case KeyCode::KEY_S:
        return STRINGIFY(KeyCode::KEY_S);
    case KeyCode::KEY_T:
        return STRINGIFY(KeyCode::KEY_T);
    case KeyCode::KEY_U:
        return STRINGIFY(KeyCode::KEY_U);
    case KeyCode::KEY_V:
        return STRINGIFY(KeyCode::KEY_V);
    case KeyCode::KEY_W:
        return STRINGIFY(KeyCode::KEY_W);
    case KeyCode::KEY_X:
        return STRINGIFY(KeyCode::KEY_X);
    case KeyCode::KEY_Y:
        return STRINGIFY(KeyCode::KEY_Y);
    case KeyCode::KEY_Z:
        return STRINGIFY(KeyCode::KEY_Z);
    case KeyCode::KEY_1: // 1 and !
        return STRINGIFY(KeyCode::KEY_1);
    case KeyCode::KEY_2: // 2 and @
        return STRINGIFY(KeyCode::KEY_2);
    case KeyCode::KEY_3: // 3 and #
        return STRINGIFY(KeyCode::KEY_3);
    case KeyCode::KEY_4: // 4 and $
        return STRINGIFY(KeyCode::KEY_4);
    case KeyCode::KEY_5: // 5 and %
        return STRINGIFY(KeyCode::KEY_5);
    case KeyCode::KEY_6: // 6 and ^
        return STRINGIFY(KeyCode::KEY_6);
    case KeyCode::KEY_7: // 7 and &
        return STRINGIFY(KeyCode::KEY_7);
    case KeyCode::KEY_8: // 8 and *
        return STRINGIFY(KeyCode::KEY_8);
    case KeyCode::KEY_9: // 9 and (
        return STRINGIFY(KeyCode::KEY_9);
    case KeyCode::KEY_0: // 0 and )
        return STRINGIFY(KeyCode::KEY_0);
    case KeyCode::KEY_Enter: // (Return)
        return STRINGIFY(KeyCode::KEY_Enter);
    case KeyCode::KEY_Escape:
        return STRINGIFY(KeyCode::KEY_Escape);
    case KeyCode::KEY_Delete:
        return STRINGIFY(KeyCode::KEY_Delete);
    case KeyCode::KEY_Tab:
        return STRINGIFY(KeyCode::KEY_Tab);
    case KeyCode::KEY_Space:
        return STRINGIFY(KeyCode::KEY_Space);
    case KeyCode::KEY_Minus: // - and (underscore)
        return STRINGIFY(KeyCode::KEY_Minus);
    case KeyCode::KEY_Equals: // = and +
        return STRINGIFY(KeyCode::KEY_Equals);
    case KeyCode::KEY_LeftBracket: // [ and {
        return STRINGIFY(KeyCode::KEY_LeftBracket);
    case KeyCode::KEY_RightBracket: // ] and }
        return STRINGIFY(KeyCode::KEY_RightBracket);
    case KeyCode::KEY_Backslash: // \ and |
        return STRINGIFY(KeyCode::KEY_Backslash);
    // case KeyCode::KEY_NonUSHash: // # and ~
    //     return STRINGIFY(KeyCode::KEY_NonUSHash);
    case KeyCode::KEY_Semicolon: // ; and :
        return STRINGIFY(KeyCode::KEY_Semicolon);
    case KeyCode::KEY_Quote: // ' and "
        return STRINGIFY(KeyCode::KEY_Quote);
    case KeyCode::KEY_Grave:
        return STRINGIFY(KeyCode::KEY_Grave);
    case KeyCode::KEY_Comma: // : and <
        return STRINGIFY(KeyCode::KEY_Comma);
    case KeyCode::KEY_Period: // . and >
        return STRINGIFY(KeyCode::KEY_Period);
    case KeyCode::KEY_Slash: // / and ?
        return STRINGIFY(KeyCode::KEY_Slash);
    case KeyCode::KEY_CapsLock:
        return STRINGIFY(KeyCode::KEY_CapsLock);
    case KeyCode::KEY_F1:
        return STRINGIFY(KeyCode::KEY_F1);
    case KeyCode::KEY_F2:
        return STRINGIFY(KeyCode::KEY_F2);
    case KeyCode::KEY_F3:
        return STRINGIFY(KeyCode::KEY_F3);
    case KeyCode::KEY_F4:
        return STRINGIFY(KeyCode::KEY_F4);
    case KeyCode::KEY_F5:
        return STRINGIFY(KeyCode::KEY_F5);
    case KeyCode::KEY_F6:
        return STRINGIFY(KeyCode::KEY_F6);
    case KeyCode::KEY_F7:
        return STRINGIFY(KeyCode::KEY_F7);
    case KeyCode::KEY_F8:
        return STRINGIFY(KeyCode::KEY_F8);
    case KeyCode::KEY_F9:
        return STRINGIFY(KeyCode::KEY_F9);
    case KeyCode::KEY_F10:
        return STRINGIFY(KeyCode::KEY_F10);
    case KeyCode::KEY_F11:
        return STRINGIFY(KeyCode::KEY_F11);
    case KeyCode::KEY_F12:
        return STRINGIFY(KeyCode::KEY_F12);
    case KeyCode::KEY_PrintScreen:
        return STRINGIFY(KeyCode::KEY_PrintScreen);
    case KeyCode::KEY_ScrollLock:
        return STRINGIFY(KeyCode::KEY_ScrollLock);
    case KeyCode::KEY_Pause:
        return STRINGIFY(KeyCode::KEY_Pause);
    case KeyCode::KEY_Insert:
        return STRINGIFY(KeyCode::KEY_Insert);
    case KeyCode::KEY_Home:
        return STRINGIFY(KeyCode::KEY_Home);
    case KeyCode::KEY_PageUp:
        return STRINGIFY(KeyCode::KEY_PageUp);
    case KeyCode::KEY_DeleteForward:
        return STRINGIFY(KeyCode::KEY_DeleteForward);
    case KeyCode::KEY_End:
        return STRINGIFY(KeyCode::KEY_End);
    case KeyCode::KEY_PageDown:
        return STRINGIFY(KeyCode::KEY_PageDown);
    case KeyCode::KEY_Right: // Right arrow
        return STRINGIFY(KeyCode::KEY_Right);
    case KeyCode::KEY_Left: // Left arrow
        return STRINGIFY(KeyCode::KEY_Left);
    case KeyCode::KEY_Down: // Down arrow
        return STRINGIFY(KeyCode::KEY_Down);
    case KeyCode::KEY_Up: // Up arrow
        return STRINGIFY(KeyCode::KEY_Up);
    case KeyCode::KP_NumLock:
        return STRINGIFY(KeyCode::KP_NumLock);
    case KeyCode::KP_Divide:
        return STRINGIFY(KeyCode::KP_Divide);
    case KeyCode::KP_Multiply:
        return STRINGIFY(KeyCode::KP_Multiply);
    case KeyCode::KP_Subtract:
        return STRINGIFY(KeyCode::KP_Subtract);
    case KeyCode::KP_Add:
        return STRINGIFY(KeyCode::KP_Add);
    case KeyCode::KP_Enter:
        return STRINGIFY(KeyCode::KP_Enter);
    case KeyCode::KP_1:
        return STRINGIFY(KeyCode::KP_1);
    case KeyCode::KP_2:
        return STRINGIFY(KeyCode::KP_2);
    case KeyCode::KP_3:
        return STRINGIFY(KeyCode::KP_3);
    case KeyCode::KP_4:
        return STRINGIFY(KeyCode::KP_4);
    case KeyCode::KP_5:
        return STRINGIFY(KeyCode::KP_5);
    case KeyCode::KP_6:
        return STRINGIFY(KeyCode::KP_6);
    case KeyCode::KP_7:
        return STRINGIFY(KeyCode::KP_7);
    case KeyCode::KP_8:
        return STRINGIFY(KeyCode::KP_8);
    case KeyCode::KP_9:
        return STRINGIFY(KeyCode::KP_9);
    case KeyCode::KP_0:
        return STRINGIFY(KeyCode::KP_0);
    case KeyCode::KP_Point: // . and Del
        return STRINGIFY(KeyCode::KP_Point);
    case KeyCode::KP_Equals:
        return STRINGIFY(KeyCode::KP_Equals);
    case KeyCode::KEY_F13:
        return STRINGIFY(KeyCode::KEY_F13);
    case KeyCode::KEY_F14:
        return STRINGIFY(KeyCode::KEY_F14);
    case KeyCode::KEY_F15:
        return STRINGIFY(KeyCode::KEY_F15);
    case KeyCode::KEY_F16:
        return STRINGIFY(KeyCode::KEY_F16);
    case KeyCode::KEY_F17:
        return STRINGIFY(KeyCode::KEY_F17);
    case KeyCode::KEY_F18:
        return STRINGIFY(KeyCode::KEY_F18);
    case KeyCode::KEY_F19:
        return STRINGIFY(KeyCode::KEY_F19);
    case KeyCode::KEY_F20:
        return STRINGIFY(KeyCode::KEY_F20);
    case KeyCode::KEY_F21:
        return STRINGIFY(KeyCode::KEY_F21);
    case KeyCode::KEY_F22:
        return STRINGIFY(KeyCode::KEY_F22);
    case KeyCode::KEY_F23:
        return STRINGIFY(KeyCode::KEY_F23);
    case KeyCode::KEY_F24:
        return STRINGIFY(KeyCode::KEY_F24);
    // case KeyCode::KEY_Help:
    //     return STRINGIFY(KeyCode::KEY_Help);
    case KeyCode::KEY_Menu:
        return STRINGIFY(KeyCode::KEY_Menu);
    case KeyCode::KEY_Mute:
        return STRINGIFY(KeyCode::KEY_Mute);
    case KeyCode::KEY_VolumeUp:
        return STRINGIFY(KeyCode::KEY_VolumeUp);
    case KeyCode::KEY_VolumeDown:
        return STRINGIFY(KeyCode::KEY_VolumeDown);
    case KeyCode::KEY_LeftControl: // WARNING : Android has no Ctrl keys.
        return STRINGIFY(KeyCode::KEY_LeftControl);
    case KeyCode::KEY_LeftShift:
        return STRINGIFY(KeyCode::KEY_LeftShift);
    case KeyCode::KEY_LeftAlt:
        return STRINGIFY(KeyCode::KEY_LeftAlt);
    case KeyCode::KEY_LeftGUI:
        return STRINGIFY(KeyCode::KEY_LeftGUI);
    case KeyCode::KEY_RightControl:
        return STRINGIFY(KeyCode::KEY_RightControl);
    case KeyCode::KEY_RightShift: // WARNING : Win32 fails to send a WM_KEYUP message if both shift keys are pressed: and one released.
        return STRINGIFY(KeyCode::KEY_RightShift);
    case KeyCode::KEY_RightAlt:
        return STRINGIFY(KeyCode::KEY_RightAlt);
    case KeyCode::KEY_RightGUI:
        return STRINGIFY(KeyCode::KEY_RightGUI);
    default:
        return STRINGIFY(KeyCode::KEY_NONE);
    }
}
} // namespace prev::input::keyboard
#endif
