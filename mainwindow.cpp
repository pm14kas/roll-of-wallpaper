#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->action_Qt, SIGNAL(triggered()), this, SLOT(aboutQT()));
    connect(ui->action_Help, SIGNAL(triggered()), this, SLOT(aboutHelp()));
    connect(ui->bIO, SIGNAL(clicked()), this, SLOT(bIOclick()));
    connect(ui->bMC, SIGNAL(clicked()), this, SLOT(bMCclick()));

    connect(ui->tbPer, SIGNAL(valueChanged(int)), this, SLOT(onSpinBox(int)));

    ui->action_Help->setVisible(false);
    qsrand(QDateTime::currentMSecsSinceEpoch());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::aboutQT()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::aboutHelp()
{
    QMessageBox::about(this, "Справка", QString("<style>p{margin:2px;} p.header{margin-left:10px;margin-top:5px;}</style>")+
                       QString("<p class=\"header\"><b>Доступные функции</b>: </p>")+
                       QString("<p><i>exp</i> - функция экспоненты</p>")+
                       QString("<p><i>ln</i> - логарифм по основанию е</p>")+
                       QString("<p><i>sin</i> - синус</p>")+
                       QString("<p><i>cos</i> - косинус</p>")+
                       QString("<p><i>arctg</i> - арктангенс</p>")+
                       QString("<p><i>tg</i> - тангенс</p>")+
                       QString("<p><i>sqrt</i> - квадратный корень</p>")+
                       QString("<p class=\"header\"><b>Системные переменные:<b></p>")+
                       QString("<p><i>e</i>, <i>pi</i></p>"));
}


//Otzhig
void MainWindow::bIOclick()
{
    try
    {
        std::vector<KeyValue> start;
        std::vector<KeyValue> end;
        for (int i = 0; i < ui->dgvXLimInput->rowCount(); i++)
        {
            start.push_back(KeyValue(ui->dgvXLimInput->item(i,0)->text().replace(',', '.').toStdString(),ui->dgvXLimInput->item(i,1)->text().replace(',', '.').toDouble()));
            end.push_back(KeyValue(ui->dgvXLimInput->item(i,0)->text().replace(',', '.').toStdString(),ui->dgvXLimInput->item(i,2)->text().replace(',', '.').toDouble()));
            if (start[i].d > end[i].d) throw 56;
        }
        Parser f(ui->cbFunction->text().replace(',', '.').toStdString());
        double T = ui->tbTmax->text().replace(',', '.').toDouble();
        double L = ui->tbL->text().replace(',', '.').toDouble();
        double r = ui->tbr->text().replace(',', '.').toDouble();
        double eps = ui->tbEps->text().replace(',', '.').toDouble();

        Output result = math_prog::ImitOtzh(f, start,end,T,L, r,eps,ui->checkHJIO->isChecked() );

        QStringList headers;
        headers.push_back("f(x)");
        if (!(ui->dgvOut->item(0, 0)))
        {
            ui->dgvOut->setItem(0, 0, new QTableWidgetItem(QString::number(f.calc(result.result))));
        }
        else
        {
            ui->dgvOut->item(0, 0)->setText(QString::number(f.calc(result.result)));
        }

        for (unsigned int i = 0; i < result.result.size(); i++)
        {
            if (!(ui->dgvOut->item(i+1, 0)))
            {
                ui->dgvOut->setItem(i+1, 0, new QTableWidgetItem(QString::number(result.result[i].d)));
            }
            else
            {
                ui->dgvOut->item(i+1, 0)->setText(QString::number(result.result[i].d));
            }
            headers.push_back(QString::fromStdString(result.result[i].s));
        }

        ui->dgvOut->setVerticalHeaderLabels(headers);

        //i dont have a clue about qtimespan or whatever, and i dont give a fcuk
        int secs = qFloor(result.timer/1000);
        result.timer = result.timer % 1000;
        int mins = qFloor(secs/60);
        secs = secs%60;
        int hours = qFloor(mins/60);
        mins = mins%60;
        ui->tbIOtime->setText(QString::number(result.timer));
        ui->tbIOtime->setText(QString::number(hours)+":"+QString::number(mins)+":"+QString::number(secs)+"."+QString::number(result.timer));

        QMessageBox::about(this, "Успех", "Расчет произведен");
    }
    catch (int cerr)
    {
        if (cerr == Parser::ERROR_DIVISION_BY_ZERO)
        {
            QMessageBox::critical(this, QString("Внимание!"), QString("Деление на ноль!\t"));
        }
        else if (cerr == Parser::ERROR_PARENTHESIS)
        {
            QMessageBox::critical(this, QString("Внимание!"), QString("Ошибка в скобках!\t"));
        }
        else if (cerr == 10)
        {
            QMessageBox::critical(this, QString("Внимание!"), QString("Ошибка задания переменных!\t"));
        }
        else if (cerr == Parser::ERROR_PARSE)
        {
            QMessageBox::critical(this, QString("Внимание!"), QString("Ошибка парсинга!\t"));
        }
        else
        {
            QMessageBox::critical(this, QString("Внимание!"), QString("Неизвестное исключение!\t"));
        }
    }

    catch (std::string f)
    {
        QMessageBox::warning(this, ("Внимание!"), QString("Неизвестная функция:\t\t\n'") + QString::fromStdString(f)+"'");
    }
}

//Monte-Karlo
void MainWindow::bMCclick()
{
    try
    {
        std::vector<KeyValue> start;
        std::vector<KeyValue> end;
        for (int i = 0; i < ui->dgvXLimInput->rowCount(); i++)
        {
            start.push_back(KeyValue(ui->dgvXLimInput->item(i,0)->text().replace(',', '.').toStdString(),ui->dgvXLimInput->item(i,1)->text().replace(',', '.').toDouble()));
            end.push_back(KeyValue(ui->dgvXLimInput->item(i,0)->text().replace(',', '.').toStdString(),ui->dgvXLimInput->item(i,2)->text().replace(',', '.').toDouble()));
            if (start[i].d > end[i].d) throw 56;
        }

        unsigned int maxIterations = qAbs(ui->tbSizeTest->text().toUInt());

        Parser f(ui->cbFunction->text().replace(',', '.').toStdString());
        Output result;

        result = math_prog::MonteCarlo(f,start, end, maxIterations, ui->checkHJMK->isChecked());

        QStringList headers;
        headers.push_back("f(x)");
        if (!(ui->dgvOut->item(0, 1)))
        {
            ui->dgvOut->setItem(0, 1, new QTableWidgetItem(QString::number(f.calc(result.result))));
        }
        else
        {
            ui->dgvOut->item(0, 1)->setText(QString::number(f.calc(result.result)));
        }

        for (unsigned int i = 0; i < result.result.size(); i++)
        {
            if (!(ui->dgvOut->item(i+1, 1)))
            {
                ui->dgvOut->setItem(i+1, 1, new QTableWidgetItem(QString::number(result.result[i].d)));
            }
            else
            {
                ui->dgvOut->item(i+1, 1)->setText(QString::number(result.result[i].d));
            }
            headers.push_back(QString::fromStdString(result.result[i].s));
        }

        ui->dgvOut->setVerticalHeaderLabels(headers);

        //i dont have a clue about qtimespan or whatever, and i dont give a fcuk
        int secs = qFloor(result.timer/1000);
        result.timer = result.timer % 1000;
        int mins = qFloor(secs/60);
        secs = secs%60;
        int hours = qFloor(mins/60);
        mins = mins%60;
        ui->tbMCtime->setText(QString::number(result.timer));
        ui->tbMCtime->setText(QString::number(hours)+":"+QString::number(mins)+":"+QString::number(secs)+"."+QString::number(result.timer));

        QMessageBox::about(this, "Успех", "Расчет произведен");
    }
    catch (int cerr)
    {
        if (cerr == Parser::ERROR_DIVISION_BY_ZERO)
        {
            QMessageBox::critical(this, QString("Внимание!"), QString("Деление на ноль!\t"));
        }
        else if (cerr == Parser::ERROR_PARENTHESIS)
        {
            QMessageBox::critical(this, QString("Внимание!"), QString("Ошибка в скобках!\t"));
        }
        else if (cerr == 10)
        {
            QMessageBox::critical(this, QString("Внимание!"), QString("Ошибка задания переменных!\t"));
        }
        else if (cerr == 56)
        {
            QMessageBox::critical(this, QString("Внимание!"), QString("Ошибка задания границ!\t"));
        }
        else if (cerr == Parser::ERROR_PARSE)
        {
            QMessageBox::critical(this, QString("Внимание!"), QString("Ошибка парсинга!\t"));
        }
        else
        {
            QMessageBox::critical(this, QString("Внимание!"), QString("Неизвестное исключение!\t"));
        }
    }

    catch (std::string f)
    {
        QMessageBox::warning(this, ("Внимание!"), QString("Неизвестная функция:\t\t\n'") + QString::fromStdString(f)+"'");
    }

    catch (...)
    {
        QMessageBox::critical(this, QString("Внимание!"), QString("Неизвестное исключение!\t"));
    }
}

void MainWindow::onSpinBox(int value)
{
    while (ui->dgvXLimInput->rowCount() < value)
    {
        int rowcount = ui->dgvXLimInput->rowCount();
        ui->dgvXLimInput->setRowCount(rowcount + 1);
    }
    ui->dgvXLimInput->setRowCount(value);

    while (ui->dgvOut->rowCount() < value+1)
    {
        int rowcount = ui->dgvOut->rowCount();
        ui->dgvOut->setRowCount(rowcount + 1);
    }
    ui->dgvOut->setRowCount(value+1);

    QStringList headersLimits;
    QStringList headersOut;

    for (int i = 0; i < ui->dgvXLimInput->rowCount(); i++)
    {
        if (!(ui->dgvXLimInput->item(i, 0)))
        {
            ui->dgvXLimInput->setItem(i, 0, new QTableWidgetItem("x"+QString::number(i+1)));
        }
        if (!(ui->dgvXLimInput->item(i, 1)))
        {
            ui->dgvXLimInput->setItem(i, 1, new QTableWidgetItem("-10"));
        }
        if (!(ui->dgvXLimInput->item(i, 2)))
        {
            ui->dgvXLimInput->setItem(i,2, new QTableWidgetItem("10"));
        }
        headersLimits.push_back("");
    }

    for (int i = 0; i < ui->dgvOut->rowCount(); i++)
    {
        if (!(ui->dgvOut->item(i, 0)))
        {
            ui->dgvOut->setItem(i, 0, new QTableWidgetItem(""));
        }
        if (!(ui->dgvOut->item(i, 1)))
        {
            ui->dgvOut->setItem(i, 1, new QTableWidgetItem(""));
        }
        if (i==0)
            headersOut.push_back("f(x)");
        else
            headersOut.push_back(ui->dgvXLimInput->item(i-1, 0)->text());
    }


    ui->dgvXLimInput->setVerticalHeaderLabels(headersLimits);
    ui->dgvOut->setVerticalHeaderLabels(headersOut);
}
