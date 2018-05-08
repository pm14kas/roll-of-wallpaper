#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QString>
#include <QDateTime>
#include <QtMath>
#include <QScriptEngine>
#include "math_prog.h"
#include "lab2.h"
#include "parser.h"
#include "interval.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
public slots:
    void aboutQT();
    void aboutHelp();
    void bIOclick();
    void bMCclick();
    void onSpinBox(int value);
private slots:
    void on_bG_clicked();
};

#endif // MAINWINDOW_H
