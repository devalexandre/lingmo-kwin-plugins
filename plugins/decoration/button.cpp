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

    // Lingmo window controls (same design as LingmoUI's WindowControls): coloured
    // buttons with white glyphs inside the capsule painted by Decoration::paint
    const qreal dpr = decoration->devicePixelRatio();
    const qreal dotSize = 16 * dpr;
    QRectF dot(0, 0, dotSize, dotSize);
    dot.moveCenter(QRectF(rect).center());

    const bool lit = isHovered() || isPressed();
    const bool dark = decoration->darkMode();
    const bool active = c->isActive();

    QColor accent;
    switch (type()) {
    case KDecoration3::DecorationButtonType::Close:
        accent = QColor(0xF2, 0x55, 0x5A);   // coral
        break;
    case KDecoration3::DecorationButtonType::Minimize:
        accent = QColor(0xF5, 0xA5, 0x24);   // amber
        break;
    case KDecoration3::DecorationButtonType::Maximize:
        accent = QColor(0x2F, 0x7C, 0xF6);   // Lingmo blue
        break;
    case KDecoration3::DecorationButtonType::Menu:
        c->icon().paint(painter, rect);
        painter->restore();
        return;
    default:
        painter->restore();
        return;
    }

    // Always coloured with a white glyph (grey when the window is inactive)
    QColor fill = !active && !lit ? (dark ? QColor(0x4A, 0x4B, 0x57) : QColor(0xCF, 0xD0, 0xD6))
                : isPressed() ? accent.darker(120)
                : lit ? accent.lighter(110) : accent;
    painter->setPen(Qt::NoPen);
    painter->setBrush(fill);
    painter->drawEllipse(lit ? dot.adjusted(-0.6 * dpr, -0.6 * dpr, 0.6 * dpr, 0.6 * dpr) : dot);

    QColor ink = !active && !lit ? (dark ? QColor(255, 255, 255, 115) : QColor(255, 255, 255, 230))
                                 : QColor(Qt::white);
    QPen pen(ink, 1.5 * dpr, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    const QPointF ctr = dot.center();
    const qreal h = 3.8 * dpr;   // half glyph size

    switch (type()) {
    case KDecoration3::DecorationButtonType::Close:
        painter->drawLine(QPointF(ctr.x() - h, ctr.y() - h), QPointF(ctr.x() + h, ctr.y() + h));
        painter->drawLine(QPointF(ctr.x() + h, ctr.y() - h), QPointF(ctr.x() - h, ctr.y() + h));
        break;
    case KDecoration3::DecorationButtonType::Minimize:
        painter->drawLine(QPointF(ctr.x() - h, ctr.y()), QPointF(ctr.x() + h, ctr.y()));
        break;
    case KDecoration3::DecorationButtonType::Maximize:
        if (isChecked()) {
            // restore: two overlapping rounded squares
            const qreal q = 3 * dpr;
            painter->drawRoundedRect(QRectF(ctr.x() - q + 2 * dpr, ctr.y() - q - 2 * dpr + 1 * dpr, 2 * q, 2 * q), 1.5 * dpr, 1.5 * dpr);
            painter->setBrush(fill);
            painter->drawRoundedRect(QRectF(ctr.x() - q - 1 * dpr, ctr.y() - q + 1 * dpr, 2 * q, 2 * q), 1.5 * dpr, 1.5 * dpr);
        } else {
            // zoom: rounded square outline
            const qreal q = 3.6 * dpr;
            painter->drawRoundedRect(QRectF(ctr.x() - q, ctr.y() - q, 2 * q, 2 * q), 1.3 * dpr, 1.3 * dpr);
        }
        break;
    default:
        break;
    }

    painter->restore();
}
