/* This file is part of the KDE project
   Copyright 2004 Tomas Mecir <mecirt@gmail.com>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KSPREAD_CONVERT
#define KSPREAD_CONVERT

#include "kspread_value.h"

class KLocale;

// KSpread namespace
namespace KSpread {

/**
The Convert class contains various methods used to convert between
various data types of KSpreadValue. Things like number to string,
string to bool, and so on */

class Convert {
 public:
  static KSpreadValue toBool (const KSpreadValue &val, KLocale *locale);
  static KSpreadValue toInteger (const KSpreadValue &val, KLocale *locale);
  static KSpreadValue toFloat (const KSpreadValue &val, KLocale *locale);
  static KSpreadValue toString (const KSpreadValue &val, KLocale *locale);
  static KSpreadValue toDateTime (const KSpreadValue &val, KLocale *locale);
};

};  //end of KSpread namespace

#endif //KSPREAD_CONVERT

