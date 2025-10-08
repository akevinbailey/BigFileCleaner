// Copyright (c) 2025. Andrew Kevin Bailey
// This code, firmware, and software is released under the MIT License (http://opensource.org/licenses/MIT).
//
// The MIT License (MIT)
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
// documentation files (the "Software"), to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
// and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or significant portions of
// the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
// BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "SizeUtils.h"
#include <QLocale>

static constexpr std::uint64_t KiB = 1024ull;
static constexpr std::uint64_t MiB = KiB * 1024ull;
static constexpr std::uint64_t GiB = MiB * 1024ull;
static constexpr std::uint64_t TiB = GiB * 1024ull;

std::uint64_t SizeUtils::toBytes(double value, const QString &unit) {
  const QString u = unit.trimmed().toUpper();
  auto v = static_cast<long double>(value);
  if (u.startsWith("TB")) return static_cast<std::uint64_t>(v * static_cast<long double>(TiB));
  if (u.startsWith("GB")) return static_cast<std::uint64_t>(v * static_cast<long double>(GiB));
  // Default MB
  return static_cast<std::uint64_t>(v * static_cast<long double>(MiB));
}

QString SizeUtils::pretty(std::uint64_t bytes) {
  QLocale loc;
  auto fmt = [&](std::uint64_t denominator, const char* suffix){
    long double v = static_cast<long double>(bytes) /
                    static_cast<long double>(denominator);
    return loc.toString(static_cast<double>(v), 'f', 2) + suffix;
  };

  if (bytes >= TiB) return fmt(TiB, " TiB");
  if (bytes >= GiB) return fmt(GiB, " GiB");
  if (bytes >= MiB) return fmt(MiB, " MiB");
  if (bytes >= KiB) return fmt(KiB, " KiB");

  return loc.toString(static_cast<qlonglong>(bytes)) + " B";
}