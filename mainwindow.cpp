#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <QFileDialog>
#include<QMessageBox>
#include <QPixmap>
#include "./src/headers/Parapix.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    this->file_name=QFileDialog::getOpenFileName(this,"Open A File","./src/data/test_data/test");
    std::string utf8_text = file_name.toUtf8().constData();
    ui->label->setPixmap(QPixmap(file_name));
    ui->label->setScaledContents(true);
    this->utf8_text = this->file_name.toUtf8().constData();
    this->tempArray=&this->utf8_text[0];
}

void MainWindow::on_pushButton_2_clicked()
{
    this->tempArray=&this->utf8_text[0];
    std::string temp2="./src/outputs/temporary";
    temp2+=std::to_string(this->counter)+".png";
    this->counter++;
    char* temp2Array=&temp2[0];
    LOGParallel(this->tempArray, temp2Array);
    ui->label->setPixmap(QPixmap(temp2Array));
    ui->label->setScaledContents(true);
    utf8_text=temp2;
}

void MainWindow::on_pushButton_3_clicked()
{
    this->tempArray=&this->utf8_text[0];
    std::string temp2="./src/outputs/temporary";
    temp2+=std::to_string(this->counter)+".png";
    this->counter++;
    char* temp2Array=&temp2[0];
    contrastStretchSerial(this->tempArray, temp2Array);
    ui->label->setPixmap(QPixmap(temp2Array));
    ui->label->setScaledContents(true);
    utf8_text=temp2;
}

void MainWindow::on_pushButton_4_clicked()
{
    this->tempArray=&this->utf8_text[0];
    std::string temp2="./src/outputs/temporary";
    temp2+=std::to_string(this->counter)+".png";
    this->counter++;
    char* temp2Array=&temp2[0];
    blurParallel(this->tempArray, temp2Array);
    ui->label->setPixmap(QPixmap(temp2Array));
    ui->label->setScaledContents(true);
    utf8_text=temp2;
}

void MainWindow::on_pushButton_5_clicked()
{
    this->tempArray=&this->utf8_text[0];
    std::string temp2="./src/outputs/temporary";
    temp2+=std::to_string(this->counter)+".png";
    this->counter++;
    char* temp2Array=&temp2[0];
    edgeDetection(this->tempArray, temp2Array);
    ui->label->setPixmap(QPixmap(temp2Array));
    ui->label->setScaledContents(true);
    utf8_text=temp2;
}
