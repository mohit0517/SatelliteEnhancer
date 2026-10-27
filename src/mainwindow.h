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
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <opencv2/opencv.hpp>

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

protected: // <--- MAKE SURE THIS IS HERE
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

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