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

#include "MainWindow.h"
#include "SizeUtils.h"

#include <QApplication>
#include <QBoxLayout>
#include <QComboBox>
#include <QFileDialog>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QLineEdit>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QTableWidget>
#include <QThread>
#include <QToolButton>
#include <QtGlobal>

#include <algorithm>
#include <filesystem>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setWindowTitle("Big File Cleaner");
  resize(1280, 600);
  buildUi();
}

MainWindow::~MainWindow() {
  if (scanner_) scanner_->requestStop();
}

void MainWindow::buildUi() {
  auto *central = new QWidget(this);
  auto *layout = new QVBoxLayout(central);

  // Row: Search Directory + Browse
  {
    auto *row = new QHBoxLayout();
    edDir_ = new QLineEdit(central);
    edDir_->setPlaceholderText("Search Directory (existing folder)");
    btnBrowse_ = new QPushButton("Browse...", central);
    connect(btnBrowse_, &QPushButton::clicked, this, &MainWindow::browseDir);
    row->addWidget(edDir_, 1);
    row->addWidget(btnBrowse_);
    layout->addLayout(row);
  }

  // Row: Minimum File Size (value and unit)
  {
    auto *row = new QHBoxLayout();
    edMinValue_ = new QLineEdit(central);
    edMinValue_->setPlaceholderText("Minimum File Size (decimal)");
    cbUnit_ = new QComboBox(central);
    cbUnit_->addItems({"MB", "GB", "TB"});
    edMinValue_->setText("1.0");
    btnSearch_ = new QPushButton("Search", central);
    connect(btnSearch_, &QPushButton::clicked, this, &MainWindow::startSearch);
    btnDeleteSelected_ = new QPushButton("Delete Selected", central);
    btnDeleteSelected_->setEnabled(false);
    connect(btnDeleteSelected_, &QPushButton::clicked, this, &MainWindow::onDeleteSelected);
    row->addWidget(edMinValue_);
    row->addWidget(cbUnit_);
    row->addStretch(1);
    row->addWidget(btnSearch_);
    row->addWidget(btnDeleteSelected_);
    layout->addLayout(row);
  }

  // Progress
  progress_ = new QProgressBar(central);
  progress_->setRange(0, 0); // indeterminate during scan
  progress_->setVisible(false);
  layout->addWidget(progress_);

  // Found table (2 columns: File, Size)
  tbl_ = new QTableWidget(0, 2, central);
  QStringList headers; headers << "File" << "Size";
  tbl_->setHorizontalHeaderLabels(headers);
  tbl_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
  tbl_->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
  tbl_->verticalHeader()->setVisible(false);
  tbl_->setEditTriggers(QAbstractItemView::NoEditTriggers);
  tbl_->setSelectionBehavior(QAbstractItemView::SelectRows);
  tbl_->setSelectionMode(QAbstractItemView::ExtendedSelection);

  // Enable/disable the Delete button based on selection
  connect(tbl_->selectionModel(), &QItemSelectionModel::selectionChanged,
  this, &MainWindow::onSelectionChanged);

  layout->addWidget(tbl_);
  setCentralWidget(central);
}

void MainWindow::browseDir() {
  const QString dir = QFileDialog::getExistingDirectory(this, "Select Search Directory", edDir_->text());
  if (!dir.isEmpty()) edDir_->setText(dir);
}

void MainWindow::startSearch() {
  // Validate directory
  const QString dir = edDir_->text().trimmed();
  if (dir.isEmpty()) {
    QMessageBox::warning(this, "Invalid Directory", "Please enter or select a search directory.");
    return;
  }

  // Parse size
  bool ok = false;
  const double value = edMinValue_->text().trimmed().toDouble(&ok);
  if (!ok || value < 0.0) {
    QMessageBox::warning(this, "Invalid Size", "Please enter a valid decimal value for minimum file size.");
    return;
  }
  const auto minBytes = SizeUtils::toBytes(value, cbUnit_->currentText());

  // Clear previous results and start fresh
  clearResults();

  // Threaded scanner
  workerThread_ = std::make_unique<QThread>();
  scanner_ = new FileScanner();
  scanner_->moveToThread(workerThread_.get());
  scanner_->configure(dir, minBytes);

  connect(workerThread_.get(), &QThread::started, scanner_, &FileScanner::start);
  connect(scanner_, &FileScanner::fileFound, this, &MainWindow::onFileFound);
  connect(scanner_, &FileScanner::finished, this, &MainWindow::onScanFinished);
  connect(scanner_, &FileScanner::finished, workerThread_.get(), &QThread::quit);
  connect(workerThread_.get(), &QThread::finished, scanner_, &QObject::deleteLater);

  progress_->setVisible(true);
  workerThread_->start();
}

void MainWindow::clearResults() {
  results_.clear();
  tbl_->setRowCount(0);
  btnDeleteSelected_->setEnabled(false);
}

void MainWindow::onFileFound(const FileRecord &rec) {
  results_.push_back(rec);
}


void MainWindow::repopulateTableSorted() {
  std::ranges::sort(results_, [](const FileRecord &a, const FileRecord &b){
  return a.sizeBytes > b.sizeBytes; // largest first
  });

  tbl_->setRowCount(static_cast<int>(results_.size()));
  for (int i = 0; i < results_.size(); ++i) {
    const auto &rec = results_[static_cast<qsizetype>(i)];
    const QString displayPath = QDir::toNativeSeparators(rec.path);
    auto *itemPath = new QTableWidgetItem(displayPath);
    auto *itemSize = new QTableWidgetItem(SizeUtils::pretty(rec.sizeBytes));
    itemPath->setToolTip(displayPath);
    itemSize->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    tbl_->setItem(i, 0, itemPath);
    tbl_->setItem(i, 1, itemSize);
  }
}


void MainWindow::onScanFinished() {
  progress_->setVisible(false);
  repopulateTableSorted();
}

QVector<int> MainWindow::selectedRows() const {
  QVector<int> rows;
  const auto indexes = tbl_->selectionModel()->selectedRows(); // unique rows
  rows.reserve(indexes.size());
  for (const QModelIndex &idx : indexes) rows.push_back(idx.row());
  std::ranges::sort(rows);
  rows.erase(std::ranges::unique(rows).begin(), rows.end());
  return rows;
}

void MainWindow::onSelectionChanged() const {
  btnDeleteSelected_->setEnabled(!selectedRows().isEmpty());
}

void MainWindow::onDeleteSelected() {
  auto rows = selectedRows();
  if (rows.isEmpty()) return;

  // Build confirmation text with normalized paths
  QStringList paths;
  paths.reserve(rows.size());
  for (int r : rows) {
    if (r >= 0 && r < results_.size())
      paths << QDir::toNativeSeparators(results_[r].path);
  }

  const QString msg = QString("Are you sure you want to delete the following %1 file(s)?\n%2").arg(paths.size()).arg(paths.join("\n"));
  if (QMessageBox::question(this, "Confirm Delete", msg) != QMessageBox::Yes) return;

  // Delete in descending row order to keep indices valid as we remove
  std::ranges::sort(rows, std::greater<int>());

  QStringList failed;

  for (int r : rows) {
    if (r < 0 || r >= results_.size()) continue;
    const auto [path, sizeBytes] = results_[r];
    std::error_code ec;
    bool ok = std::filesystem::remove(std::filesystem::path(path.toStdU16String()), ec);
    if (!ok || ec) {
      failed << (QDir::toNativeSeparators(path) + (ec ? (" — " + QString::fromStdString(ec.message())) : QString()));
      continue;
    }

    // success: remove from model/view
    results_.removeAt(r);
    tbl_->removeRow(r);
  }

  if (!failed.isEmpty()) {
    QMessageBox::warning(this, "Some Deletes Failed", QString("The following files could not be deleted: %1").arg(failed.join("")));
  }

  onSelectionChanged();
}