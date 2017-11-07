#include "slabel.h"
#include <QMouseEvent>
#include <QFrame>
#include <math.h>       /* pow */

SLabel::SLabel(QWidget *parent) : QLabel(parent)
{
    points = std::vector<status>(maxwidth*maxheight, NONE);

    totals = std::vector<int>({0,0,0});
    counts = std::vector<int>({0,0,0});
    distances = std::vector<int>({0,0,0});
}


void SLabel::mousePressEvent(QMouseEvent *ev) {
    if(ev->button() == Qt::LeftButton) {
        currValue = qRed(orimg.pixel(QPoint(ev->localPos().toPoint().x(), ev->localPos().toPoint().y())));
        pressed = true;
    }
    if(ev->button() == Qt::RightButton) {
        mode = status((mode + 1) % 3); //rotate through modes

        switch(mode) {
        case SIGNA:
            signalLabel->setText("[Green (Signal)[Average]:]");
            noiseLabel->setText("Red (Noise)[Sigma]:");
            break;
        case NOISE:
            signalLabel->setText("Green (Signal)[Average]:");
            noiseLabel->setText("[Red (Noise)[Sigma]:]");
            break;
        case NONE:
            signalLabel->setText("Green (Signal)[Average]:");
            noiseLabel->setText("Red (Noise)[Sigma]:");
            break;
        }
    }
}

void SLabel::mouseReleaseEvent(QMouseEvent*) {
    recalculate();
    pressed = false;
}

void SLabel::mouseMoveEvent(QMouseEvent *ev) {
    std::vector<double> wallsx = std::vector<double>();
    std::vector<double> wallsy = std::vector<double>();

    if (pressed)
    {
        QPoint pressed = ev->localPos().toPoint();

        for(int i = 0 - brushradius/2; i < (brushradius+1)/2; i++) {
            for(int j = 0 - brushradius/2; j < (brushradius+1)/2; j++) {
                int newx = pressed.x() + i;
                int newy = pressed.y() + j;

                bool isWithinCanvas = newx >= 0 && newx < maxwidth && newy >= 0 && newy < maxheight;

                if(!isWithinCanvas) {
                    return;
                }

                int index = newx * maxwidth + newy;
                QPoint p = QPoint(newx, newy);
                QRgb rgb = orimg.pixel(p);

                bool isWithinThreshold = (threshold == 0) || (qRed(rgb) <= currValue + threshold && qRed(rgb) >= currValue - threshold);

                if(isWithinCanvas && isWithinThreshold) {
                    if (points[index] != mode) {
                        totals[mode] += qRed(rgb);
                        counts[mode]++;
                        distances[mode] += pow((qRed(rgb) - globalAverage),2);

                        totals[points[index]] -= qRed(rgb);
                        counts[points[index]]--;
                        distances[points[index]] -= pow((qRed(rgb) - globalAverage),2);

                        points[index] = mode;
                    }

                    switch (mode) {
                    case NONE:
                        img.setPixel(p, orimgC.pixel(p));
                        break;
                    case SIGNA:
                        if(qRed(rgb) > maxsignal) maxsignal = qRed(rgb);
                        img.setPixel(p, qRgb(0,255,0));
                        break;
                    case NOISE:
                        img.setPixel(p, qRgb(255,0,0));
                        break;
                    }
                }
            }
        }
        displayImage();
    }
}

// Calculate mean noise
// Subtract mean noise from all Values
// Calculate standard deviation of noise
// Divide new mean Signal by standard deviation of noise = snr

void SLabel::recalculate() {



    double signalAvg;
    double noiseAvg;
    double noisestd;

    if (counts[SIGNA] > 0) {
        signalAvg = (double)totals[SIGNA]/counts[SIGNA];
        signal->setText(QString::number(signalAvg));
    } else {
        signal->setText("");
    }

    if (counts[NOISE] > 0) {
        noiseAvg = (double)totals[NOISE]/counts[NOISE];
//        noise->setText(QString::number(noiseAvg));

        double stddevNoise = 0;

        for(int index = 0; index < maxwidth*maxheight; index++) {
            status s = points[index];
            if(s == NOISE) {
                int y = index % maxwidth;
                int x = (index - y)/maxwidth;
                QPoint p = QPoint(x,y);
                QRgb rgb = orimg.pixel(p);

                stddevNoise += pow((qRed(rgb) - noiseAvg),2);
            }
        }

        noisestd = sqrt((double)stddevNoise/counts[NOISE]);
        noise->setText(QString::number(noisestd));

    } else {
        noise->setText("");
    }


    if (counts[NOISE] > 0 && counts[SIGNA] > 0) {
        double movedDownSignalAvg = signalAvg - noiseAvg;

//        snr->setText(QString::number((double)signalAvg/noiseAvg));
//        psnr->setText(QString::number((double)maxsignal/noiseAvg));
        snr->setText(QString::number((double)movedDownSignalAvg/noisestd));
        psnr->setText(QString::number((double)(maxsignal-noiseAvg)/noisestd));
    } else {
        snr->setText("");
        psnr->setText("");
    }
}


void SLabel::displayImage() {
    QPixmap pix = QPixmap::fromImage(img);
    this->setPixmap(pix);
}

void SLabel::setImage(QImage image) {
    img = image.scaled(maxwidth,maxheight);
    orimgC = image.scaled(maxwidth,maxheight);
    orimg = image.scaled(maxwidth,maxheight).convertToFormat(QImage::Format_Grayscale8);

    totals = std::vector<int>({0,0,0});
    counts = std::vector<int>({0,0,0});
    distances = std::vector<int>({0,0,0});

    double total = 0;
    for(int index = 0; index < maxwidth*maxheight; index++) {
        int y = index % maxwidth;
        int x = (index - y)/maxwidth;
        QPoint p = QPoint(x,y);


        total += qRed(orimg.pixel(p));

        status s = points[index];
        if(s != NONE) {
            QRgb rgb = orimg.pixel(p);
            totals[s] += qRed(rgb);
            counts[s]++;
            switch (s) {
            case NONE:
                img.setPixel(p, orimgC.pixel(p));
                break;
            case SIGNA:
                img.setPixel(p, qRgb(0,255,0));
                break;
            case NOISE:
                img.setPixel(p, qRgb(255,0,0));
                break;
            }
        }
    }
    globalAverage = total / (maxwidth*maxheight);

    for(int index = 0; index < maxwidth*maxheight; index++) {
        status s = points[index];
        if(s != NONE) {
            int y = index % maxwidth;
            int x = (index - y)/maxwidth;
            QPoint p = QPoint(x,y);
            QRgb rgb = orimg.pixel(p);

            distances[s] += pow((qRed(rgb) - globalAverage),2);
        }
    }


    displayImage();
    recalculate();
}

void SLabel::setBrushRadius(int i) {
    brushradius = i;
}

int SLabel::getBrushRadius() {
    return brushradius;
}

void SLabel::erase() {
    points = std::vector<status>(maxwidth*maxheight, NONE);

    totals = std::vector<int>({0,0,0});
    counts = std::vector<int>({0,0,0});
    distances = std::vector<int>({0,0,0});

    noise->setText("");
    signal->setText("");
    snr->setText("");
    psnr->setText("");

    setImage(orimgC);
}

void SLabel::setThreshold(int i) {
    threshold = i;
}
