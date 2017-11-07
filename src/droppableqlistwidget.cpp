#include "droppableqlistwidget.h"
#include <QMimeData>
#include <QList>
#include <QDir>
#include <QtDebug>


DroppableQListWidget::DroppableQListWidget(QWidget *parent) : QListWidget(parent)
{
    this->setAcceptDrops(true);
    setDropIndicatorShown(true);
}

void DroppableQListWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void DroppableQListWidget::dropEvent(QDropEvent *event)
{
    const QList<QUrl> urls = event->mimeData()->urls();
    for (QUrl url : urls) {
        bool found = false;
        for (int i = 0; i < this->count(); ++i) {
            if (this->item(i)->data(Qt::DisplayRole).toUrl() == url) {
                found = true;
                break;
            }
        }
        QString urlS = url.toString(QUrl::RemoveScheme);
        if (!found && url.isLocalFile() && accepts(urlS)) addItem(urlS.right(urlS.size() - 2));
    }
    this->update();
}

bool DroppableQListWidget::accepts(QString path) {
    bool found = false;
    for (int i = 0; i < this->count(); ++i) {
        if (this->item(i)->data(Qt::DisplayRole).toString() == path) {
            found = true;
            break;
        }
    }
    return !found && !QDir(path).exists() && (path.right(4) == ".png" || path.right(4) == ".jpg" || path.right(4) == ".tif");
}

void DroppableQListWidget::dragMoveEvent(QDragMoveEvent *event)
{
    event->accept();
}

void DroppableQListWidget::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Backspace || event->key() == Qt::Key_Delete) {
        for (QListWidgetItem* i : this->selectedItems()) {
            delete i;
        }
    }
}
