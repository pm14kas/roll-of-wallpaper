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
    connect(ui->tbFunc, SIGNAL(valueChanged(int)), this, SLOT(onSpinBoxFunction(int)));

    //ui->action_Help->setVisible(false);
    qsrand(QDateTime::currentMSecsSinceEpoch());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showResult(int index, Parser f, Output o, QLineEdit* tb)
{
    QStringList headers;
    headers.push_back("f(x)");
    if (!(ui->dgvOut->item(0, index)))
    {
        ui->dgvOut->setItem(0, index, new QTableWidgetItem(QString::number(f.calc(o.result))));
    }
    else
    {
        ui->dgvOut->item(0, index)->setText(QString::number(f.calc(o.result)));
    }

    for (unsigned int i = 0; i < o.result.size(); i++)
    {
        if (!(ui->dgvOut->item(i+1, index)))
        {
            ui->dgvOut->setItem(i+1, index, new QTableWidgetItem(QString::number(o.result[i].d)));
        }
        else
        {
            ui->dgvOut->item(i+1, index)->setText(QString::number(o.result[i].d));
        }
        headers.push_back(QString::fromStdString(o.result[i].s));
    }

    ui->dgvOut->setVerticalHeaderLabels(headers);

    //i dont have a clue about qtimespan or whatever, and i dont give a fcuk
    int secs = qFloor(o.timer/1000);
    o.timer = o.timer % 1000;
    int mins = qFloor(secs/60);
    secs = secs%60;
    int hours = qFloor(mins/60);
    mins = mins%60;
    tb->setText(QString::number(hours)+":"+QString::number(mins)+":"+QString::number(secs)+"."+QString::number(o.timer));
    QMessageBox::about(this, "Успех", "Расчет произведен");
}

QString MainWindow::handleCustomFunctions()
{
    QString function = ui->cbFunction->text().replace(',', '.');

    for (int k = ui->cashTomNy->rowCount() - 1; k >= 0 ; k--)
    {
        QString functionName = ui->cashTomNy->item(k,0)->text();
        QString g = function.mid(function.indexOf(functionName) + functionName.size());
        g = g.trimmed();
        if (g[0] != '(') {
            throw Parser::ERROR_PARENTHESIS;
        }
        unsigned int j = 1, i;
        for (i = 1; i < g.size(); i++)
        {
            if (g[i] == '(')
            {
                j++;
            }
            else if (g[i] == ')')
            {
                j--;
            }
            if (!j)
            {
                break;
            }
        }
        if (j)
        {
            throw Parser::ERROR_PARENTHESIS;
        }

        QString variable = g.mid(1, i-1);
        QString tempFunctionName = ui->cashTomNy->item(k, 0)->text();
        QString tempFunctionDefinition = ui->cashTomNy->item(k, 1)->text();

        tempFunctionName = tempFunctionName + "(" + variable + ")";
        tempFunctionDefinition = tempFunctionDefinition.replace("arg", "(" + variable + ")");

        function = function.replace(tempFunctionName, tempFunctionDefinition);

    }
    return function;
}

void MainWindow::aboutQT()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::aboutHelp()
{
    QMessageBox::about(this, "Справка", QString("<style>p{margin:2px;} p.header{margin-left:10px;margin-top:5px;}</style>")+
                       QString("<p class=\"header\"><b>Доступные функции для числовой математики</b>: </p>")+
                       QString("<p><i>abs</i> - модуль</p>")+
                       QString("<p><i>exp</i> - функция экспоненты</p>")+
                       QString("<p><i>ln</i> - логарифм по основанию е</p>")+
                       QString("<p><i>sin</i> - синус</p>")+
                       QString("<p><i>cos</i> - косинус</p>")+
                       QString("<p><i>arctg</i> - арктангенс</p>")+
                       QString("<p><i>tg</i> - тангенс</p>")+
                       QString("<p><i>sqrt</i> - квадратный корень</p>")+
                       QString("<p class=\"header\"><b>Доступные функции для интервальной математики</b>: </p>")+
                       QString("<p><i>abs</i> - модуль</p>")+
                       QString("<p><i>sin</i> - синус</p>")+
                       QString("<p><i>cos</i> - косинус</p>")+
                       QString("<p><i>sqrt</i> - квадратный корень</p>")+
                       QString("<p class=\"header\"><b>Системные переменные:</b></p>")+
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
        Parser f(handleCustomFunctions().replace(',', '.').toStdString());
        double T = ui->tbTmax->text().replace(',', '.').toDouble();
        double L = ui->tbL->text().replace(',', '.').toDouble();
        double r = ui->tbr->text().replace(',', '.').toDouble();
        double eps = ui->tbEps->text().replace(',', '.').toDouble();

        Output result = math_prog::ImitOtzh(f, start,end,T,L, r,eps,ui->checkHJIO->isChecked() );
        showResult(0, f, result, ui->tbIOtime);
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
            start.push_back(KeyValue(ui->dgvXLimInput->item(i,0)->text().replace(',', '.').toStdString(), ui->dgvXLimInput->item(i,1)->text().replace(',', '.').toDouble()));
            end.push_back(KeyValue(ui->dgvXLimInput->item(i,0)->text().replace(',', '.').toStdString(), ui->dgvXLimInput->item(i,2)->text().replace(',', '.').toDouble()));
            if (start[i].d > end[i].d) throw 56;
        }


        unsigned int maxIterations = qAbs(ui->tbSizeTest->text().toUInt());

        Parser f(handleCustomFunctions().replace(',', '.').toStdString());
        Output result;
        result = math_prog::MonteCarlo(f,start, end, maxIterations, ui->checkHJMK->isChecked());
        showResult(1, f, result, ui->tbMCtime);
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

void MainWindow::onSpinBoxFunction(int value)
{
    while (ui->cashTomNy->rowCount() < value)
    {
        int rowcount = ui->cashTomNy->rowCount();
        ui->cashTomNy->setRowCount(rowcount + 1);
    }
    ui->cashTomNy->setRowCount(value);

    for (int i = 0; i < ui->cashTomNy->rowCount(); i++)
    {
        if (!(ui->cashTomNy->item(i, 0)))
        {
            ui->cashTomNy->setItem(i, 0, new QTableWidgetItem("function"+QString::number(i+1)));
        }
        if (!(ui->cashTomNy->item(i, 1)))
        {
            ui->cashTomNy->setItem(i, 1, new QTableWidgetItem("1+1"));
        }
    }
}

void MainWindow::on_bG_clicked()
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
        Parser f(handleCustomFunctions().replace(',', '.').toStdString());
        double iters = ui->tIterG->text().replace(',', '.').toDouble();
        double intersecs = ui->tIntersectionsG->text().replace(',', '.').toDouble();
        double mutates = ui->tMutationG->text().replace(',', '.').toDouble();
        double popul = ui->tPopulG->text().replace(',', '.').toDouble();
        double gens = ui->tGenersG->text().replace(',', '.').toDouble();
        double best = ui->tBestG->text().replace(',', '.').toDouble();

        if (intersecs < 0.0 || intersecs > 1.0)
        {
            QMessageBox::warning(this, "Ошибка", QString("Коэффициент скрещеваний должен быть в диапазоне [0,1]!"));
            return;
        }

        if (mutates < 0.0 || mutates > 1.0)
        {
            QMessageBox::warning(this, "Ошибка", QString("Коэффициент мутаций должен быть в диапазоне [0,1]!"));
            return;
        }

        int EliteCount = qFloor((double)popul * best);
        if (EliteCount <= 0)
        {
            QMessageBox::warning(this, "Ошибка", QString("Указан слишком маленький процент лучших особей. В расчетах будет выбираться только одна лучшая особь!"));
            EliteCount = 1;
        }
        if (EliteCount > popul)
        {

            QMessageBox::warning(this, "Ошибка", QString("Указан слишком большой процент лучших особей. В расчётах все особи будут считаться лучшими, минимум скорее всего не будет достигнут."));
            EliteCount = popul;
        }

        Output result = lab2::calc(f, start, end, iters, intersecs, mutates, popul, gens, EliteCount);

        showResult(2, f, result, ui->tbGtime);
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

void MainWindow::on_bI_clicked()
{
    try
    {
        std::vector<KeyValueInterval> limits;
        for (int i = 0; i < ui->dgvXLimInput->rowCount(); i++)
        {
            limits.push_back(
                        KeyValueInterval(ui->dgvXLimInput->item(i,0)->text().replace(',', '.').toStdString(),
                                             Interval(
                                                 ui->dgvXLimInput->item(i,1)->text().replace(',', '.').toDouble(),
                                                 ui->dgvXLimInput->item(i,2)->text().replace(',', '.').toDouble()
                                             )
                                         )
                        );
            if (limits[i].d.left > limits[i].d.right) throw 56;
        }
        ParserInterval f2(handleCustomFunctions().replace(',', '.').toStdString());
        Parser f1(handleCustomFunctions().replace(',', '.').toStdString());
        double epsilon = ui->tbEpsilon->text().replace(',', '.').toDouble();

        //Пример использования
//        std::vector< std::vector<Interval>> interval22 = IntervalMethod::diff2Copy(f2, limits);

//        QString kek;

//        for (int i = 0; i < interval22.size(); i++)
//        {
//            for (int j = 0; j < interval22.size(); j++)
//            {
//                kek += QString::fromStdString(interval22[i][j].toString()) + QString(" ");
//            }
//            kek += "\n";
//        }

//        QMessageBox::about(this, " ", kek);

        OutputInterval o = IntervalMethod::calc(f1, f2, limits, epsilon);


        QStringList headers;
        headers.push_back("f(x)");
        if (!(ui->dgvOut->item(0, 3)))
        {
            ui->dgvOut->setItem(0, 3, new QTableWidgetItem(QString::fromStdString(o.fmin.toString())));
        }
        else
        {
            ui->dgvOut->item(0, 3)->setText(QString::fromStdString(o.fmin.toString()));
        }

        for (unsigned int i = 0; i < o.result.size(); i++)
        {
            if (!(ui->dgvOut->item(i+1, 3)))
            {
                ui->dgvOut->setItem(i+1, 3, new QTableWidgetItem(QString::fromStdString(o.result[i].d.toString())));
            }
            else
            {
                ui->dgvOut->item(i+1, 3)->setText(QString::fromStdString(o.result[i].d.toString()));
            }
            headers.push_back(QString::fromStdString(o.result[i].s));
        }

        ui->dgvOut->setVerticalHeaderLabels(headers);

        //i dont have a clue about qtimespan or whatever, and i dont give a fcuk
        int secs = qFloor(o.timer/1000);
        o.timer = o.timer % 1000;
        int mins = qFloor(secs/60);
        secs = secs%60;
        int hours = qFloor(mins/60);
        mins = mins%60;
        ui->tbItime->setText(QString::number(hours)+":"+QString::number(mins)+":"+QString::number(secs)+"."+QString::number(o.timer));
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

void MainWindow::on_bC_clicked()
{
    try
    {
        std::vector<KeyValueInterval> limits;
        for (int i = 0; i < ui->dgvXLimInput->rowCount(); i++)
        {
            limits.push_back(
                        KeyValueInterval(ui->dgvXLimInput->item(i,0)->text().replace(',', '.').toStdString(),
                                             Interval(
                                                 ui->dgvXLimInput->item(i,1)->text().replace(',', '.').toDouble(),
                                                 ui->dgvXLimInput->item(i,2)->text().replace(',', '.').toDouble()
                                             )
                                         )
                        );
            if (limits[i].d.left > limits[i].d.right) throw 56;
        }
        ParserInterval f2(handleCustomFunctions().replace(',', '.').toStdString());
        Parser f1(handleCustomFunctions().replace(',', '.').toStdString());

        double iters = ui->tIterG_2->text().replace(',', '.').toDouble();
        double intersecs = ui->tIntersectionsG_2->text().replace(',', '.').toDouble();
        double mutates = ui->tMutationG_2->text().replace(',', '.').toDouble();
        double popul = ui->tPopulG_2->text().replace(',', '.').toDouble();
        double gens = ui->tGenersG_2->text().replace(',', '.').toDouble();
        double best = ui->tBestG_2->text().replace(',', '.').toDouble();
        double maxWidth = ui->maxIntervalWidth->text().replace(',', '.').toDouble();

        if (intersecs < 0.0 || intersecs > 1.0)
        {
            QMessageBox::warning(this, "Ошибка", QString("Коэффициент скрещеваний должен быть в диапазоне [0,1]!"));
            return;
        }

        if (mutates < 0.0 || mutates > 1.0)
        {
            QMessageBox::warning(this, "Ошибка", QString("Коэффициент мутаций должен быть в диапазоне [0,1]!"));
            return;
        }

        int EliteCount = qFloor((double)popul * best);
        if (EliteCount <= 0)
        {
            QMessageBox::warning(this, "Ошибка", QString("Указан слишком маленький процент лучших особей. В расчетах будет выбираться только одна лучшая особь!"));
            EliteCount = 1;
        }
        if (EliteCount > popul)
        {

            QMessageBox::warning(this, "Ошибка", QString("Указан слишком большой процент лучших особей. В расчётах все особи будут считаться лучшими, минимум скорее всего не будет достигнут."));
            EliteCount = popul;
        }

        //Output result = lab2::calc(f, start, end, iters, intersecs, mutates, popul, gens, EliteCount);

        Output o = CombinedMethod::calc(f1, f2, limits, iters, intersecs, mutates, popul, gens, EliteCount, maxWidth);

        showResult(4, f1, o, ui->tbCtime);
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
