#include "mainwindow.h"
#include <QApplication>
#include <opencv2/core/utils/logger.hpp>

int main(int argc, char *argv[]) {
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_SILENT);

    QApplication a(argc, argv);

    MainWindow w;
    w.setWindowTitle("Satellite Image Enhancer - DRDO CAIR");
    w.show();

    return a.exec();
}