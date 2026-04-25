#include <QApplication>
#include "MainWindow.h"
#include "Logger.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    Logger::instance().init("application.log");
    Logger::instance().log(LogLevel::INFO, "\u041f\u0440\u0438\u043b\u043e\u0436\u0435\u043d\u0438\u0435 \u0437\u0430\u043f\u0443\u0449\u0435\u043d\u043e");

    MainWindow window;
    window.setWindowTitle(QString::fromUtf8("\u0422\u0430\u0431\u043b\u0438\u0446\u0430 \u0441\u043a\u043b\u043e\u043d\u0435\u043d\u0438\u0439 - \u0417\u0430\u0434\u0430\u043d\u0438\u0435 4"));
    window.resize(900, 750);
    window.show();

    int result = app.exec();

    Logger::instance().log(LogLevel::INFO, "\u041f\u0440\u0438\u043b\u043e\u0436\u0435\u043d\u0438\u0435 \u0437\u0430\u0432\u0435\u0440\u0448\u0435\u043d\u043e");
    return result;
}
