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

#pragma once


#include <QMainWindow>
#include <memory>
#include "FileScanner.h"

class QLineEdit;
class QComboBox;
class QPushButton;
class QTableWidget;
class QProgressBar;
class QThread;

class MainWindow : public QMainWindow {
  Q_OBJECT
  public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow() override;

private slots:
  void browseDir();
  void startSearch();
  void onFileFound(const FileRecord &rec);
  void onScanFinished();
  void onDeleteSelected();
  void onSelectionChanged() const;

private:
  void buildUi();
  void clearResults();
  void repopulateTableSorted();
  QVector<int> selectedRows() const;

  // Inputs
  QLineEdit *edDir_{};
  QPushButton *btnBrowse_{};
  QLineEdit *edMinValue_{}; // decimal number
  QComboBox *cbUnit_{}; // MB, GB, TB
  QPushButton *btnSearch_{};
  QPushButton *btnDeleteSelected_{};

  // Results
  QTableWidget *tbl_{};     // columns: File, Size
  QProgressBar *progress_{};

  // Data
  QVector<FileRecord> results_;

  // Scanning thread
  std::unique_ptr<QThread> workerThread_;
  FileScanner *scanner_{}; // owned by thread
};