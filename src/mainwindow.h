#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QTextEdit>
#include <QProgressBar>
#include <opencv2/opencv.hpp>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnLoad_clicked();
    void on_btnApply_clicked();
    void on_btnExport_clicked();
    void on_btnReset_clicked();

private:
    // Core Data
    cv::Mat originalImage;
    cv::Mat enhancedImage;

    // UI Elements
    QLabel *labelOriginal;
    QLabel *labelEnhaced;
    QComboBox *comboTech;
    QTextEdit *textMetrics;
    QProgressBar *progressBar;

    // Helpers
    void displayImage(const cv::Mat& img, QLabel* label);
    void updateMetadata(const QString& fileName, const cv::Mat& img);
    void updateMetrics(const QString& techName, double mse, double psnr, double ssim);
};

#endif // MAINWINDOW_H