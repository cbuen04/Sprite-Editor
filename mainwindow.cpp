#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "model.h"
#include "canvas.h"
#include <QLabel>
#include "graphicsscene.h"
#include <QGraphicsView>
#include <QGridLayout>
#include <cmath>
#include <string>
#include "openingwindow.h"
#include <QFileDialog>

using std::fmin;

/**
 * @brief MainWindow::MainWindow creates the main window application. It takes a Model object as a parameter to serve as the
 * container for application data.
 * @param model
 * @param parent
 */
MainWindow::MainWindow(Model&model, QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    int scaledCanvasWidth = 0;
    int scaledCanvasHeight = 0;

    int maxWidth(ui->canvasLabel->width());
    int maxHeight(ui->canvasLabel->height());
    int srcWidth(model.imgWidth);
    int srcHeight(model.imgHeight);


    calculateAspectRatioFit(srcWidth, srcHeight, maxWidth, maxHeight, scaledCanvasWidth, scaledCanvasHeight);

    Canvas *c = new Canvas(model.frames.at(0), scaledCanvasWidth, scaledCanvasHeight, ui->canvasLabel);

    c->move((ui->canvasLabel->width()/2) -(scaledCanvasWidth/2), (ui->canvasLabel->height()/2) - (scaledCanvasHeight/2));

    ui->canvasLabel->setScaledContents(true);

    // Initializing slider
    ui->sizeSlider->setMaximum(40);
    ui->sizeSlider->setMinimum(1);
    ui->sizeSlider->setValue(5);


    ui->frameRateBox->setMinimum(1);
    ui->frameRateBox->setMaximum(30);
    ui->frameRateBox->setValue(5);
    model.frameRate = 200;

    ui->playPauseAnimationButton->setText("Play");

    c->brushSize = 5; // setting initial brush size to match the slider initial value (kinda janky)
    c->brushColor = Qt::black; // initializing brush color
    ui->colorPrevWidget->setStyleSheet("background-color: black;");

    // brush slots
    connect(ui->sizeSlider,
            &QSlider::valueChanged,
            c,
            &Canvas::brushSizeChanged);
    connect(ui->eraseButton,
            &QPushButton::clicked,
            c,
            &Canvas::eraseSelected);
    connect(ui->brushButton,
            &QPushButton::clicked,
            c,
            &Canvas::brushSelected);
    connect(ui->brushButton,
            &QPushButton::clicked,
            this,
            &MainWindow::toggleBrushButton);
    connect(ui->eraseButton,
            &QPushButton::clicked,
            this,
            &MainWindow::toggleEraserButton);
    connect(ui->colorPickButton,
            &QPushButton::clicked,
            c,

            // changing the color of the brush as well as saving presets
            &Canvas::colorDialogSelected);
    connect(c,
            &Canvas::firstHistoryChanged,
            ui->colorHist1,
            &QPushButton::setStyleSheet);
    connect(c,
            &Canvas::secondHistoryChanged,
            ui->colorHist2,
            &QPushButton::setStyleSheet);
    connect(c,
            &Canvas::thirdHistoryChanged,
            ui->colorHist3,
            &QPushButton::setStyleSheet);
    connect(c,
            &Canvas::fourthHistoryChanged,
            ui->colorHist4,
            &QPushButton::setStyleSheet);

    connect(c,
            &Canvas::newCurrentColor,
            ui->colorPrevWidget,
            &QWidget::setStyleSheet);

    //connecting color recents
    connect(ui->colorHist1,
            &QPushButton::clicked,
            c,
            &Canvas::firstHistorySelcted);
    connect(ui->colorHist2,
            &QPushButton::clicked,
            c,
            &Canvas::secondHistorySelcted);
    connect(ui->colorHist3,
            &QPushButton::clicked,
            c,
            &Canvas::thirdHistorySelcted);
    connect(ui->colorHist4,
            &QPushButton::clicked,
            c,
            &Canvas::fourthHistorySelcted);

    //connectng frame functionality
    connect(ui->plusButton,
            &QPushButton::clicked,
            &model,
            &Model::addFrame);

    connect(ui->nextFrameButton,
            &QPushButton::clicked,
            &model,
            &Model::nextFrame);
    connect(&model,
            &Model::sendNextFrame,
            c,
            &Canvas::nextFrameChanged);

    connect(ui->previousFrameButton,
            &QPushButton::clicked,
            &model,
            &Model::prevFrame);
    connect(&model,
            &Model::sendPreviousFrame,
            c,
            &Canvas::prevFrameChanged);
    connect(c,
            &Canvas::updateModelFrames,
            &model,
            &Model::receiveUpdatedCanvasFrame);
    connect(ui->deleteFrameButton,
            &QPushButton::clicked,
            &model,
            &Model::deleteFrame);

    connect(&model,
            &Model::sendNextAnimationFrame,
            &model,
            &Model::incrementAnimation);

    connect(&model,
            &Model::sendNextAnimationFrame,
            this,
            &MainWindow::drawAnimation);

    // connect the play/pause button to preview animation
    connect(this,
            &MainWindow::sendPlayValue,
            &model,
            &Model::setPlayPauseBool);

    connect(ui->playPauseAnimationButton,
            &QPushButton::clicked,
            this,
            &MainWindow::playPauseAnimation);

    connect(ui->frameRateBox,
            &QSpinBox::valueChanged,
            &model,
            &Model::frameRateChanged);
    connect(&model,
            &Model::pauseAnimation,
            &model,
            &Model::deleteFrameRunning);
    connect(&model,
            &Model::enableDelete,
            ui->deleteFrameButton,
            &QPushButton::setEnabled);
    connect(&model,
            &Model::startAnimation,
            &model,
            &Model::startAnimationAfterDelete);

    // connect play canvas animation
    connect(ui->playFullscreenButton,
            &QPushButton::clicked,
            this,
            &MainWindow::openFullScreenPreview);
    connect(this,
            &MainWindow::getFirstFrame,
            &model,
            &Model::incrementFullscreenAnimation);
    connect(&model,
            &Model::sendNextCanvasAnimationFrame,
            this->preview,
            &FullscreenPreview::receiveFrame);
    connect(this->preview,
            &FullscreenPreview::updatedImage,
            &model,
            &Model::incrementFullscreenAnimation);
    connect(this->preview,
            &FullscreenPreview::enableMainWindow,
            this,
            &MainWindow::enableMainWindowAfterHide);
    connect(this->preview,
            &FullscreenPreview::enableMainWindow,
            c,
            &Canvas::recieveCanDraw);

    //Conect Total frame count
    connect(&model,
            &Model::updateCurrentFrameLabel,
            this,
            &MainWindow::setTextCurrentFrameLabel);

    //CopyFrame
    connect(ui->copyFrameButton,
            &QPushButton::clicked,
            &model,
            &Model::copyFrame);

    //toolbar
    connect(ui->actionsave,
            &QAction::triggered,
            this,
            &MainWindow::showSaveWindow);
    connect(this,
            &MainWindow::sendSaved,
            &model,
            &Model::saveClicked);
}
/**
 * @brief MainWindow::drawAnimation
 * @param img
 */
void MainWindow::drawAnimation(QImage &img){
    QPixmap p(QPixmap::fromImage(img));
    int scaledWidth = 0;
    int scaledHeight = 0;
    int maxWidth(ui->animationLabel->width());
    int maxHeight(ui->animationLabel->height());
    int srcWidth(img.width());
    int srcHeight(img.height());


    calculateAspectRatioFit(srcWidth, srcHeight, maxWidth, maxHeight, scaledWidth, scaledHeight);


    ui->animationLabel->setPixmap(p.scaled(scaledWidth, scaledHeight, Qt::KeepAspectRatio));


}

MainWindow::~MainWindow(){
    delete ui;
}
/**
 * @brief MainWindow::calculateAspectRatioFit calulates the aspect ratio and then a scaled width and height
 * @param srcWidth orginal width
 * @param srcHeight original height
 * @param maxWidth - maxium width
 * @param maxHeight maxium height
 * @param scaledWidth - newly scaled width
 * @param scaledHeight - newly scaled height
 */
void MainWindow::calculateAspectRatioFit(int srcWidth, int srcHeight, int maxWidth, int maxHeight, int& scaledWidth, int& scaledHeight){
    double ratio = fmin(maxWidth/(double)srcWidth, maxHeight/(double)srcHeight);

    scaledWidth = srcWidth * ratio;
    scaledHeight = srcHeight * ratio;
}
/**
 * @brief MainWindow::playPauseCanvasAnimation - sets text for the fullscreen animation
 */
void MainWindow::playPauseCanvasAnimation(){
    if(canvasAnimationButtonPlay){
        ui->playFullscreenButton->setText("Pause");
    } else {
        ui->playFullscreenButton->setText("Play Fullscreen");
    }

    emit sendCanvasPlayValue(canvasAnimationButtonPlay);
    canvasAnimationButtonPlay = !canvasAnimationButtonPlay;
}
/**
 * @brief MainWindow::playPauseAnimation
 */
void MainWindow::playPauseAnimation()
{
    if(animationButtonPlay){
        ui->playPauseAnimationButton->setText("Pause");
    } else {
        ui->playPauseAnimationButton->setText("Play Preview");
    }

    emit sendPlayValue(animationButtonPlay);
    animationButtonPlay = !animationButtonPlay;
}
/**
 * @brief MainWindow::setTextCurrentFrameLabel
 * @param curr
 * @param total
 */
void MainWindow::setTextCurrentFrameLabel(int curr, int total){
    QString s = QString::number(curr+1) + "/" + QString::number(total);
    ui->currentFrameLabel->setText(s);
}


/**
 * @brief MainWindow::openFullScreenPreview - slot for on click for the play fullscreen will display the sprite in actual size even when small
 */
void MainWindow::openFullScreenPreview(){
    preview->show();
    preview->animationRunning = true;
    emit getFirstFrame();
    this->setDisabled(true);
}

/**
 * @brief MainWindow::enableMainWindowAfterHide - slot to enable mainwindow again after the slot is hidden.
 */
void MainWindow::enableMainWindowAfterHide(){
    this->setDisabled(false);
}


void MainWindow::showSaveWindow(bool){
    QString filter = "Sprites (*.ssp)";
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Sprite"), QDir::homePath() , filter, &filter); // homepath MAY NOT WORK FOR ALL OS
    emit sendSaved(fileName);
}

/**
 * @brief MainWindow::toggleBrushButton
 * Toggles if the brush button is selected or not.
 */
void MainWindow::toggleBrushButton(){
    if(!brushSelected){
        brushSelected = true;
        eraserSelected = false;
        ui->brushButton->setStyleSheet("QPushButton{ color: black; border: 1px solid black; } QPushButton:hover{ background-color: rgba(0,0,0,0.05); color: black;} "
                                       "QPushButton:pressed{ background-color: rgba(0,0,0,0.1); }");
        ui->eraseButton->setStyleSheet("QPushButton{ color: black; } QPushButton:hover{ background-color: rgba(0,0,0,0.05); color: black;} "
                                       "QPushButton:pressed{ background-color: rgba(0,0,0,0.1); }");
    }
}

/**
 * @brief MainWindow::toggleEraserButton
 * Toggles if the eraser button is selected or not.
 */
void MainWindow::toggleEraserButton(){
    if(!eraserSelected){
        brushSelected = false;
        eraserSelected = true;
        ui->eraseButton->setStyleSheet("QPushButton{ color: black; border: 1px solid black; } QPushButton:hover{ background-color: rgba(0,0,0,0.05); color: black;} "
                                       "QPushButton:pressed{ background-color: rgba(0,0,0,0.1); }");
        ui->brushButton->setStyleSheet("QPushButton{ color: black; } QPushButton:hover{ background-color: rgba(0,0,0,0.05); color: black;} "
                                       "QPushButton:pressed{ background-color: rgba(0,0,0,0.1); }");
    }
}
