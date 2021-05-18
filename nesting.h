#pragma once

#include <QWidget>

class Layouts : public QWidget {

  public:
    Layouts(QWidget *parent = nullptr);


   private slots:
     void on_pushButton_clicked();
};
