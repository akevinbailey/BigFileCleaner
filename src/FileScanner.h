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

// ReSharper disable CppFunctionIsNotImplemented
#pragma once
#include <QObject>
#include <QString>
#include <QVector>
#include <QAtomicInteger>
#include <cstdint>

struct FileRecord {
  QString path;
  std::uint64_t sizeBytes{};
};

class FileScanner : public QObject {
  Q_OBJECT
  public:
  explicit FileScanner(QObject *parent = nullptr);

  void configure(const QString &rootDir, std::uint64_t minBytes);

public slots:
void start();
  void requestStop();

  signals:
  void fileFound(const FileRecord &rec);
  void finished();

private:
  QString rootDir_;
  std::uint64_t minBytes_{};
  QAtomicInteger<bool> stop_{false};
};