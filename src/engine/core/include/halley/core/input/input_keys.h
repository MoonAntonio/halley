/*****************************************************************\
           __
          / /
		 / /                     __  __
		/ /______    _______    / / / / ________   __       __
	   / ______  \  /_____  \  / / / / / _____  | / /      / /
	  / /      | / _______| / / / / / / /____/ / / /      / /
	 / /      / / / _____  / / / / / / _______/ / /      / /
	/ /      / / / /____/ / / / / / / |______  / |______/ /
   /_/      /_/ |________/ / / / /  \_______/  \_______  /
                          /_/ /_/                     / /
			                                         / /
		       High Level Game Framework            /_/

  ---------------------------------------------------------------

  Copyright (c) 2007-2011 - Rodrigo Braz Monteiro.
  This file is subject to the terms of halley_license.txt.

\*****************************************************************/

#pragma once

#include <halley/text/halleystring.h>
#include <halley/data_structures/flat_map.h>

namespace Halley {
	class Keys {
	public:
		enum Key {
			// SDL key bindings
			Unknown = 0,

			A = 4,
			B = 5,
			C = 6,
			D = 7,
			E = 8,
			F = 9,
			G = 10,
			H = 11,
			I = 12,
			J = 13,
			K = 14,
			L = 15,
			M = 16,
			N = 17,
			O = 18,
			P = 19,
			Q = 20,
			R = 21,
			S = 22,
			T = 23,
			U = 24,
			V = 25,
			W = 26,
			X = 27,
			Y = 28,
			Z = 29,

			Num1 = 30,
			Num2 = 31,
			Num3 = 32,
			Num4 = 33,
			Num5 = 34,
			Num6 = 35,
			Num7 = 36,
			Num8 = 37,
			Num9 = 38,
			Num0 = 39,

			Return = 40,
			Enter = 40,
			Escape = 41,
			Esc = 41,
			Backspace = 42,
			Tab = 43,
			Space = 44,

			Minus = 45,
			Equals = 46,
			LeftBracket = 47,
			RightBracket = 48,
			Backslash = 49,
			NonUSHash = 50,
			Semicolon = 51,
			Apostrophe = 52,
			Grave = 53,
			Comma = 54,
			Period = 55,
			Slash = 56,

			CapsLock = 57,

			F1 = 58,
			F2 = 59,
			F3 = 60,
			F4 = 61,
			F5 = 62,
			F6 = 63,
			F7 = 64,
			F8 = 65,
			F9 = 66,
			F10 = 67,
			F11 = 68,
			F12 = 69,

			Printscreen = 70,
			ScrollLock = 71,
			Pause = 72,
			Insert = 73,
			Home = 74,
			PageUp = 75,
			Delete = 76,
			End = 77,
			PageDown = 78,
			Right = 79,
			Left = 80,
			Down = 81,
			Up = 82,

			NumLockClear = 83,
			KP_Divide = 84,
			KP_Multiply = 85,
			KP_Minus = 86,
			KP_Plus = 87,
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
			KP_Period = 99,

			NonUSBackslash = 100,
			Application = 101,
			Power = 102,
			KP_Equals = 103,
			F13 = 104,
			F14 = 105,
			F15 = 106,
			F16 = 107,
			F17 = 108,
			F18 = 109,
			F19 = 110,
			F20 = 111,
			F21 = 112,
			F22 = 113,
			F23 = 114,
			F24 = 115,
			Execute = 116,
			Help = 117,
			Menu = 118,
			Select = 119,
			Stop = 120,
			Again = 121,
			Undo = 122,
			Cut = 123,
			Copy = 124,
			Paste = 125,
			Find = 126,
			Mute = 127,
			Volumeup = 128,
			Volumedown = 129,
			KP_Comma = 133,
			KP_Equalsas400 = 134,

			International1 = 135,
			International2 = 136,
			International3 = 137,
			International4 = 138,
			International5 = 139,
			International6 = 140,
			International7 = 141,
			International8 = 142,
			International9 = 143,
			Lang1 = 144,
			Lang2 = 145,
			Lang3 = 146,
			Lang4 = 147,
			Lang5 = 148,
			Lang6 = 149,
			Lang7 = 150,
			Lang8 = 151,
			Lang9 = 152,

			Alterase = 153,
			Sysreq = 154,
			Cancel = 155,
			Clear = 156,
			Prior = 157,
			Return2 = 158,
			Separator = 159,
			Out = 160,
			Oper = 161,
			Clearagain = 162,
			Crsel = 163,
			Exsel = 164,

			KP_00 = 176,
			KP_000 = 177,
			Thousandsseparator = 178,
			Decimalseparator = 179,
			Currencyunit = 180,
			Currencysubunit = 181,
			KP_Leftparen = 182,
			KP_Rightparen = 183,
			KP_Leftbrace = 184,
			KP_Rightbrace = 185,
			KP_Tab = 186,
			KP_Backspace = 187,
			KP_A = 188,
			KP_B = 189,
			KP_C = 190,
			KP_D = 191,
			KP_E = 192,
			KP_F = 193,
			KP_Xor = 194,
			KP_Power = 195,
			KP_Percent = 196,
			KP_Less = 197,
			KP_Greater = 198,
			KP_Ampersand = 199,
			KP_Dblampersand = 200,
			KP_Verticalbar = 201,
			KP_Dblverticalbar = 202,
			KP_Colon = 203,
			KP_Hash = 204,
			KP_Space = 205,
			KP_At = 206,
			KP_Exclam = 207,
			KP_Memstore = 208,
			KP_Memrecall = 209,
			KP_Memclear = 210,
			KP_Memadd = 211,
			KP_Memsubtract = 212,
			KP_Memmultiply = 213,
			KP_Memdivide = 214,
			KP_Plusminus = 215,
			KP_Clear = 216,
			KP_Clearentry = 217,
			KP_Binary = 218,
			KP_Octal = 219,
			KP_Decimal = 220,
			KP_Hexadecimal = 221,

			LCtrl = 224,
			LShift = 225,
			LAlt = 226,
			LGui = 227,
			RCtrl = 228,
			RShift = 229,
			RAlt = 230,
			RGui = 231,

			Mode = 257,

			AudioNext = 258,
			AudioPrev = 259,
			AudioStop = 260,
			AudioPlay = 261,
			AudioMute = 262,
			MediaSelect = 263,
			WWW = 264,
			Mail = 265,
			Calculator = 266,
			Computer = 267,
			Ac_Search = 268,
			Ac_Home = 269,
			Ac_Back = 270,
			Ac_Forward = 271,
			Ac_Stop = 272,
			Ac_Refresh = 273,
			Ac_Bookmarks = 274,

			Brightnessdown = 275,
			Brightnessup = 276,
			Displayswitch = 277,
			Kbdillumtoggle = 278,
			Kbdillumdown = 279,
			Kbdillumup = 280,
			Eject = 281,
			Sleep = 282,

			Android_Menu = 118,
			Android_Back = 270,

			Last = 512
		};

	private:
	};

	namespace KeyMods {
		enum Mod {
			None = 0,
			Shift = 1,
			Ctrl = 2,
			Alt = 4
		};
	}
}
