#include "fullscreenpreview.h"
#include "ui_fullscreenpreview.h"

/**
 * @brief FullscreenPreview::FullscreenPreview - constructor sets up label
 * @param parent
 */
FullscreenPreview::FullscreenPreview(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FullscreenPreview)
{
    ui->setupUi(this);
}

/**
 * @brief FullscreenPreview::~FullscreenPreview - destructor
 */
FullscreenPreview::~FullscreenPreview()
{
    delete ui;
}

/**
 * @brief FullscreenPreview::receiveFrame - receives a frame and updates the preview
 * @param image - image to be displayed
 */
void FullscreenPreview::receiveFrame(QImage& image){
    if(animationRunning){
        ui->frameLabel->setAlignment (Qt::AlignCenter);
        ui->frameLabel->setPixmap(QPixmap::fromImage(image));
        emit updatedImage();
    }
}

/**
 * @brief FullscreenPreview::reject overrides close button to hide and enable the canvas
 */
void FullscreenPreview::reject(){
    this->hide();
    animationRunning = false;

    emit enableMainWindow(animationRunning);
}
