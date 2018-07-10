#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include "engine.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_readButton1_clicked()
{
    QString path1 = QFileDialog::getOpenFileName(this,
               tr("Load first img data to workspace [1]"), "",
               tr("DAT (*.dat);"));
        ui->path1->setText(path1);

    QString path2 = QFileDialog::getOpenFileName(this,
                tr("Load second img data to workspace [2]"), "",
                tr("DAT (*.dat);"));
        ui->path2->setText(path2);


       auto workSpace = std::make_unique<WorkSpace>(path1, path2);

       workSpace->findCorners(*workSpace);

       workSpace->calculateScaleMatrix(*workSpace);
       workSpace->calculateRotationMatrix(*workSpace);
       workSpace->calculateTranslateMatrix(*workSpace);
       workSpace->calculateTransformation(*workSpace);

       float me = workSpace->returnMatchingError(*workSpace);

       ui->textBrowser->append("TRANSFORMATION:");
       ui->textBrowser->append(QString::fromStdString("[X] = " + std::to_string(workSpace->tx) + " + [" + std::to_string(workSpace->m*workSpace->cosfi) + " " + std::to_string(-workSpace->m*workSpace->sinfi))+"] [x]");
       ui->textBrowser->append(QString::fromStdString("[Y] = " + std::to_string(workSpace->ty) + " + [" + std::to_string(workSpace->m*workSpace->sinfi) + "  " + std::to_string(workSpace->m*workSpace->cosfi))+ " ] [y] \n");

       ui->textBrowser->append("TRANSLATE: tx = " + QString::fromStdString(std::to_string(workSpace->tx) + " | ty = " + std::to_string(workSpace->ty))+"\n");

       ui->textBrowser->append("SCALE: m = " + QString::fromStdString(std::to_string(workSpace->m)) + "\n");

       ui->textBrowser->append("ROTATION: sin(fi) = " + QString::fromStdString(std::to_string(workSpace->sinfi) + " | cos(fi) = " + std::to_string(workSpace->cosfi) + " | fi in degrees = " + std::to_string(workSpace->degrees))+"\n");

       ui->textBrowser->append("MATCHING ERROR: mE = " + QString::fromStdString(std::to_string(me)));

}
