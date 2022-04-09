#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "model.h"
#include "fullscreenpreview.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/**
 * @brief The MainWindow class is the main interface of the application.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(Model& model,QWidget *parent = nullptr);
    ~MainWindow();


private:
    bool brushSelected = true;
    bool eraserSelected = false;
    bool canvasAnimationButtonPlay = true;
    bool animationButtonPlay = true;
    FullscreenPreview* preview = new FullscreenPreview();
    Ui::MainWindow *ui;
    Canvas *canvas;

    void calculateAspectRatioFit(int srcWidth, int srcHeight, int maxWidth, int maxHeight, int& scaledWidth, int& scaledHeight);

signals:
    void sendPlayValue(bool);
    void sendCanvasPlayValue(bool);
    void getFirstFrame();
    void sendSaved(QString);


public slots:
    void toggleBrushButton();
    void toggleEraserButton();
    void drawAnimation(QImage&);
    void playPauseAnimation();
    void playPauseCanvasAnimation();
    void setTextCurrentFrameLabel(int, int);
    void openFullScreenPreview();
    void enableMainWindowAfterHide();
    void showSaveWindow(bool);
};
#endif
