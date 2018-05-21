#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QString>
#include <QDateTime>
#include <QtMath>
#include <QScriptEngine>
#include <QLineEdit>
#include "math_prog.h"
#include "lab2.h"
#include "parser.h"
#include "interval.h"
#include "parser_interval.h"
#include "intervalmethod.h"
#include "combinedmethod.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void showResult(int index, Parser f, Output o, QLineEdit* tb);
    QString handleCustomFunctions();
private:
    Ui::MainWindow *ui;
public slots:
    void aboutQT();
    void aboutHelp();
    void bIOclick();
    void bMCclick();
    void onSpinBox(int value);
    void onSpinBoxFunction(int value);
private slots:
    void on_bG_clicked();
    void on_bI_clicked();
    void on_bC_clicked();
};

#endif // MAINWINDOW_H
