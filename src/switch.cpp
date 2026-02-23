/*
 * This is nearly complete Material design Switch widget implementation in qtwidgets module.
 * More info: https://material.io/design/components/selection-controls.html#switches
 * Copyright (C) 2018-2020 Iman Ahmadvand
 *
 * This is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * It is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
*/

#include "switch.h"
#include <QFontMetrics>

// ============================================================
// Animator
// ============================================================

Animator::Animator(QObject* target, QObject* parent) : QVariantAnimation(parent) {
    setTargetObject(target);
}

Animator::~Animator() {
    stop();
}

QObject* Animator::targetObject() const {
    return target.data();
}

void Animator::setTargetObject(QObject* _target) {
    if (target.data() == _target)
        return;

    if (isRunning()) {
        qWarning("Animation::setTargetObject: you can't change the target of a running animation");
        return;
    }

    target = _target;
}

void Animator::updateCurrentValue(const QVariant& value) {
    Q_UNUSED(value);

    if (!target.isNull()) {
        auto update = QEvent(QEvent::StyleAnimationUpdate);
        update.setAccepted(false);
        QCoreApplication::sendEvent(target.data(), &update);
        if (!update.isAccepted())
            stop();
    }
}

void Animator::updateState(QAbstractAnimation::State newState, QAbstractAnimation::State oldState) {
    if (target.isNull() && oldState == Stopped) {
        qWarning("Animation::updateState: Changing state of an animation without target");
        return;
    }

    QVariantAnimation::updateState(newState, oldState);

    if (!endValue().isValid() && direction() == Forward) {
        qWarning("Animation::updateState (%s): starting an animation without end value", targetObject()->metaObject()->className());
    }
}

void Animator::setup(int duration, QEasingCurve easing) {
    setDuration(duration);
    setEasingCurve(easing);
}

void Animator::interpolate(const QVariant& _start, const QVariant& end) {
    setStartValue(_start);
    setEndValue(end);
    start(); // Animation starten — wird durch updateCurrentValue fortlaufend aufgerufen
}

void Animator::setCurrentValue(const QVariant& value) {
    // Start == End: kein Tween, nur aktuellen Wert sofort setzen
    setStartValue(value);
    setEndValue(value);
    updateCurrentValue(currentValue());
}


// ============================================================
// SelectionControl
// ============================================================

SelectionControl::SelectionControl(QWidget* parent) : QAbstractButton(parent) {
    setObjectName("SelectionControl");
    setCheckable(true);
}

SelectionControl::~SelectionControl() {

}

void SelectionControl::enterEvent(QEnterEvent* e) {
    setCursor(Qt::PointingHandCursor);
    QAbstractButton::enterEvent(e);
}

Qt::CheckState SelectionControl::checkState() const {
    return isChecked() ? Qt::Checked : Qt::Unchecked;
}

void SelectionControl::checkStateSet() {
    const auto state = checkState();
    emit stateChanged(state);
    toggle(state);
}

void SelectionControl::nextCheckState() {
    QAbstractButton::nextCheckState();
    SelectionControl::checkStateSet();
}

// ============================================================
// Switch
// ============================================================

void Switch::init() {
    this->praefix = nullptr;
    setFont(style.font);
    setObjectName("Switch");
    /* setup animations */
    thumbBrushAnimation = new Animator{ this, this };
    trackBrushAnimation = new Animator{ this, this };
    thumbPosAnimation = new Animator{ this, this };
    thumbPosAnimation->setup(style.thumbPosAnimation.duration, style.thumbPosAnimation.easing);
    trackBrushAnimation->setup(style.trackBrushAnimation.duration, style.trackBrushAnimation.easing);
    thumbBrushAnimation->setup(style.thumbBrushAnimation.duration, style.thumbBrushAnimation.easing);
    /* set init values */
    trackBrushAnimation->setStartValue(colorFromOpacity(style.trackOffBrush, style.trackOffOpacity));
    trackBrushAnimation->setEndValue(colorFromOpacity(style.trackOffBrush, style.trackOffOpacity));
    thumbBrushAnimation->setStartValue(colorFromOpacity(style.thumbOffBrush, style.thumbOffOpacity));
    thumbBrushAnimation->setEndValue(colorFromOpacity(style.thumbOffBrush, style.thumbOffOpacity));
    /* do NOT override ButtonText — inherit from application/window palette so
       dark- and light-mode foreground colours are handled automatically. */
    setSizePolicy(QSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Fixed));
}

QRect Switch::indicatorRect() {
    const auto w = style.indicatorMargin.left() + style.height + style.indicatorMargin.right();
    return ltr(this) ? QRect(0, 0, w, style.height) : QRect(width() - w, 0, w, style.height);
}

QRect Switch::textRect() {
    const auto w = style.indicatorMargin.left() + style.height + style.indicatorMargin.right();
    return ltr(this) ? rect().marginsRemoved(QMargins(w, 0, 0, 0)) : rect().marginsRemoved(QMargins(0, 0, w, 0));
}

void Switch::toggledBtn(bool checked)
{
    if(this->praefix)
        this->praefix->setText(checked ? "Ein " : "Aus ");
}

Switch::Switch(QWidget* parent, QLabel *praefix) : SelectionControl(parent) {
    init();
    if(praefix) {
        this->praefix = praefix;
        toggledBtn(false);
        connect(this, &Switch::toggled, this, &Switch::toggledBtn);
    }
}

Switch::Switch(const QString& text, int height, QWidget* parent) : SelectionControl(parent) {
    // Scale all geometry proportionally to the requested height
    style.height = height;
    style.indicatorMargin = QMargins(height / 5, height / 5, height / 5, height / 5);
    style.font = QFont("Roboto medium", qMax(6, qRound(height * 0.36)));
    init();
    setText(text);
}

Switch::Switch(const QString& text, const QBrush& brush, QWidget* parent) : Switch(text, 30, parent) {
    // Akzentfarbe anpassen: Thumb und Track im eingeschalteten Zustand einfärben
    style.thumbOnBrush = brush.color();
    style.trackOnBrush = brush.color();
}

Switch::~Switch() {
    // Animationen werden automatisch durch QObject-Parent-Kind-Mechanismus gelöscht
}

// QSize Switch::sizeHint() const {
//     auto h = style.height;
//     auto w = style.indicatorMargin.left() + style.height + style.indicatorMargin.right() + fontMetrics().width(text());
//     return QSize(w, h);
// }

QSize Switch::sizeHint() const {
    auto h = style.height;
    auto w = style.indicatorMargin.left() + style.height + style.indicatorMargin.right() + QFontMetrics(font()).horizontalAdvance(text());
    return QSize(w, h);
}

void Switch::paintEvent(QPaintEvent*) {
    /* Radial-Reaktion (Ripple-Effekt) wird für Desktop-Nutzung nicht benötigt */
    QPainter p(this);

    const auto _indicatorRect = indicatorRect();
    const auto _textRect = textRect();
    // Track ist etwas schmaler als der Indikator-Bereich
    auto trackMargin = style.indicatorMargin;
    trackMargin.setTop(trackMargin.top() + 2);
    trackMargin.setBottom(trackMargin.bottom() + 2);
    QRectF trackRect = _indicatorRect.marginsRemoved(trackMargin);

    if (isEnabled()) {
        p.setOpacity(1.0);
        p.setPen(Qt::NoPen);
        /* draw track */
        p.setBrush(trackBrushAnimation->currentValue().value<QColor>());
        p.setRenderHint(QPainter::Antialiasing, true);
        p.drawRoundedRect(trackRect, cornerRadius(), cornerRadius());
        p.setRenderHint(QPainter::Antialiasing, false);
        /* draw thumb */
        trackRect.setX(trackRect.x() - trackMargin.left() - trackMargin.right() - 2 + thumbPosAnimation->currentValue().toInt());
        auto thumbRect = trackRect;

        if (!shadowPixmap.isNull())
            p.drawPixmap(thumbRect.center() - QPointF(thumbRadius(), thumbRadius() - 1.0), shadowPixmap);

        p.setBrush(thumbBrushAnimation->currentValue().value<QColor>());
        p.setRenderHint(QPainter::Antialiasing, true);
        //        qDebug() << thumbRect << thumbPosAnimation->currentValue();
        p.drawEllipse(thumbRect.center(), thumbRadius() - shadowElevation() - 1.0, thumbRadius() - shadowElevation() - 1.0);
        p.setRenderHint(QPainter::Antialiasing, false);

        /* draw text */
        if (text().isEmpty())
            return;

        p.setOpacity(1.0);
        p.setPen(palette().color(QPalette::Active, QPalette::ButtonText));
        p.setFont(font());
        p.drawText(_textRect, Qt::AlignLeft | Qt::AlignVCenter, text());
    } else {
        p.setOpacity(style.trackDisabledOpacity);
        p.setPen(Qt::NoPen);
        // draw track
        p.setBrush(style.trackDisabled);
        p.setRenderHint(QPainter::Antialiasing, true);
        p.drawRoundedRect(trackRect, cornerRadius(), cornerRadius());
        p.setRenderHint(QPainter::Antialiasing, false);
        // draw thumb
        p.setOpacity(1.0);
        if (!isChecked())
            trackRect.setX(trackRect.x() - trackMargin.left() - trackMargin.right() - 2);
        else
            trackRect.setX(trackRect.x() + trackMargin.left() + trackMargin.right() + 2);
        auto thumbRect = trackRect;

        if (!shadowPixmap.isNull())
            p.drawPixmap(thumbRect.center() - QPointF(thumbRadius(), thumbRadius() - 1.0), shadowPixmap);

        p.setOpacity(1.0);
        p.setBrush(style.thumbDisabled);
        p.setRenderHint(QPainter::Antialiasing, true);
        p.drawEllipse(thumbRect.center(), thumbRadius() - shadowElevation() - 1.0, thumbRadius() - shadowElevation() - 1.0);

        /* draw text */
        if (text().isEmpty())
            return;

        p.setOpacity(style.disabledTextOpacity);
        p.setPen(palette().color(QPalette::Disabled, QPalette::ButtonText));
        p.setFont(font());
        p.drawText(_textRect, Qt::AlignLeft | Qt::AlignVCenter, text());
    }
}

void Switch::resizeEvent(QResizeEvent* e) {
    shadowPixmap = Style::drawShadowEllipse(thumbRadius(), shadowElevation(), QColor(0, 0, 0, 70));
    SelectionControl::resizeEvent(e);
}

void Switch::toggle(Qt::CheckState state) {
    // Animationszielwerte je nach Zustand (Checked/Unchecked) setzen
    // Falls Widget nicht sichtbar ist: direkt auf Endwert springen (kein Tween)
    if (state == Qt::Checked) {
        const QVariant posEnd = (style.indicatorMargin.left() + style.indicatorMargin.right() + 2) * 2;
        const QVariant thumbEnd = colorFromOpacity(style.thumbOnBrush, style.thumbOnOpacity);
        const QVariant trackEnd = colorFromOpacity(style.trackOnBrush, style.trackOnOpacity);

        if (!isVisible()) {
            thumbPosAnimation->setCurrentValue(posEnd);
            thumbBrushAnimation->setCurrentValue(thumbEnd);
            trackBrushAnimation->setCurrentValue(trackEnd);
        } else {
            thumbPosAnimation->interpolate(0, posEnd);
            thumbBrushAnimation->interpolate(colorFromOpacity(style.thumbOffBrush, style.thumbOffOpacity), thumbEnd);
            trackBrushAnimation->interpolate(colorFromOpacity(style.trackOffBrush, style.trackOffOpacity), trackEnd);
        }
    } else { // Qt::Unchecked
        const QVariant posEnd = 0;
        const QVariant thumbEnd = colorFromOpacity(style.thumbOffBrush, style.thumbOffOpacity);
        const QVariant trackEnd = colorFromOpacity(style.trackOffBrush, style.trackOffOpacity);

        if (!isVisible()) {
            thumbPosAnimation->setCurrentValue(posEnd);
            thumbBrushAnimation->setCurrentValue(thumbEnd);
            trackBrushAnimation->setCurrentValue(trackEnd);
        } else {
            thumbPosAnimation->interpolate(thumbPosAnimation->currentValue().toInt(), posEnd);
            thumbBrushAnimation->interpolate(colorFromOpacity(style.thumbOnBrush, style.thumbOnOpacity), thumbEnd);
            trackBrushAnimation->interpolate(colorFromOpacity(style.trackOnBrush, style.trackOnOpacity), trackEnd);
        }
    }
}
