#include "MainWindow.h"
#include "Logger.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QHeaderView>
#include <QFont>
#include <QStatusBar>
#include <QMenuBar>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setupUI();
    updateButtonStates();

    Logger::instance().setCallback([this](const std::string& line) {
        appendLog(line);
    });

    Logger::instance().log(LogLevel::INFO, "\u0413\u043b\u0430\u0432\u043d\u043e\u0435 \u043e\u043a\u043d\u043e \u0438\u043d\u0438\u0446\u0438\u0430\u043b\u0438\u0437\u0438\u0440\u043e\u0432\u0430\u043d\u043e");
}

void MainWindow::setupUI() {
    auto* centralWidget = new QWidget(this);
    auto* mainLayout = new QVBoxLayout(centralWidget);

    auto* dbGroup = new QGroupBox(QString::fromUtf8("\u0411\u0430\u0437\u0430 \u0441\u043a\u043b\u043e\u043d\u0435\u043d\u0438\u0439"), this);
    auto* dbLayout = new QVBoxLayout(dbGroup);

    auto* dbTopLayout = new QHBoxLayout();
    loadFileBtn_ = new QPushButton(QString::fromUtf8("\u0417\u0430\u0433\u0440\u0443\u0437\u0438\u0442\u044c \u0444\u0430\u0439\u043b..."), this);
    statusLabel_ = new QLabel(QString::fromUtf8("\u0411\u0430\u0437\u0430 \u043d\u0435 \u0437\u0430\u0433\u0440\u0443\u0436\u0435\u043d\u0430"), this);
    dbTopLayout->addWidget(loadFileBtn_);
    dbTopLayout->addWidget(statusLabel_);
    dbTopLayout->addStretch();
    dbLayout->addLayout(dbTopLayout);

    tableWidget_ = new QTableWidget(this);
    tableWidget_->setColumnCount(7);
    QStringList headers;
    headers << QString::fromUtf8("\u0421\u043b\u043e\u0432\u043e")
            << QString::fromUtf8("\u0418\u043c.")
            << QString::fromUtf8("\u0420\u043e\u0434.")
            << QString::fromUtf8("\u0414\u0430\u0442.")
            << QString::fromUtf8("\u0412\u0438\u043d.")
            << QString::fromUtf8("\u0422\u0432.")
            << QString::fromUtf8("\u041f\u0440.");
    tableWidget_->setHorizontalHeaderLabels(headers);
    tableWidget_->horizontalHeader()->setStretchLastSection(true);
    tableWidget_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget_->setAlternatingRowColors(true);
    tableWidget_->setMinimumHeight(150);
    dbLayout->addWidget(tableWidget_);

    mainLayout->addWidget(dbGroup);

    auto* lookupGroup = new QGroupBox(QString::fromUtf8("\u041f\u043e\u0438\u0441\u043a \u0441\u043b\u043e\u0432\u0430"), this);
    auto* lookupLayout = new QVBoxLayout(lookupGroup);

    auto* lookupInputLayout = new QHBoxLayout();
    lookupInput_ = new QLineEdit(this);
    lookupInput_->setPlaceholderText(QString::fromUtf8("\u0412\u0432\u0435\u0434\u0438\u0442\u0435 \u0441\u043b\u043e\u0432\u043e \u0432 \u043b\u044e\u0431\u043e\u043c \u043f\u0430\u0434\u0435\u0436\u0435..."));
    lookupBtn_ = new QPushButton(QString::fromUtf8("\u041d\u0430\u0439\u0442\u0438"), this);
    lookupInputLayout->addWidget(lookupInput_);
    lookupInputLayout->addWidget(lookupBtn_);
    lookupLayout->addLayout(lookupInputLayout);

    resultLabel_ = new QLabel(this);
    resultLabel_->setStyleSheet("QLabel { padding: 8px; font-size: 14px; }");
    resultLabel_->setWordWrap(true);
    lookupLayout->addWidget(resultLabel_);

    mainLayout->addWidget(lookupGroup);

    auto* processGroup = new QGroupBox(QString::fromUtf8("\u041e\u0431\u0440\u0430\u0431\u043e\u0442\u043a\u0430 \u0444\u0430\u0439\u043b\u0430"), this);
    auto* processLayout = new QVBoxLayout(processGroup);

    auto* inputLayout = new QHBoxLayout();
    selectInputBtn_ = new QPushButton(QString::fromUtf8("\u0412\u0445\u043e\u0434\u043d\u043e\u0439 \u0444\u0430\u0439\u043b..."), this);
    inputPathLabel_ = new QLabel(QString::fromUtf8("\u043d\u0435 \u0432\u044b\u0431\u0440\u0430\u043d"), this);
    inputLayout->addWidget(selectInputBtn_);
    inputLayout->addWidget(inputPathLabel_, 1);
    processLayout->addLayout(inputLayout);

    auto* outputLayout = new QHBoxLayout();
    selectOutputBtn_ = new QPushButton(QString::fromUtf8("\u0412\u044b\u0445\u043e\u0434\u043d\u043e\u0439 \u0444\u0430\u0439\u043b..."), this);
    outputPathLabel_ = new QLabel(QString::fromUtf8("\u043d\u0435 \u0432\u044b\u0431\u0440\u0430\u043d"), this);
    outputLayout->addWidget(selectOutputBtn_);
    outputLayout->addWidget(outputPathLabel_, 1);
    processLayout->addLayout(outputLayout);

    processBtn_ = new QPushButton(QString::fromUtf8("\u041e\u0431\u0440\u0430\u0431\u043e\u0442\u0430\u0442\u044c"), this);
    processLayout->addWidget(processBtn_);

    resultsDisplay_ = new QTextEdit(this);
    resultsDisplay_->setReadOnly(true);
    resultsDisplay_->setMaximumHeight(120);
    processLayout->addWidget(resultsDisplay_);

    mainLayout->addWidget(processGroup);

    auto* logGroup = new QGroupBox(QString::fromUtf8("\u041b\u043e\u0433"), this);
    auto* logLayout = new QVBoxLayout(logGroup);

    logDisplay_ = new QTextEdit(this);
    logDisplay_->setReadOnly(true);
    logDisplay_->setFont(QFont("Consolas", 9));
    logDisplay_->setMaximumHeight(140);
    logLayout->addWidget(logDisplay_);

    clearLogBtn_ = new QPushButton(QString::fromUtf8("\u041e\u0447\u0438\u0441\u0442\u0438\u0442\u044c \u043b\u043e\u0433"), this);
    logLayout->addWidget(clearLogBtn_);

    mainLayout->addWidget(logGroup);

    setCentralWidget(centralWidget);
    statusBar()->showMessage(QString::fromUtf8("\u0413\u043e\u0442\u043e\u0432\u043e"));

    connect(loadFileBtn_, &QPushButton::clicked, this, &MainWindow::onLoadFileClicked);
    connect(lookupBtn_, &QPushButton::clicked, this, &MainWindow::onLookupClicked);
    connect(lookupInput_, &QLineEdit::returnPressed, this, &MainWindow::onLookupClicked);
    connect(clearLogBtn_, &QPushButton::clicked, this, &MainWindow::onClearLogClicked);
    connect(processBtn_, &QPushButton::clicked, this, &MainWindow::onProcessFileClicked);

    connect(selectInputBtn_, &QPushButton::clicked, this, [this]() {
        QString path = QFileDialog::getOpenFileName(this,
            QString::fromUtf8("\u0412\u044b\u0431\u0440\u0430\u0442\u044c \u0432\u0445\u043e\u0434\u043d\u043e\u0439 \u0444\u0430\u0439\u043b"),
            QString(),
            QString::fromUtf8("\u0422\u0435\u043a\u0441\u0442\u043e\u0432\u044b\u0435 \u0444\u0430\u0439\u043b\u044b (*.txt);;All Files (*)"));
        if (!path.isEmpty()) {
            inputFilePath_ = path.toStdString();
            inputPathLabel_->setText(path);
            updateButtonStates();
            Logger::instance().log(LogLevel::INFO,
                "\u0412\u044b\u0431\u0440\u0430\u043d \u0432\u0445\u043e\u0434\u043d\u043e\u0439 \u0444\u0430\u0439\u043b: " + inputFilePath_);
        }
    });

    connect(selectOutputBtn_, &QPushButton::clicked, this, [this]() {
        QString path = QFileDialog::getSaveFileName(this,
            QString::fromUtf8("\u0412\u044b\u0431\u0440\u0430\u0442\u044c \u0432\u044b\u0445\u043e\u0434\u043d\u043e\u0439 \u0444\u0430\u0439\u043b"),
            QString(),
            QString::fromUtf8("\u0422\u0435\u043a\u0441\u0442\u043e\u0432\u044b\u0435 \u0444\u0430\u0439\u043b\u044b (*.txt);;All Files (*)"));
        if (!path.isEmpty()) {
            outputFilePath_ = path.toStdString();
            outputPathLabel_->setText(path);
            updateButtonStates();
            Logger::instance().log(LogLevel::INFO,
                "\u0412\u044b\u0431\u0440\u0430\u043d \u0432\u044b\u0445\u043e\u0434\u043d\u043e\u0439 \u0444\u0430\u0439\u043b: " + outputFilePath_);
        }
    });
}

void MainWindow::onLoadFileClicked() {
    QString path = QFileDialog::getOpenFileName(this,
        QString::fromUtf8("\u0417\u0430\u0433\u0440\u0443\u0437\u0438\u0442\u044c \u0442\u0430\u0431\u043b\u0438\u0446\u0443 \u0441\u043a\u043b\u043e\u043d\u0435\u043d\u0438\u0439"),
        QString(),
        QString::fromUtf8("\u0422\u0435\u043a\u0441\u0442\u043e\u0432\u044b\u0435 \u0444\u0430\u0439\u043b\u044b (*.txt);;All Files (*)"));

    if (path.isEmpty()) {
        return;
    }

    try {
        database_.loadFromFile(path.toStdString());
        updateTableWidget();
        statusLabel_->setText(
            QString::fromUtf8("\u0417\u0430\u0433\u0440\u0443\u0436\u0435\u043d\u043e \u0441\u043b\u043e\u0432: %1").arg(database_.wordCount()));
        statusBar()->showMessage(
            QString::fromUtf8("\u0411\u0430\u0437\u0430 \u0441\u043a\u043b\u043e\u043d\u0435\u043d\u0438\u0439 \u0437\u0430\u0433\u0440\u0443\u0436\u0435\u043d\u0430"));
        updateButtonStates();
    } catch (const std::exception& e) {
        QMessageBox::critical(this,
            QString::fromUtf8("\u041e\u0448\u0438\u0431\u043a\u0430"),
            QString::fromUtf8(e.what()));
        Logger::instance().log(LogLevel::ERR,
            std::string("\u041e\u0448\u0438\u0431\u043a\u0430 \u0437\u0430\u0433\u0440\u0443\u0437\u043a\u0438 \u0444\u0430\u0439\u043b\u0430: ") + e.what());
    }
}

void MainWindow::onLookupClicked() {
    QString input = lookupInput_->text().trimmed();
    if (input.isEmpty()) {
        QMessageBox::information(this,
            QString::fromUtf8("\u0412\u043d\u0438\u043c\u0430\u043d\u0438\u0435"),
            QString::fromUtf8("\u0412\u0432\u0435\u0434\u0438\u0442\u0435 \u0441\u043b\u043e\u0432\u043e \u0434\u043b\u044f \u043f\u043e\u0438\u0441\u043a\u0430"));
        return;
    }

    try {
        auto [nominative, caseName] = database_.lookup(input.toStdString());
        std::string result = nominative + " (" + caseName + ")";
        resultLabel_->setText(QString::fromUtf8(result.c_str()));
        resultLabel_->setStyleSheet(
            "QLabel { padding: 8px; font-size: 14px; background-color: #d4edda; border-radius: 4px; }");
        Logger::instance().log(LogLevel::INFO,
            "\u041f\u043e\u0438\u0441\u043a '" + input.toStdString() + "' -> " + result);
    } catch (const std::exception& e) {
        resultLabel_->setText(QString::fromUtf8(e.what()));
        resultLabel_->setStyleSheet(
            "QLabel { padding: 8px; font-size: 14px; background-color: #f8d7da; border-radius: 4px; }");
        Logger::instance().log(LogLevel::WARNING,
            "\u041f\u043e\u0438\u0441\u043a '" + input.toStdString() + "': \u043d\u0435 \u043d\u0430\u0439\u0434\u0435\u043d\u043e");
    }
}

void MainWindow::onProcessFileClicked() {
    if (database_.isEmpty()) {
        QMessageBox::warning(this,
            QString::fromUtf8("\u041e\u0448\u0438\u0431\u043a\u0430"),
            QString::fromUtf8("\u0421\u043d\u0430\u0447\u0430\u043b\u0430 \u0437\u0430\u0433\u0440\u0443\u0437\u0438\u0442\u0435 \u0431\u0430\u0437\u0443 \u0441\u043a\u043b\u043e\u043d\u0435\u043d\u0438\u0439"));
        return;
    }

    if (inputFilePath_.empty() || outputFilePath_.empty()) {
        QMessageBox::warning(this,
            QString::fromUtf8("\u041e\u0448\u0438\u0431\u043a\u0430"),
            QString::fromUtf8("\u0412\u044b\u0431\u0435\u0440\u0438\u0442\u0435 \u0432\u0445\u043e\u0434\u043d\u043e\u0439 \u0438 \u0432\u044b\u0445\u043e\u0434\u043d\u043e\u0439 \u0444\u0430\u0439\u043b\u044b"));
        return;
    }

    try {
        FileProcessor processor(database_);
        auto results = processor.processFile(inputFilePath_, outputFilePath_);

        resultsDisplay_->clear();
        for (const auto& line : results) {
            resultsDisplay_->append(QString::fromUtf8(line.c_str()));
        }

        statusBar()->showMessage(
            QString::fromUtf8("\u041e\u0431\u0440\u0430\u0431\u043e\u0442\u043a\u0430 \u0437\u0430\u0432\u0435\u0440\u0448\u0435\u043d\u0430, \u0441\u0442\u0440\u043e\u043a: %1").arg(results.size()));

        QMessageBox::information(this,
            QString::fromUtf8("\u0413\u043e\u0442\u043e\u0432\u043e"),
            QString::fromUtf8("\u041e\u0431\u0440\u0430\u0431\u043e\u0442\u043a\u0430 \u0437\u0430\u0432\u0435\u0440\u0448\u0435\u043d\u0430. \u0420\u0435\u0437\u0443\u043b\u044c\u0442\u0430\u0442 \u0437\u0430\u043f\u0438\u0441\u0430\u043d \u0432:\n") +
            QString::fromStdString(outputFilePath_));
    } catch (const std::exception& e) {
        QMessageBox::critical(this,
            QString::fromUtf8("\u041e\u0448\u0438\u0431\u043a\u0430"),
            QString::fromUtf8(e.what()));
        Logger::instance().log(LogLevel::ERR,
            std::string("\u041e\u0448\u0438\u0431\u043a\u0430 \u043e\u0431\u0440\u0430\u0431\u043e\u0442\u043a\u0438: ") + e.what());
    }
}

void MainWindow::onClearLogClicked() {
    logDisplay_->clear();
    Logger::instance().clearHistory();
    Logger::instance().log(LogLevel::INFO, "\u041b\u043e\u0433 \u043e\u0447\u0438\u0449\u0435\u043d \u043f\u043e\u043b\u044c\u0437\u043e\u0432\u0430\u0442\u0435\u043b\u0435\u043c");
}

void MainWindow::updateTableWidget() {
    const auto& entries = database_.getAllEntries();
    const auto& cases = DeclensionDatabase::caseNames();

    tableWidget_->setRowCount(static_cast<int>(entries.size()));

    for (size_t row = 0; row < entries.size(); ++row) {
        std::string nominative = database_.getNominative(row);
        tableWidget_->setItem(static_cast<int>(row), 0,
            new QTableWidgetItem(QString::fromUtf8(nominative.c_str())));

        for (size_t col = 0; col < cases.size(); ++col) {
            auto it = entries[row].find(cases[col]);
            std::string val = (it != entries[row].end()) ? it->second : "-";
            tableWidget_->setItem(static_cast<int>(row), static_cast<int>(col + 1),
                new QTableWidgetItem(QString::fromUtf8(val.c_str())));
        }
    }

    tableWidget_->resizeColumnsToContents();
}

void MainWindow::appendLog(const std::string& line) {
    logDisplay_->append(QString::fromUtf8(line.c_str()));
}

void MainWindow::updateButtonStates() {
    bool dbLoaded = !database_.isEmpty();
    lookupBtn_->setEnabled(dbLoaded);
    lookupInput_->setEnabled(dbLoaded);
    processBtn_->setEnabled(dbLoaded && !inputFilePath_.empty() && !outputFilePath_.empty());
}
