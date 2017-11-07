#ifndef DROPPABLEQLISTWIDGET_H
#define DROPPABLEQLISTWIDGET_H

#include <QListWidget>
#include<QDragEnterEvent>

class DroppableQListWidget : public QListWidget
{
public:
    DroppableQListWidget(QWidget *parent = Q_NULLPTR);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void keyPressEvent(QKeyEvent *event);
    bool accepts(QString path);
};

#endif // DROPPABLEQLISTWIDGET_H
