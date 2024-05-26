#ifndef GRAPH_WORK_H
#define GRAPH_WORK_H

#include <QObject>
#include <QWidget>
#include <qcustomplot.h>

class Graph_worker : public QCustomPlot
{
    Q_OBJECT
public:
    explicit Graph_worker(QCustomPlot *parent = nullptr);
public slots:
    void doWork(QVector<double> &x, QVector<double> &y) {
        QString result;
        // this->graph(0)->setData(x,y);
        this->graph(0)->addData(x,y);

        this->rescaleAxes();
        this->replot();
        this->update();
        emit resultReady(result);
    }
    void cleargraph();
    void setX();
    void setY();
    void setXY();

signals:
    void resultReady(const QString &result);
};

#endif // GRAPH_WORK_H
