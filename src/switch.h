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

#ifndef SWITCH_H
#define SWITCH_H

/**
 * @file switch.h
 * @brief Material-Design Toggle-Switch-Widget für Qt Widgets.
 */

#include <QtWidgets>
#include "style.h"

/**
 * @brief Hilfsobjekt für QProperty-basierte Animationen.
 *
 * Kapselt QVariantAnimation und sendet bei jeder Wertänderung einen
 * StyleAnimationUpdate-Event an das Zielobjekt, das daraufhin sich neu zeichnet.
 * Läuft nicht mehr als nötig: stoppt automatisch wenn das Widget das Event ablehnt.
 */
class Animator final : public QVariantAnimation {
    Q_OBJECT
    Q_PROPERTY(QObject* targetObject READ targetObject WRITE setTargetObject)

public:
    Animator(QObject* target, QObject* parent = nullptr);
    ~Animator() override;

    QObject* targetObject() const;
    void setTargetObject(QObject* target);

    inline bool isRunning() const {
        return state() == Running;
    }

public slots:
    void setup(int duration, QEasingCurve easing = QEasingCurve::Linear);
    void interpolate(const QVariant& start, const QVariant& end);
    /** @brief Laufende Animation sofort auf einen festen Wert setzen (kein Tween). */
    void setCurrentValue(const QVariant&);

protected:
    /** @brief Sendet StyleAnimationUpdate an target; stoppt wenn abgelehnt. */
    void updateCurrentValue(const QVariant& value) override final;
    void updateState(QAbstractAnimation::State newState,
                     QAbstractAnimation::State oldState) override final;

private:
    QPointer<QObject> target; ///< Schwacher Zeiger auf das animierte Objekt
};

/**
 * @brief Abstrakte Basisklasse für auswählbare Steuerelemente (Toggle-Stil).
 *
 * Erweitert QAbstractButton um ein `checkState()`-Interface und ein
 * `stateChanged(int)`-Signal, das nach jedem Zustandswechsel emittiert wird.
 */
class SelectionControl : public QAbstractButton {
    Q_OBJECT

public:
    explicit SelectionControl(QWidget* parent = nullptr);
    ~SelectionControl() override;

    Qt::CheckState checkState() const;

Q_SIGNALS:
    void stateChanged(int);

protected:
    void enterEvent(QEnterEvent*) override;
    void checkStateSet() override;
    void nextCheckState() override;
    virtual void toggle(Qt::CheckState state) = 0;
};

/**
 * @brief Material-Design Toggle-Switch.
 *
 * Zeigt einen animierten Schieberegler an. Unterstützt:
 * - Optionalen Label-Text (rechts vom Schalter)
 * - Optionalen Prefix-QLabel (zeigt "Ein"/"Aus" und aktualisiert sich automatisch)
 * - Individuelle Farbe über QBrush-Konstruktor
 *
 * @par Verwendung:
 * @code
 * Switch *sw = new Switch("Benachrichtigungen", this);
 * connect(sw, &QAbstractButton::toggled, this, [](bool on) { ... });
 * @endcode
 */
class Switch final : public SelectionControl {
    Q_OBJECT

public:
    explicit Switch(QWidget* parent = nullptr, QLabel * praefix = nullptr);
    Switch(const QString& text, QWidget* parent = nullptr);
    Switch(const QString& text, const QBrush&, QWidget* parent = nullptr);
    /** @brief Konstruktor mit expliziter Höhe in Pixeln – skaliert alles proportional. */
    Switch(const QString& text, int height, QWidget* parent = nullptr);
    ~Switch() override;

    QSize sizeHint() const override final;

protected:
    void paintEvent(QPaintEvent*) override final;
    void resizeEvent(QResizeEvent*) override final;
    void toggle(Qt::CheckState) override final;

    /** @brief Berechnet den Anzeigebereich des Schiebereglers. */
    QRect indicatorRect();
    /** @brief Berechnet den Anzeigebereich des Label-Texts. */
    QRect textRect();

    // Geometry helpers – derived from style.height so scaling is always consistent
    // Original reference: height=36 → thumbRadius=14.5, cornerRadius=8, shadowElevation=2
    double thumbRadius()     const { return style.height * 0.403; }
    double cornerRadius()    const { return (style.height - style.indicatorMargin.top() - style.indicatorMargin.bottom() - 4.0) / 2.0; }
    double shadowElevation() const { return style.height * 0.056; }

    /** @brief Erstellt eine halbtransparente Farbe mit gegebener Opazität (0.0–1.0). */
    static inline QColor colorFromOpacity(const QColor& c, qreal opacity) {
        return QColor(c.red(), c.green(), c.blue(), qRound(opacity * 255.0));
    }
    static inline bool ltr(QWidget* w) {
        if (nullptr != w)
            return w->layoutDirection() == Qt::LeftToRight;

        return false;
    }

private slots:
    /** @brief Aktualisiert den Prefix-Label wenn sich der Schaltzustand ändert. */
    void toggledBtn(bool checked);

private:
    /** @brief Initialisiert Animationen und gemeinsame Konstruktor-Logik. */
    void init();
    Style::Switch style;                  ///< Visuelle Parameter (Farben, Animationsdauern)
    QPixmap       shadowPixmap;           ///< Vorgerenderte Schatten-Ellipse
    QPointer<Animator> thumbBrushAnimation; ///< Farbanimation des Thumbs
    QPointer<Animator> trackBrushAnimation; ///< Farbanimation des Tracks
    QPointer<Animator> thumbPosAnimation;   ///< Positionsanimation des Thumbs
    QLabel *praefix;                      ///< Optionaler "Ein/Aus"-Prefix-Label (kann nullptr sein)
};

#endif // SWITCH_H
