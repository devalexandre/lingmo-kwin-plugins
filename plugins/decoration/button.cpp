/*
 * Copyright (C) 2020 PandaOS Team.
 *
 * Author:     rekols <rekols@foxmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "button.h"
#include "decoration.h"
#include "helper.h"

#include <KDecoration3/DecoratedWindow>
#include <KDecoration3/Decoration>

#include <QPainter>
#include <QPainterPath>

Button::Button(KDecoration3::DecorationButtonType type, const QPointer<KDecoration3::Decoration> &decoration, QObject *parent)
    : KDecoration3::DecorationButton(type, decoration, parent)
{
#if KDECORATION_VERSION <= QT_VERSION_CHECK(5, 27, 12)
    auto c = decoration->window().toStrongRef().data();
#else
    auto c = decoration->window();
#endif 

    switch (type) {
    case KDecoration3::DecorationButtonType::Menu:
        break;
    case KDecoration3::DecorationButtonType::Minimize:
        setVisible(c->isMinimizeable());
        connect(c, &KDecoration3::DecoratedWindow::minimizeableChanged, this, &Button::setVisible);
        break;
    case KDecoration3::DecorationButtonType::Maximize:
        setVisible(c->isMaximizeable());
        connect(c, &KDecoration3::DecoratedWindow::maximizeableChanged, this, &Button::setVisible);
        break;
    case KDecoration3::DecorationButtonType::Close:
        setVisible(c->isCloseable());
        connect(c, &KDecoration3::DecoratedWindow::closeableChanged, this, &Button::setVisible);
        break;
    default:
        setVisible(false);
        break;
    }
}

KDecoration3::DecorationButton *Button::create(KDecoration3::DecorationButtonType type, KDecoration3::Decoration *decoration, QObject *parent)
{
    return new Button(type, decoration, parent);
}

void Button::paint(QPainter *painter, const QRectF &repaintArea)
{
    Q_UNUSED(repaintArea)

    Lingmo::Decoration *decoration = qobject_cast<Lingmo::Decoration *>(this->decoration());

    if (!decoration)
        return;

#if KDECORATION_VERSION <= QT_VERSION_CHECK(5, 27, 12)
    auto c = decoration->window().toStrongRef().data();
#else
    auto c = decoration->window();
#endif
    const QRect &rect = geometry().toRect();

    painter->save();
    painter->setRenderHints(QPainter::Antialiasing, true);

    // Lingmo Beautiful Buttons - colored circular buttons
    const int circleSize = 12 * decoration->devicePixelRatio();
    QRect circleRect(0, 0, circleSize, circleSize);
    circleRect.moveCenter(rect.center());

    painter->setPen(Qt::NoPen);

    QColor buttonColor;
    QColor borderColor;
    QColor symbolColor;
    bool drawSymbol = isHovered() || isPressed();

    switch (type()) {
    case KDecoration3::DecorationButtonType::Menu: {
        c->icon().paint(painter, rect);
        break;
    }
    case KDecoration3::DecorationButtonType::ApplicationMenu: {
        break;
    }
    case KDecoration3::DecorationButtonType::Minimize: {
        // Yellow button (Minimize) - #FDBE41
        buttonColor = QColor(253, 190, 65);
        borderColor = QColor(227, 164, 41);
        symbolColor = QColor(149, 95, 19);

        painter->setBrush(buttonColor);
        painter->setPen(QPen(borderColor, 0.5));
        painter->drawEllipse(circleRect);

        if (drawSymbol) {
            painter->setPen(QPen(symbolColor, 1.5 * decoration->devicePixelRatio()));
            int offset = circleSize / 4;
            QPoint center = circleRect.center();
            // Draw horizontal line
            painter->drawLine(center.x() - offset, center.y(),
                            center.x() + offset, center.y());
        }
        break;
    }
    case KDecoration3::DecorationButtonType::Maximize: {
        // Green button (Maximize/Restore) - #34C759
        buttonColor = QColor(52, 199, 89);
        borderColor = QColor(38, 175, 67);
        symbolColor = QColor(18, 86, 35);

        painter->setBrush(buttonColor);
        painter->setPen(QPen(borderColor, 0.5));
        painter->drawEllipse(circleRect);

        if (drawSymbol) {
            painter->setPen(QPen(symbolColor, 1.5 * decoration->devicePixelRatio()));
            int offset = circleSize / 4;
            QPoint center = circleRect.center();

            if (isChecked()) {
                // Restore: two overlapping squares (small icon)
                QRect smallRect(center.x() - offset/2, center.y() - offset/2, offset, offset);
                painter->drawRect(smallRect);
            } else {
                // Maximize: diagonal arrows pointing outward
                painter->drawLine(center.x() - offset, center.y() - offset,
                                center.x() + offset, center.y() + offset);
                painter->drawLine(center.x() + offset, center.y() - offset,
                                center.x() - offset, center.y() + offset);
            }
        }
        break;
    }
    case KDecoration3::DecorationButtonType::Close: {
        // Red button (Close) - #FC5F54
        buttonColor = QColor(252, 95, 84);
        borderColor = QColor(226, 71, 61);
        symbolColor = QColor(120, 24, 21);

        painter->setBrush(buttonColor);
        painter->setPen(QPen(borderColor, 0.5));
        painter->drawEllipse(circleRect);

        if (drawSymbol) {
            painter->setPen(QPen(symbolColor, 1.5 * decoration->devicePixelRatio()));
            int offset = circleSize / 4;
            QPoint center = circleRect.center();
            // Draw X
            painter->drawLine(center.x() - offset, center.y() - offset,
                            center.x() + offset, center.y() + offset);
            painter->drawLine(center.x() + offset, center.y() - offset,
                            center.x() - offset, center.y() + offset);
        }
        break;
    }
    default:
        break;
    }

    painter->restore();
}
