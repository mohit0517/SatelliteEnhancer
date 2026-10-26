#include "mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QImage>
#include <QFileInfo>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QGraphicsPixmapItem>
#include <QScrollBar>
#include <QDir>
#include <QFileInfoList>
#include <fstream>

#include "preprocess.h"
#include "linear_enhance.h"
#include "nonlinear.h"
#include "filters.h"
#include "edge_enhance.h"
#include "metrics.h"

// --- Custom Zoom/Pan Class ---
ZoomPanView::ZoomPanView(QWidget *parent) : QGraphicsView(parent) {
    setDragMode(QGraphicsView::ScrollHandDrag);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setRenderHint(QPainter::SmoothPixmapTransform);
    setStyleSheet("background-color: #181825; border: 2px dashed #45475a; border-radius: 10px;");
}

void ZoomPanView::wheelEvent(QWheelEvent *event) {
    double scaleFactor = 1.15;
    if (event->angleDelta().y() < 0) scaleFactor = 1.0 / scaleFactor;
    scale(scaleFactor, scaleFactor);
}

// --- Main Window ---
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Remote Sensing & GIS Image Analysis");
    resize(1400, 900);

    setStyleSheet(
        "QMainWindow { background-color: #1e1e2e; }"
        "QWidget { color: #cdd6f4; font-family: 'Segoe UI'; }"
        "QPushButton { background-color: #89b4fa; color: #11111b; border-radius: 6px; padding: 10px; font-weight: bold; }"
        "QPushButton:hover { background-color: #b4befe; }"
        "QComboBox { background-color: #313244; color: #cdd6f4; border: 1px solid #45475a; border-radius: 6px; padding: 6px; }"
        "QTextEdit { background-color: #181825; color: #a6e3a1; border: 1px solid #45475a; border-radius: 6px; padding: 8px; font-family: Consolas; }"
        );

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    // --- LEFT PANEL ---
    QWidget *leftPanel = new QWidget();
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    QHBoxLayout *imageLayout = new QHBoxLayout();

    QVBoxLayout *origLayout = new QVBoxLayout();
    QLabel *titleOrig = new QLabel("Original Satellite Image");
    titleOrig->setAlignment(Qt::AlignCenter);
    titleOrig->setStyleSheet("font-size: 16px; font-weight: bold; color: #89b4fa;");
    sceneOriginal = new QGraphicsScene(this);
    viewOriginal = new ZoomPanView();
    viewOriginal->setScene(sceneOriginal);
    origLayout->addWidget(titleOrig);
    origLayout->addWidget(viewOriginal);

    QVBoxLayout *enhLayout = new QVBoxLayout();
    QLabel *titleEnh = new QLabel("Processed / Enhanced Image");
    titleEnh->setAlignment(Qt::AlignCenter);
    titleEnh->setStyleSheet("font-size: 16px; font-weight: bold; color: #a6e3a1;");
    sceneEnhanced = new QGraphicsScene(this);
    viewEnhanced = new ZoomPanView();
    viewEnhanced->setScene(sceneEnhanced);
    enhLayout->addWidget(titleEnh);
    enhLayout->addWidget(viewEnhanced);

    imageLayout->addLayout(origLayout);
    imageLayout->addLayout(enhLayout);
    leftLayout->addLayout(imageLayout);

    // --- RIGHT PANEL ---
    QWidget *rightPanel = new QWidget();
    rightPanel->setFixedWidth(400);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);

    QPushButton *btnLoad = new QPushButton("📁 Load Satellite Image");
    connect(btnLoad, &QPushButton::clicked, this, &MainWindow::on_btnLoad_clicked);

    comboTech = new QComboBox();
    comboTech->addItems({"CLAHE", "Histogram Equalization", "Gamma Correction", "Median Filter", "Unsharp Mask", "Sobel Edge"});

    QPushButton *btnApply = new QPushButton("⚡ Apply Enhancement");
    connect(btnApply, &QPushButton::clicked, this, &MainWindow::on_btnApply_clicked);

    QPushButton *btnBatch = new QPushButton("📂 Batch Process Folder");
    connect(btnBatch, &QPushButton::clicked, this, &MainWindow::on_btnBatch_clicked);

    QPushButton *btnExport = new QPushButton("💾 Export Processed Image");
    connect(btnExport, &QPushButton::clicked, this, &MainWindow::on_btnExport_clicked);

    QPushButton *btnReset = new QPushButton("🔄 Reset to Original");
    connect(btnReset, &QPushButton::clicked, this, &MainWindow::on_btnReset_clicked);

    textMetrics = new QTextEdit();
    textMetrics->setReadOnly(true);
    textMetrics->setText("--- AWAITING DATA ---\nLoad an image to view metadata.\n\n💡 TIP: Use Mouse Wheel to Zoom, Click and Drag to Pan.");

    progressBar = new QProgressBar();
    progressBar->setValue(0);
    progressBar->setTextVisible(false);
    progressBar->setFixedHeight(10);

    rightLayout->addWidget(btnLoad);
    rightLayout->addWidget(comboTech);
    rightLayout->addWidget(btnApply);
    rightLayout->addWidget(btnBatch);
    rightLayout->addSpacing(20);
    rightLayout->addWidget(new QLabel("Analysis Metrics:"));
    rightLayout->addWidget(textMetrics);
    rightLayout->addSpacing(30);
    rightLayout->addWidget(btnExport);
    rightLayout->addWidget(btnReset);
    rightLayout->addWidget(progressBar);
    rightLayout->addStretch();

    mainLayout->addWidget(leftPanel);
    mainLayout->addWidget(rightPanel);
}

MainWindow::~MainWindow() {}

void MainWindow::displayImage(const cv::Mat& img, QGraphicsScene* scene) {
    if (img.empty()) return;
    QImage qimg;
    if (img.channels() == 1) {
        qimg = QImage(img.data, img.cols, img.rows, img.step, QImage::Format_Grayscale8).copy();
    } else {
        cv::Mat rgb;
        cv::cvtColor(img, rgb, cv::COLOR_BGR2RGB);
        qimg = QImage(rgb.data, rgb.cols, rgb.rows, rgb.step, QImage::Format_RGB888).copy();
    }
    scene->clear();
    scene->addPixmap(QPixmap::fromImage(qimg));
    scene->setSceneRect(qimg.rect());
}

void MainWindow::updateMetadata(const QString& fileName, const cv::Mat& img) {
    QFileInfo fileInfo(fileName);
    textMetrics->clear();
    textMetrics->append("--- IMAGE METADATA ---");
    textMetrics->append(QString("File: %1").arg(fileInfo.fileName()));
    textMetrics->append(QString("Size: %1 x %2").arg(img.cols).arg(img.rows));
    textMetrics->append(QString("Channels: %1").arg(img.channels()));
    textMetrics->append("\n💡 TIP: Mouse Wheel to Zoom, Drag to Pan.");
}

void MainWindow::on_btnLoad_clicked() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open Image", "data/raw", "Images (*.png *.jpg *.jpeg *.tif *.bmp)");
    if (fileName.isEmpty()) return;

    originalImage = cv::imread(fileName.toStdString());
    if (originalImage.empty()) return;

    displayImage(originalImage, sceneOriginal);
    sceneEnhanced->clear();
    updateMetadata(fileName, originalImage);
}

void MainWindow::on_btnApply_clicked() {
    if (originalImage.empty()) return;

    int index = comboTech->currentIndex();
    switch(index) {
    case 0: enhancedImage = rs::clahe(originalImage, 3.0, 8); break;
    case 1: enhancedImage = rs::histEqualize(originalImage); break;
    case 2: enhancedImage = rs::gammaCorrect(originalImage, 0.5); break;
    case 3: enhancedImage = rs::medianFilter(originalImage, 5); break;
    case 4: enhancedImage = rs::unsharpMask(originalImage, 5, 1.0, 1.5); break;
    case 5: enhancedImage = rs::sobelEdge(originalImage); break;
    }

    displayImage(enhancedImage, sceneEnhanced);

    cv::Mat refImg = (enhancedImage.channels() == 1) ? rs::toGray(originalImage) : originalImage;
    double psnr = rs::computePSNR(refImg, enhancedImage);
    double ssim = rs::computeSSIM(refImg, enhancedImage);
    double entropy = rs::computeEntropy(enhancedImage);
    double contrast = rs::computeContrast(enhancedImage);

    textMetrics->append("\n--- METRICS ---");
    textMetrics->append(QString("Technique: %1").arg(comboTech->currentText()));
    textMetrics->append(QString("PSNR: %1 dB | SSIM: %2").arg(psnr, 0, 'f', 2).arg(ssim, 0, 'f', 4));
    textMetrics->append(QString("Entropy: %1 | Contrast: %2").arg(entropy, 0, 'f', 2).arg(contrast, 0, 'f', 2));
}

void MainWindow::on_btnExport_clicked() {
    if (enhancedImage.empty()) return;
    QString fileName = QFileDialog::getSaveFileName(this, "Save Image", "data/enhanced/output.png", "PNG (*.png);;JPEG (*.jpg)");
    if (fileName.isEmpty()) return;
    cv::imwrite(fileName.toStdString(), enhancedImage);
}

void MainWindow::on_btnReset_clicked() {
    if (!originalImage.empty()) {
        enhancedImage = cv::Mat();
        sceneEnhanced->clear();
    }
}

void MainWindow::on_btnBatch_clicked() {
    QString dirName = QFileDialog::getExistingDirectory(this, "Select Folder with Images", "data/raw");
    if (dirName.isEmpty()) return;

    QString outDir = dirName + "/batch_output";
    QDir().mkpath(outDir);

    std::string csvPath = (dirName + "/batch_results.csv").toStdString();
    std::ofstream csv(csvPath);
    csv << "Filename,Technique,PSNR,SSIM,Entropy,Contrast\n";

    int index = comboTech->currentIndex();
    std::string techName = comboTech->currentText().toStdString();

    QDir dir(dirName);
    QStringList filters;
    filters << "*.png" << "*.jpg" << "*.jpeg" << "*.tif" << "*.bmp";
    dir.setNameFilters(filters);
    QFileInfoList fileList = dir.entryInfoList();

    int count = 0;
    textMetrics->append("\n--- BATCH PROCESSING STARTED ---");
    textMetrics->append(QString("Found %1 images.").arg(fileList.size()));

    for (const QFileInfo &fileInfo : fileList) {
        cv::Mat img = cv::imread(fileInfo.absoluteFilePath().toStdString());
        if (img.empty()) continue;

        cv::Mat processed;
        switch(index) {
        case 0: processed = rs::clahe(img, 3.0, 8); break;
        case 1: processed = rs::histEqualize(img); break;
        case 2: processed = rs::gammaCorrect(img, 0.5); break;
        case 3: processed = rs::medianFilter(img, 5); break;
        case 4: processed = rs::unsharpMask(img, 5, 1.0, 1.5); break;
        case 5: processed = rs::sobelEdge(img); break;
        }

        std::string outPath = outDir.toStdString() + "/" + fileInfo.baseName().toStdString() + "_enhanced.png";
        cv::imwrite(outPath, processed);

        cv::Mat refImg = (processed.channels() == 1) ? rs::toGray(img) : img;
        double psnr = rs::computePSNR(refImg, processed);
        double ssim = rs::computeSSIM(refImg, processed);
        double entropy = rs::computeEntropy(processed);
        double contrast = rs::computeContrast(processed);

        csv << fileInfo.fileName().toStdString() << ","
            << techName << ","
            << psnr << "," << ssim << "," << entropy << "," << contrast << "\n";

        count++;
    }

    csv.close();
    textMetrics->append(QString("✅ Successfully processed %1 images.").arg(count));
    textMetrics->append("CSV and images saved to: " + outDir);
    QMessageBox::information(this, "Batch Complete", QString("Processed %1 images!\nCSV saved.").arg(count));
}