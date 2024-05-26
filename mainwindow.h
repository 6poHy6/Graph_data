#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QThread>
#include "graph_work.h"



QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();



private slots:
    void on_startButton_clicked();

    void on_xAxisButton_clicked();

    void on_yAxisButton_clicked();

    void on_xyAxisButton_clicked();

    void on_refreshportButton_clicked();

    void on_connectButton_clicked();

    void state();

    void draw();

private:
    Ui::MainWindow *ui;
    QVector<double> x,y;
    QThread drow_thread;
    Graph_worker *worker;
    QSerialPort serialPort;
    QByteArray data;
    char datain[10] = {0};
public slots:
    void handleResults(const QString &);
signals:
    void operate(QVector<double> &x, QVector<double> &y);
    void setX();
    void setY();
    void setXY();
    void clearGraph();

};
#endif // MAINWINDOW_H
