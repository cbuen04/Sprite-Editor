#include "openingwindow.h"
#include "ui_openingwindow.h"
#include "mainwindow.h"
#include <QJsonDocument>
#include <QJsonObject>

/**
 * @brief OpeningWindow::OpeningWindow This class holds logic for the opening window popup display
 * that asks the user for how they would like to load the pixel editor or create a new one
 * @param model Model to be used in the editor project
 * @param parent QWidget
 */
OpeningWindow::OpeningWindow(Model* model, QWidget *parent) :
    QWidget(parent), w(nullptr),
    ui(new Ui::OpeningWindow)
{
    m = model;
    ui->setupUi(this);
    ui->widthBox->setMinimum(1);
    ui->widthBox->setMaximum(600);
    ui->widthBox->setValue(400);
    ui->heightBox->setMinimum(1);
    ui->heightBox->setMaximum(400);
    ui->heightBox->setValue(400);
    width = 400;
    height = 400;

    ui->startButton->setEnabled(false);
    ui->widthBox->setEnabled(false);
    ui->heightBox->setEnabled(false);

    // Button connections
    connect(ui->createNewButton,
            &QPushButton::pressed,
            this,
            &OpeningWindow::createNewPressed);
    connect(ui->loadButton,
            &QPushButton::pressed,
            this,
            &OpeningWindow::loadExistingPressed);
    connect(ui->startButton,
            &QPushButton::pressed,
            this,
            &OpeningWindow::startPressed);
    connect(ui->widthBox,
            &QSpinBox::valueChanged,
            this,
            &OpeningWindow::widthBoxChanged);
    connect(ui->heightBox,
            &QSpinBox::valueChanged,
            this,
            &OpeningWindow::heightBoxChanged);
}

/**
 * @brief OpeningWindow::~OpeningWindow
 */
OpeningWindow::~OpeningWindow()
{
    delete ui;
    delete w;
}

/**
 * @brief OpeningWindow::loadExistingPressed Logic to load a sprite project from existing
 */
void OpeningWindow::loadExistingPressed(){
    QString fileName = QFileDialog::getOpenFileName(this, "open a file");
    if(!(fileName.size() == 0)){
        QFile file(fileName);
        file.open(QIODevice::ReadOnly | QIODevice::Text);

        QByteArray bytes = file.readAll();
        file.close();
        QJsonDocument document = QJsonDocument::fromJson(bytes);
        QJsonObject fileObj = document.object();

        int imageWidth = fileObj["width"].toInt();
        int imageHeight = fileObj["height"].toInt();

        m = new Model(imageWidth, imageHeight);
        m->read(fileObj);
        w = new MainWindow(*m);
        w->show();
        this->hide();
    }
    else{
        ui->loadButton->setEnabled(true);
        ui->loadButton->setDown(false);
    }
}

/**
 * @brief OpeningWindow::startPressed Logic to create and display a new opening window if the user
 * wants start from new
 */
void OpeningWindow::startPressed(){
    m = new Model(width, height);
    w = new MainWindow(*m);
    w->show();
    this->hide();
}

/**
 * @brief OpeningWindow::widthBoxChanged Logic to connect changing width isntance variables if the user
 * chooses a non default width size
 * @param inputWidth
 */
void OpeningWindow::widthBoxChanged(int inputWidth){
    width = inputWidth;
}

/**
 * @brief OpeningWindow::heightBoxChanged Logic to connect changing height isntance variables if the user
 * chooses a non default height size
 * @param inputHeight
 */
void OpeningWindow::heightBoxChanged(int inputHeight){
    height = inputHeight;
}

/**
 * @brief OpeningWindow::createNewPressed "parent" button that contros what extra buttons are enabled when the user chooses to create a new project
 * from new
 */
void OpeningWindow::createNewPressed(){
    ui->widthBox->setEnabled(true);
    ui->heightBox->setEnabled(true);
    ui->loadButton->setEnabled(false);
    ui->startButton->setEnabled(true);
}
