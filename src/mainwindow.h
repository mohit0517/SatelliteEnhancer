#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QTextEdit>
#include <QProgressBar>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QWheelEvent>
#include <opencv2/opencv.hpp>

// Custom GraphicsView to handle Zoom and Pan
class ZoomPanView : public QGraphicsView {
    Q_OBJECT
public:
    explicit ZoomPanView(QWidget *parent = nullptr);
protected:
    void wheelEvent(QWheelEvent *event) override;
};

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
    void on_btnBatch_clicked();
    void on_btnOpenFolder_clicked();

private:
    cv::Mat originalImage;
    cv::Mat enhancedImage;

    // UI Elements
    QGraphicsScene *sceneOriginal;
    QGraphicsScene *sceneEnhanced;
    ZoomPanView *viewOriginal;
    ZoomPanView *viewEnhanced;

    QComboBox *comboTech;
    QTextEdit *textMetrics;
    QProgressBar *progressBar;

    void displayImage(const cv::Mat& img, QGraphicsScene* scene);
    void updateMetadata(const QString& fileName, const cv::Mat& img);
};

#endif // MAINWINDOW_H