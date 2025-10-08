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

#include "FileScanner.h"

#include <filesystem>
#include <system_error>

using namespace std;
namespace fs = std::filesystem;

FileScanner::FileScanner(QObject *parent) : QObject(parent) {}

void FileScanner::configure(const QString &rootDir, std::uint64_t minBytes) {
  rootDir_ = rootDir;
  minBytes_ = minBytes;
}

void FileScanner::requestStop() { stop_.storeRelease(true); }

void FileScanner::start() {
  stop_.storeRelease(false);
  std::error_code ec;
  fs::directory_options opts = fs::directory_options::skip_permission_denied;

  if (!fs::exists(rootDir_.toStdU16String(), ec) || !fs::is_directory(rootDir_.toStdU16String(), ec)) {
    emit finished();
    return;
  }

  for (fs::recursive_directory_iterator it(rootDir_.toStdU16String(), opts, ec), end; it != end && !stop_.loadAcquire(); it.increment(ec)) {
    if (ec) { ec.clear(); continue; }
    const fs::directory_entry &de = *it;
    if (!de.is_regular_file(ec)) { if (ec) ec.clear(); continue; }
    const auto sz = de.file_size(ec);
    if (ec) { ec.clear(); continue; }
    if (sz >= minBytes_) {
      FileRecord rec{ QString::fromStdU16String(de.path().u16string()), static_cast<std::uint64_t>(sz) };
      emit fileFound(rec);
    }
  }

  emit finished();
}