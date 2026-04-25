#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QLineEdit>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>

#include "DeclensionDatabase.h"
#include "FileProcessor.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void onLoadFileClicked();
    void onLookupClicked();
    void onProcessFileClicked();
    void onClearLogClicked();

private:
    void setupUI();
    void updateTableWidget();
    void appendLog(const std::string& line);
    void updateButtonStates();

    DeclensionDatabase database_;

    QPushButton* loadFileBtn_;
    QLabel* statusLabel_;
    QTableWidget* tableWidget_;

    QLineEdit* lookupInput_;
    QPushButton* lookupBtn_;
    QLabel* resultLabel_;

    QPushButton* selectInputBtn_;
    QPushButton* selectOutputBtn_;
    QLabel* inputPathLabel_;
    QLabel* outputPathLabel_;
    QPushButton* processBtn_;
    QTextEdit* resultsDisplay_;

    QTextEdit* logDisplay_;
    QPushButton* clearLogBtn_;

    std::string inputFilePath_;
    std::string outputFilePath_;
};

#endif
