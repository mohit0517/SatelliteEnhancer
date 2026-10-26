#include "mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QImage>
#include <QFileInfo>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QFrame>

#include "preprocess.h"
#include "linear_enhance.h"
#include "nonlinear.h"
#include "filters.h"
#include "edge_enhance.h"
#include "metrics.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // 1. Main Window Setup
    setWindowTitle("Remote Sensing & GIS Image Analysis");
    resize(1400, 900);
    setStyleSheet(
        "QMainWindow { background-color: #1e1e2e; }"
        "QWidget { color: #cdd6f4; font-family: 'Segoe UI', Arial, sans-serif; }"
        "QLabel { font-size: 14px; }"
        "QPushButton { background-color: #89b4fa; color: #11111b; border-radius: 6px; padding: 10px; font-weight: bold; }"
        "QPushButton:hover { background-color: #b4befe; }"
        "QComboBox { background-color: #313244; color: #cdd6f4; border: 1px solid #45475a; border-radius: 6px; padding: 6px; }"
        "QTextEdit { background-color: #181825; color: #a6e3a1; border: 1px solid #45475a; border-radius: 6px; padding: 8px; font-family: Consolas, monospace; }"
        "QProgressBar { border: 1px solid #45475a; border-radius: 5px; text-align: center; background-color: #181825; }"
        "QProgressBar::chunk { background-color: #a6e3a1; width: 20px; }"
        );

    // 2. Create Central Widget and Main Layout (Left/Right Split)
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);

    // --- LEFT PANEL (Images) ---
    QWidget *leftPanel = new QWidget();
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);

    // Image Titles
    QHBoxLayout *imageTitles = new QHBoxLayout();
    QLabel *titleOrig = new QLabel("Original Satellite Image");
    titleOrig->setAlignment(Qt::AlignCenter);
    titleOrig->setStyleSheet("font-size: 16px; font-weight: bold; color: #89b4fa;");
    QLabel *titleEnh = new QLabel("Processed / Enhanced Image");
    titleEnh->setAlignment(Qt::AlignCenter);
    titleEnh->setStyleSheet("font-size: 16px; font-weight: bold; color: #a6e3a1;");
    imageTitles->addWidget(titleOrig);
    imageTitles->addWidget(titleEnh);

    // Image Displays
    QHBoxLayout *imageLayout = new QHBoxLayout();
    labelOriginal = new QLabel("No Image Loaded");
    labelOriginal->setAlignment(Qt::AlignCenter);
    labelOriginal->setStyleSheet("background-color: #181825; border: 2px dashed #45475a; border-radius: 10px;");
    labelOriginal->setMinimumSize(500, 500);

    labelEnhaced = new QLabel("Awaiting Processing...");
    labelEnhaced->setAlignment(Qt::AlignCenter);
    labelEnhaced->setStyleSheet("background-color: #181825; border: 2px dashed #45475a; border-radius: 10px;");
    labelEnhaced->setMinimumSize(500, 500);

    imageLayout->addWidget(labelOriginal);
    imageLayout->addWidget(labelEnhaced);

    leftLayout->addLayout(imageTitles);
    leftLayout->addLayout(imageLayout);

    // --- RIGHT PANEL (Controls & Data) ---
    QWidget *rightPanel = new QWidget();
    rightPanel->setFixedWidth(400); // Dashboard sidebar width
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);

    // Controls
    QPushButton *btnLoad = new QPushButton("📁 Load Satellite Image");
    connect(btnLoad, &QPushButton::clicked, this, &MainWindow::on_btnLoad_clicked);

    comboTech = new QComboBox();
    comboTech->addItems({"CLAHE (Contrast Enhancement)", "Histogram Equalization", "Gamma Correction", "Median Filter (Denoise)", "Unsharp Mask (Sharpen)", "Sobel Edge Detection"});

    QPushButton *btnApply = new QPushButton("⚡ Apply Enhancement");
    connect(btnApply, &QPushButton::clicked, this, &MainWindow::on_btnApply_clicked);

    QPushButton *btnExport = new QPushButton("💾 Export Processed Image");
    connect(btnExport, &QPushButton::clicked, this, &MainWindow::on_btnExport_clicked);

    QPushButton *btnReset = new QPushButton("🔄 Reset to Original");
    connect(btnReset, &QPushButton::clicked, this, &MainWindow::on_btnReset_clicked);

    // Metrics Panel
    textMetrics = new QTextEdit();
    textMetrics->setReadOnly(true);
    textMetrics->setText("--- AWAITING DATA ---\nLoad an image to view metadata.");

    // Progress Bar
    progressBar = new QProgressBar();
    progressBar->setValue(0);
    progressBar->setTextVisible(false);
    progressBar->setFixedHeight(10);

    // Assemble Right Panel
    rightLayout->addWidget(btnLoad);
    rightLayout->addWidget(comboTech);
    rightLayout->addWidget(btnApply);
    rightLayout->addSpacing(20);
    rightLayout->addWidget(new QLabel("Analysis Metrics:"));
    rightLayout->addWidget(textMetrics);
    rightLayout->addSpacing(20);
    rightLayout->addWidget(btnExport);
    rightLayout->addWidget(btnReset);
    rightLayout->addWidget(progressBar);
    rightLayout->addStretch(); // Pushes everything up

    // Add panels to main layout
    mainLayout->addWidget(leftPanel);
    mainLayout->addWidget(rightPanel);
}

MainWindow::~MainWindow() {}

void MainWindow::displayImage(const cv::Mat& img, QLabel* label) {
    if (img.empty()) return;
    QImage qimg;
    if (img.channels() == 1) {
        qimg = QImage(img.data, img.cols, img.rows, img.step, QImage::Format_Grayscale8).copy();
    } else {
        cv::Mat rgb;
        cv::cvtColor(img, rgb, cv::COLOR_BGR2RGB);
        qimg = QImage(rgb.data, rgb.cols, rgb.rows, rgb.step, QImage::Format_RGB888).copy();
    }
    label->setPixmap(QPixmap::fromImage(qimg).scaled(label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void MainWindow::updateMetadata(const QString& fileName, const cv::Mat& img) {
    QFileInfo fileInfo(fileName);
    textMetrics->clear();
    textMetrics->append("--- IMAGE METADATA ---");
    textMetrics->append(QString("File: %1").arg(fileInfo.fileName()));
    textMetrics->append(QString("Size: %1 x %2").arg(img.cols).arg(img.rows));
    textMetrics->append(QString("Channels: %1").arg(img.channels()));
    textMetrics->append(QString("Disk Size: %1 KB").arg(fileInfo.size() / 1024));
    textMetrics->append("\nAwaiting Processing...");
}

void MainWindow::updateMetrics(const QString& techName, double mse, double psnr, double ssim) {
    textMetrics->append("\n--- METRICS ---");
    textMetrics->append(QString("Technique: %1").arg(techName));
    textMetrics->append(QString("MSE:  %1").arg(mse, 0, 'f', 2));
    textMetrics->append(QString("PSNR: %1 dB").arg(psnr, 0, 'f', 2));
    textMetrics->append(QString("SSIM: %1").arg(ssim, 0, 'f', 4));
}

void MainWindow::on_btnLoad_clicked() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open Image", "data/raw", "Images (*.png *.jpg *.jpeg *.tif *.bmp)");
    if (fileName.isEmpty()) return;

    originalImage = cv::imread(fileName.toStdString());
    if (originalImage.empty()) {
        QMessageBox::warning(this, "Error", "Could not load image!");
        return;
    }

    displayImage(originalImage, labelOriginal);
    labelEnhaced->clear();
    labelEnhaced->setText("Awaiting Processing...");
    updateMetadata(fileName, originalImage);
    progressBar->setValue(100);
}

void MainWindow::on_btnApply_clicked() {
    if (originalImage.empty()) {
        QMessageBox::warning(this, "Error", "Please load an image first!");
        return;
    }

    int index = comboTech->currentIndex();
    switch(index) {
    case 0: enhancedImage = rs::clahe(originalImage, 3.0, 8); break;
    case 1: enhancedImage = rs::histEqualize(originalImage); break;
    case 2: enhancedImage = rs::gammaCorrect(originalImage, 0.5); break;
    case 3: enhancedImage = rs::medianFilter(originalImage, 5); break;
    case 4: enhancedImage = rs::unsharpMask(originalImage, 5, 1.0, 1.5); break;
    case 5: enhancedImage = rs::sobelEdge(originalImage); break;
    }

    displayImage(enhancedImage, labelEnhaced);

    cv::Mat refImg = (enhancedImage.channels() == 1) ? rs::toGray(originalImage) : originalImage;
    double mse = rs::computeMSE(refImg, enhancedImage);
    double psnr = rs::computePSNR(refImg, enhancedImage);
    double ssim = rs::computeSSIM(refImg, enhancedImage);

    updateMetrics(comboTech->currentText(), mse, psnr, ssim);
}

void MainWindow::on_btnExport_clicked() {
    if (enhancedImage.empty()) {
        QMessageBox::warning(this, "Error", "No processed image to save!");
        return;
    }
    QString fileName = QFileDialog::getSaveFileName(this, "Save Image", "data/enhanced/output.png", "PNG (*.png);;JPEG (*.jpg)");
    if (fileName.isEmpty()) return;
    if (cv::imwrite(fileName.toStdString(), enhancedImage)) {
        QMessageBox::information(this, "Success", "Image saved successfully!");
    }
}

void MainWindow::on_btnReset_clicked() {
    if (!originalImage.empty()) {
        enhancedImage = cv::Mat();
        displayImage(originalImage, labelEnhaced);
        labelEnhaced->setText("Reset to Original");
        updateMetadata("Reset", originalImage);
    }
}