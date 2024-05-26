#include "mainwindow.h"
#include "ui_mainwindow.h"

bool checkedPORT = 0;

bool checkedStart = 0;

int i = 0;
int t=0;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //комбобокс свободных портов
    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
    {
        ui->portComboBox->addItem(serialPortInfo.portName());
    }
    worker = new Graph_worker;
    worker->moveToThread(&drow_thread);
    connect(&drow_thread, &QThread::finished, worker, &QObject::deleteLater);
    connect(this, &MainWindow::operate, worker, &Graph_worker::doWork);
    connect(worker, &Graph_worker::resultReady, this, &MainWindow::handleResults);
    connect(&serialPort, &QSerialPort::readyRead, this, &MainWindow::draw);

    connect(this, &MainWindow::setX, worker, &Graph_worker::setX);
    connect(this, &MainWindow::setY, worker, &Graph_worker::setY);
    connect(this, &MainWindow::setXY, worker, &Graph_worker::setXY);
    connect(this, &MainWindow::clearGraph,worker, &Graph_worker::cleargraph);
    drow_thread.start();
    ui->graph_layout->addWidget(worker);
    //создание графиков
    // ui->customplot->addGraph();
    // //формат линии
    // ui->customplot->graph(0)->setScatterStyle(QCPScatterStyle::ssCircle);//точки
    // ui->customplot->graph(0)->setLineStyle(QCPGraph::lsLine);//линии
    // //подпись осей
    // ui->customplot->xAxis->setLabel("t, мс");
    // ui->customplot->yAxis->setLabel("I, Ампер");
    // //размерность
    // ui->customplot->xAxis->setRange(0,100);
    // ui->customplot->yAxis->setRange(-2.5, 2.5);
    // //движение по оси координат только по оси X
    // ui->customplot->xAxis->axisRect()->setRangeDrag(Qt::Horizontal);
    // ui->customplot->xAxis->axisRect()->setRangeZoom(Qt::Horizontal);
    // //разрешение на движение графика
    // ui->customplot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables | QCP::iSelectItems);

    ui->chanelcomboBox->addItem("100 - 2400 mA");
    ui->chanelcomboBox->addItem("5 - 100 mA");
    ui->chanelcomboBox->addItem("0.25 - 5 mA");
    ui->chanelcomboBox->addItem("0.0 1- 0.25 mA");
    ui->chanelcomboBox->addItem("Auto");
    // this->setCentralWidget(worker);
    serialPort.setBaudRate(115200); //115200921600
    //таймер
    // QTimer *timer = new QTimer(this);
    // connect(timer, SIGNAL(timeout()), this, SLOT(draw()));
    // timer->start(1);

    ui->startButton->setEnabled(false);
    draw();
}

MainWindow::~MainWindow()
{
    drow_thread.quit();
    drow_thread.wait();
    serialPort.close();
    delete ui;
}


//Заглушка для отрисовывания графиков
void MainWindow::on_startButton_clicked()
{

    if(!checkedStart){
        t = 0;
        i = 0;
        x.clear();
        y.clear();
        emit clearGraph();
        QByteArray flag_data;
        flag_data.setNum(this->ui->chanelcomboBox->currentIndex() + 1);
        qDebug() << flag_data;
        serialPort.write(flag_data);
        // serialPort.write()
        // ui->customplot->graph(0)->data()->clear();
        // ui->customplot->update();
        ui->startButton->setText("Стоп");
        ui->connectButton->setEnabled(false);
        ui->portComboBox->setEnabled(false);
        checkedStart = !checkedStart;
    } else{
        serialPort.write("0");
        serialPort.waitForBytesWritten();
        ui->startButton->setText("Старт");
        ui->connectButton->setEnabled(true);
        ui->portComboBox->setEnabled(true);
        checkedStart = !checkedStart;
    }

    draw();
    //ui->customplot->graph(0)->data()->clear();
    //ui->customplot->replot();
    //ui->customplot->update();
    //MainWindow::draw();

}

//движение по оси координат только по оси X
void MainWindow::on_xAxisButton_clicked()
{
    emit setX();
}

//движение по оси координат только по оси Y
void MainWindow::on_yAxisButton_clicked()
{
    emit setY();
}

//движение по оси координат по XY
void MainWindow::on_xyAxisButton_clicked()
{
    emit setXY();
}

//обновление доступных портов
void MainWindow::on_refreshportButton_clicked()
{
    ui->portComboBox->clear();
    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
    {
        ui->portComboBox->addItem(serialPortInfo.portName());
    }
}


//подключение к устройству
void MainWindow::on_connectButton_clicked()
{
    data.clear();
    serialPort.setPortName(this->ui->portComboBox->currentText());
    if (!checkedPORT){
        if (!serialPort.open(QIODevice::ReadWrite)) {
            // если подключится не получится, то покажем сообщение с ошибкой
            QMessageBox::critical(this, "Ошибка", "Не удалось подключится к порту");
            serialPort.close();
            return;
        }
        //опрос устройства
        serialPort.write("s");
        serialPort.waitForBytesWritten(); // ждем пока дойдет

        data.append(serialPort.readAll());
        while (serialPort.waitForReadyRead(10)) {

            data.append(serialPort.readAll());

        }
        if (data != "s"){
            // если подключится не получится, то покажем сообщение с ошибкой
            QMessageBox::warning(this, "Ошибка", "Не удалось подключится к устройству");
            serialPort.close();
            return;
        }
        ui->connectButton->setText("Отключиться");
        ui->startButton->setEnabled(true);
        ui->refreshportButton->setEnabled(false);
        checkedPORT = !checkedPORT;
        data.clear();
    } else{
        //disconnect
        serialPort.open(QIODevice::ReadWrite);
        serialPort.write("0");
        serialPort.waitForBytesWritten();
        serialPort.close();
        ui->connectButton->setText("Подключиться");
        ui->startButton->setEnabled(false);
        ui->refreshportButton->setEnabled(true);
        checkedPORT = !checkedPORT;
    }
}

void MainWindow::state(){

    if (checkedPORT){

        serialPort.setPortName(this->ui->portComboBox->currentText());
        serialPort.open(QIODevice::ReadWrite);
        data.append(serialPort.readAll());
        // ui->portlineEdit->setText(data.data());
        t++;
        // ui->timelineEdit->setText(QString::number(t));
        // qDebug()<< QString::number(t);
    }

}
void MainWindow::draw(){
    if(checkedStart){
        uint8_t dd[10];
        //for (int j = 0; j < 10; j++) datain[j] = 0;
        serialPort.read(datain, 10);
        for (int j = 0; j < 10; j++){
            dd[j] = datain[j];
            y.push_back(dd[j]);
        }
        for(int index = 0; index < 10; index++){
            qDebug() << "dd = " << (int)dd[index];
            qDebug() << "di = " << (int)datain[index];
            qDebug() << "y = " << y[y.size() - 10 + index] << "\n";
        }

        int ch = i + 10;
        for (i; i < ch; i++){
            x.push_back(i);

        }
        if((x.size() % 100) == 0){
            qDebug() << "x" << x[x.size() - 1] << " size data" << y.size();
            emit operate(x, y);
            x.clear();
            y.clear();
        }
        /*
        QSerialPort serialPort;
        QByteArray data;
        serialPort.setBaudRate(921600);
        serialPort.setPortName(this->ui->portComboBox->currentText());
        serialPort.open(QIODevice::ReadWrite);*/

        // ui->customplot->graph(0)->setData(x,y);
        // ui->customplot->rescaleAxes();
        // ui->customplot->replot();
        // ui->customplot->update();
        state();
    }
}

void MainWindow::handleResults(const QString &)
{

}
