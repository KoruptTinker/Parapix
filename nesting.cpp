#include <QVBoxLayout>
#include <QPushButton>
#include <QListWidget>
#include "nesting.h"
#include <QFileDialog>
#include <QMessageBox>

Layouts::Layouts(QWidget *parent)
    : QWidget(parent) {

  auto *vbox = new QVBoxLayout();
  auto *hbox = new QHBoxLayout(this);


  auto *lw = new QListWidget(this);

  auto *log_button = new QPushButton("Laplacian Of Gaussian", this);
  auto *contrastStetching_button = new QPushButton("Contast Stretching", this);
  auto *bluring_button = new QPushButton("Blurring", this);
  auto *edgeDetection_button = new QPushButton("Edge Detection", this);
  auto *uploadFile_button = new QPushButton("Upload File",this);

  const QSize btnSize = QSize(150, 60);
  log_button->setFixedSize(btnSize);
  contrastStetching_button->setFixedSize(btnSize);
  bluring_button->setFixedSize(btnSize);
  edgeDetection_button->setFixedSize(btnSize);
  uploadFile_button->setFixedSize(btnSize);


  QPalette pal = log_button->palette();
  pal.setBrush(QPalette::Button, QColor(Qt::black));
  pal.setBrush(QPalette::ButtonText,QColor(Qt::white));
  log_button->setFlat(true);
  log_button->setAutoFillBackground(true);
  log_button->setPalette(pal);
  log_button->update();

  contrastStetching_button->setFlat(true);
  contrastStetching_button->setAutoFillBackground(true);
  contrastStetching_button->setPalette(pal);
  contrastStetching_button->update();

  bluring_button->setFlat(true);
  bluring_button->setAutoFillBackground(true);
  bluring_button->setPalette(pal);
  bluring_button->update();

  edgeDetection_button->setFlat(true);
  edgeDetection_button->setAutoFillBackground(true);
  edgeDetection_button->setPalette(pal);
  edgeDetection_button->update();

  QPalette pal1 = log_button->palette();
  pal1.setBrush(QPalette::Button, QColor(Qt::yellow));
  pal1.setBrush(QPalette::ButtonText,QColor(Qt::black));
  uploadFile_button->setFlat(true);
  uploadFile_button->setAutoFillBackground(true);
  uploadFile_button->setPalette(pal1);
  uploadFile_button->update();


  vbox->setSpacing(30);  //Add space between buttons
  vbox->addWidget(uploadFile_button);
  vbox->addStretch(1);
  vbox->addWidget(log_button);
  vbox->addWidget(contrastStetching_button);
  vbox->addWidget(edgeDetection_button);
  vbox->addWidget(bluring_button);
  vbox->addStretch(1);

  hbox->addWidget(lw);
  hbox->addSpacing(15);
  hbox->addLayout(vbox);

  setLayout(hbox);

}

//void Layouts::on_pushButton_clicked()
//{
//    QString file_name=QFileDialog::getOpenFileName(this,"Open a file","C://");
//    QMessageBox::information(this,"..",file_name);
//}

