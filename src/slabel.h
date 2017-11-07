#ifndef SLABEL_H
#define SLABEL_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPixmap>

enum status {
    NONE=0,NOISE=1,SIGNA=2
};

struct Point {
    int x;
    int y;
};


class SLabel : public QLabel
{
    Q_OBJECT
public:
    explicit SLabel(QWidget *parent = 0);
    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent *ev);

    void setImage(QImage img);
    void displayImage();
    void recalculate();
    void setThreshold(int i);

    int removePoint(QPoint p, std::vector<QPoint> vec);

    QLineEdit* snr;
    QLineEdit* noise;
    QLineEdit* signal;
    QLineEdit* psnr;

    QLabel* noiseLabel;
    QLabel* signalLabel;

    void setBrushRadius(int i);
    int getBrushRadius();

    void erase();
signals:

public slots:

private:
    bool pressed = false;
    QImage img;
    QImage orimg;
    QImage orimgC;

    std::vector<status> points;
    int maxsignal = 0;

    status mode = NOISE;

    std::vector<int> totals;
    std::vector<int> distances;
    std::vector<int> counts;

    double globalAverage = 0;
    int rauschabstand = 0;

    int brushradius = 5;
    int maxheight = 750;
    int maxwidth = 750;
    int threshold = 0;
    int connectionDistance = 10;
    int currValue = 0;
};

#endif // SLABEL_H
