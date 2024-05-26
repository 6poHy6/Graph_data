#include "graph_work.h"

Graph_worker::Graph_worker(QCustomPlot *parent)
    : QCustomPlot{parent}
{
    this->addGraph();
    //формат линии
    this->graph(0)->setScatterStyle(QCPScatterStyle::ssCircle);//точки
    this->graph(0)->setLineStyle(QCPGraph::lsLine);//линии
    //подпись осей
    this->xAxis->setLabel("t, мс");
    this->yAxis->setLabel("I, Ампер");
    //размерность
    this->xAxis->setRange(0,100);
    this->yAxis->setRange(-2.5, 2.5);
    //движение по оси координат только по оси X
    this->xAxis->axisRect()->setRangeDrag(Qt::Horizontal);
    this->xAxis->axisRect()->setRangeZoom(Qt::Horizontal);
    //разрешение на движение графика
    this->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables | QCP::iSelectItems);
}

void Graph_worker::cleargraph()
{
    this->graph(0)->data()->clear();
    this->update();
}

void Graph_worker::setX()
{
   this->xAxis->axisRect()->setRangeDrag(Qt::Horizontal);
   this->xAxis->axisRect()->setRangeZoom(Qt::Horizontal);
   this->yAxis->axisRect()->setRangeDrag(Qt::Horizontal);
   this->yAxis->axisRect()->setRangeZoom(Qt::Horizontal);
}

void Graph_worker::setY()
{
   this->xAxis->axisRect()->setRangeDrag(Qt::Vertical);
   this->xAxis->axisRect()->setRangeZoom(Qt::Vertical);
   this->yAxis->axisRect()->setRangeDrag(Qt::Vertical);
   this->yAxis->axisRect()->setRangeZoom(Qt::Vertical);
}

void Graph_worker::setXY()
{
   this->xAxis->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
   this->xAxis->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
   this->yAxis->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
   this->yAxis->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
}
