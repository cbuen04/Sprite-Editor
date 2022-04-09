#include "model.h"
#include "canvas.h"
#include <QTimer>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFileDialog>

/**
 * @brief Model::Model
 * This is the constructor for the model. It initializes all the parameters needed to create a
 * drawing canvas
 * @param imgWidth The width of the images in pixels
 * @param imgHeight The height of the images in pixels
 * @param parent The QObject parent of the model
 */

Model::Model(int imgWidth, int imgHeight, QObject *parent)
    : QObject{parent}, imgWidth(imgWidth), imgHeight(imgHeight)
{
    QPixmap pixmap(imgWidth, imgHeight);
    pixmap.fill(Qt::transparent);
    QImage img = pixmap.toImage();
    frames.push_back(img);
}
/**
 * @brief Model::copyFrame
 * This method copies the current frame into an added frame.
 */
void Model::copyFrame(){
    QImage img = frames.at(currFrame).copy();
    frames.insert(frames.begin()+currFrame+1, img);
    emit sendNextFrame(frames.at(++currFrame));
    emit updateCurrentFrameLabel(currFrame, frames.size());

}
/**
 * @brief Model::addFrame
 * This method adds a new image frame to the model and sends an update of the current
 * frame index to the view.
 */
void Model::addFrame(){
    QPixmap pixmap(imgWidth, imgHeight);
    pixmap.fill(Qt::transparent);
    QImage img = pixmap.toImage();
    frames.push_back(img);
    emit updateCurrentFrameLabel(currFrame, frames.size());
}
/**
 * @brief Model::nextFrame
 * This is a slot that changes the frame that is currently being
 * displayed on the preview and canvas to the next available frame.
 */
void Model::nextFrame(){
    if(currFrame < (int)frames.size()-1){
        emit sendNextFrame(frames.at(++currFrame));
    }

    emit updateCurrentFrameLabel(currFrame, frames.size());
}
/**
 * @brief Model::prevFrame
 * This is a slot that changes the frame currently being displayed to
 * the previous frame.
 */
void Model::prevFrame(){
    if(currFrame > 0){
        emit sendPreviousFrame(frames.at(--currFrame));
    }

    emit updateCurrentFrameLabel(currFrame, frames.size());
}
/**
 * @brief Model::receiveUpdatedCanvasFrame
 * This method assigns an image to the current frame.
 * @param img
 */
void Model::receiveUpdatedCanvasFrame(QImage& img){
    frames.at(currFrame) = img;
}
/**
 * @brief Model::deleteFrame
 * This method removes an image from the vector of images (and thus the animation).
 */
void Model::deleteFrame(){
    if(!animationRunning && currFrame != 0){
        frames.erase(frames.begin()+currFrame);
        currAnimationFrame = 0;
        currFullscreenFrame = 0;
        emit sendPreviousFrame(frames.at(--currFrame));
    }else if (!animationRunning && currFrame == 0 && frames.size()>1){
        frames.erase(frames.begin());
        currAnimationFrame = 0;
        currFullscreenFrame = 0;
        emit sendNextFrame(frames.at(currFrame));
    } else if(animationRunning){
        deletingFrame = true;
    }

    emit updateCurrentFrameLabel(currFrame, frames.size());
}
/**
 * @brief Model::deleteFrameRunning
 * This method removes an image from the vector of images while the animation is
 * is running. Special conditions are considered to prevent the animation from
 * attempting to display an already-deleted frame.
 */
void Model::deleteFrameRunning(){
    if(currFrame != 0){
        emit sendPreviousFrame(frames.at(--currFrame));

        frames.erase(frames.begin()+currFrame+1);
        currAnimationFrame = 0;
        deletingFrame = false;
        emit startAnimation();
    }else if (currFrame == 0 && frames.size()>1){
        frames.erase(frames.begin());
        emit sendNextFrame(frames.at(currFrame));
        currAnimationFrame = 0;
        deletingFrame = false;

        emit startAnimation();
    }
    else if (currFrame == 0 && frames.size() == 1){
        deletingFrame = false;
        emit startAnimation();
    }
    emit updateCurrentFrameLabel(currFrame, frames.size());

}
/**
 * @brief Model::emitSendNextAnimationFrame
 * This method is a wrapper for the sendNextAnimationFrame signal. It also increments
 * currAnimationFrame.
 */
void Model::emitSendNextAnimationFrame(){
    emit sendNextAnimationFrame(frames.at(currAnimationFrame));
    currAnimationFrame = (currAnimationFrame+1) % frames.size();
}
/**
 * @brief Model::emitSendNextCanvasAnimationFrame
 * This method is a wrapper for the nextCanvasAnimationFrame signal.
 */
void Model::emitSendNextCanvasAnimationFrame(){
    emit sendNextCanvasAnimationFrame(frames.at(currFullscreenFrame));
}
/**
 * @brief Model::startAnimationAfterDelete
 * This method is a slot that is used to restart the animation cycle when the user
 * deletes a frame while the animation is running.
 */
void Model::startAnimationAfterDelete(){
    emitSendNextAnimationFrame();
}

/**
 * @brief Model::incrementAnimation
 * This method increments the animation preview to the next frame, or handles
 * pausing the animation to safely delete a frame.
 */
void Model::incrementAnimation(){

    if(animationRunning && !deletingFrame){
        QTimer::singleShot(frameRate, this, &Model::emitSendNextAnimationFrame);

        emit enableDelete(true);
    }
    else if (animationRunning && deletingFrame){

        emit enableDelete(false);
        QTimer::singleShot(frameRate, this, &Model::emitPauseAnimation);
    }
}
/**
 * @brief Model::incrementFullscreenAnimation
 * This method progresses the full screen animation to the next frame.
 */
void Model::incrementFullscreenAnimation(){
    QTimer::singleShot(frameRate, this, &Model::emitSendNextCanvasAnimationFrame);
    currFullscreenFrame = (currFullscreenFrame+1) % frames.size();
}
/**
 * @brief Model::setPlayPauseBool
 * @param play, a boolean that is true if the animation is running
 * This method sets the animationRunning boolean used to safely delete frames
 * while the animation is running.
 */
void Model::setPlayPauseBool(bool play){
    if(!animationRunning){
        animationRunning = play;
        emitSendNextAnimationFrame();
    } else {
        animationRunning = play;
    }
}

/**
 * @brief Model::frameRateChanged
 * @param framesPerSecond The new frames per second value for the frame rate.
 * This method updates the frame rate.
 */
void Model::frameRateChanged(int framesPerSecond){
    frameRate = 1000 / framesPerSecond;
}

/**
 * @brief Model::emitPauseAnimation
 * This method is a wrapper for the pauseAnimation signal.
 */
void Model::emitPauseAnimation(){
    emit pauseAnimation();
}


/**
 * @brief Model::saveClicked
 * @param filename The filename to save to.
 * This method writes a sprite project to the specified filename.
 */
void Model::saveClicked(QString filename){

    QJsonObject saveSprite;
    write(saveSprite, filename);
}

/**
 * @brief Model::write
 * this method creates a json file of the frames and stores each individual pixel
 * this allows to save a sprite project
 * @param json
 */
void Model::write(QJsonObject &json, QString filename) const {
    json["width"] = frames.at(0).width();
    json["height"] = frames.at(0).height();
    json["numberOfFrames"] = (int)frames.size();
    QJsonArray frameArray;

    for(int l = 0; l < (int)frames.size(); l++){
        QJsonArray heightArray;
        for(int i = 0; i < frames.at(l).height(); i++){
            QJsonArray widthArray;
            for(int j = 0; j < frames.at(l).width(); j++){
                QColor color = frames.at(l).pixelColor(j, i);

                QJsonArray colorArray;
                colorArray.push_back(color.red());
                colorArray.push_back(color.green());
                colorArray.push_back(color.blue());
                colorArray.push_back(color.alpha());

                widthArray.push_back(colorArray);
            }
            heightArray.push_back(widthArray);
        }
        QString frameName = "frame";
        frameName.append(QString::number(l));
        json[frameName] = heightArray;
        frameArray.push_back(json[frameName]);
    }
    json["frames"] = frameArray;

    QJsonDocument doc;
    doc.setObject(json);
    QFile file(filename);
    file.open(QFile::WriteOnly | QFile::Text | QFile::Truncate);
    file.write(doc.toJson());
    file.close();
}

/**
 * @brief Model::read
 * this method deserializes a json file, which allows us to retrieve a saved project and
 * load it in the sprite editor
 * @param json
 */
void Model::read(QJsonObject &json) {
    numberOfFrames = json["numberOfFrames"].toInt();
    QJsonArray frameArray = json["frames"].toArray();
    frames.pop_back();
    for(int i = 0; i < frameArray.size(); i++){
        QImage imgAtFrame(imgWidth, imgHeight, QImage::Format_RGBA64);

        QJsonArray heights = frameArray.at(i).toArray();
        for(int j = 0; j < heights.size(); j++){
            QJsonArray widths = heights.at(j).toArray();
            for(int k = 0; k < widths.size(); k++){
                QJsonArray colorArray = widths.at(k).toArray();
                QColor color;
                color.setRed(colorArray.at(0).toInt());
                color.setGreen(colorArray.at(1).toInt());
                color.setBlue(colorArray.at(2).toInt());
                color.setAlpha(colorArray.at(3).toInt());

                imgAtFrame.setPixel(k, j, qRgba(color.red(), color.green(), color.blue(), color.alpha()));
            }
        }
        frames.push_back(imgAtFrame);
    }
}

