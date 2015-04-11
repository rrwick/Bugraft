//Copyright 2014 Ryan Wick

//This file is part of Bugraft.

//Bugraft is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//Bugraft is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.

//You should have received a copy of the GNU General Public License
//along with Bugraft.  If not, see <http://www.gnu.org/licenses/>.




#include "infotextwidget.h"

#include <QPainter>
#include <QToolTip>
#include <QMouseEvent>


InfoTextWidget::InfoTextWidget(QWidget * parent) :
    QWidget(parent)
{
    setFixedSize(16, 16);
    setMouseTracking(true);
}

InfoTextWidget::InfoTextWidget(QWidget * parent, QString infoTextP) :
    QWidget(parent)
{
    setFixedSize(16, 16);
    setMouseTracking(true);
    setInfoText(infoTextP);
}

void InfoTextWidget::setInfoText(QString infoTextP)
{
    //Convert text to a rich text format, which will let QToolTip wrap the text.
    infoText = "<html>" + infoTextP + "</html>";
}


void InfoTextWidget::paintEvent(QPaintEvent * /*event*/)
{
    QPainter painter(this);

    QPixmap infoIcon;
    if (isEnabled())
        infoIcon = QPixmap(":/icons/information-256.png");
    else
        infoIcon = QPixmap(":/icons/information-256-inactive.png");

    int scaledSize = std::min(width(), height());

    QPixmap scaledInfoIcon = infoIcon.scaled(scaledSize, scaledSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    double topLeftX = width()/2.0 - scaledSize/2.0;
    double topLeftY = height()/2.0 - scaledSize/2.0;

    painter.drawPixmap(topLeftX, topLeftY, scaledInfoIcon);
}


void InfoTextWidget::mousePressEvent(QMouseEvent * event)
{
    QToolTip::showText(event->globalPos(), infoText);
}
