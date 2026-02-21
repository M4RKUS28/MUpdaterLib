/*
 * This is nearly complete Material design Switch widget implementation in qtwidgets module.
 * More info: https://material.io/design/components/selection-controls.html#switches
 * Copyright (C) 2018 Iman Ahmadvand
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

#ifndef STYLE_H
#define STYLE_H

#include <QtCore/qeasingcurve.h>

#include <QPainter>
#include <QFont>
#include <QColor>

/**
 * @warning qt_blurImage ist eine private, undokumentierte Qt-interne API.
 *          Die Signatur kann sich bei Qt-Updates ändern. Falls Kompilierungsfehler
 *          auftreten, als Workaround QGraphicsBlurEffect verwenden.
 */
Q_DECL_IMPORT void qt_blurImage(QPainter *p, QImage &blurImage, qreal radius,
                                bool quality, bool alphaOnly, int transposed = 0);

/** @brief Style-Definitionen für den Material-Design Switch-Widget. */
namespace Style {

// Material Design Farb-Konstanten (als Namespace-Variablen statt globaler #defines
// um Namespace-Kollisionen zu vermeiden)
inline const QColor cyan500("#00bcd4"); ///< Material Cyan 500
inline const QColor gray50("#fafafa");  ///< Material Gray 50 (fast Weiß)
inline const QColor Black("#000000");   ///< Schwarz (Großschreibung vermeidet Konflikt mit Qt-Makros)
inline const QColor gray400("#bdbdbd"); ///< Material Gray 400 (deaktivierter Zustand)

using Type = QEasingCurve::Type;

/** @brief Easing-Typ und Dauer einer einzelnen Animation. */
struct Animation {
    Animation() = default;
    Animation(Type _easing, int _duration) : easing{_easing}, duration{_duration} {}

    Type easing;   ///< QEasingCurve-Typ (z.B. Linear, InOutQuad)
    int  duration; ///< Dauer in Millisekunden
};

/**
 * @brief Visuelle Parameter des Switch-Widgets.
 *
 * Alle Farben, Opazitäten und Animationseinstellungen sind hier
 * zentralisiert und können nach der Konstruktion geändert werden.
 */
struct Switch {
    Switch() :
        height{ 36 },
        font{ QFont("Roboto medium", 13) },
        indicatorMargin{ QMargins(8, 8, 8, 8) },
        thumbOnBrush{ cyan500 },
        thumbOnOpacity{ 1 },
        trackOnBrush{ cyan500 },
        trackOnOpacity{ 0.5 },
        thumbOffBrush{ gray50 },
        thumbOffOpacity{ 1 },
        trackOffBrush{ Black },
        trackOffOpacity{ 0.38 },
        thumbDisabled{ gray400 },
        thumbDisabledOpacity{ 1 },
        trackDisabled{ Black },
        trackDisabledOpacity{ 0.12 },
        textColor{ Black },
        disabledTextOpacity{ 0.26 },
        thumbBrushAnimation{ Animation(Type::Linear, 150) },
        trackBrushAnimation{ Animation(Type::Linear, 150) },
        thumbPosAnimation{ Animation(Type::InOutQuad, 150) } {

    }

    int     height;           ///< Gesamthöhe des Widgets in Pixeln
    QFont   font;             ///< Schriftart für optionalen Label-Text
    QMargins indicatorMargin; ///< Abstand zwischen Thumb und Widget-Kante

    QColor thumbOnBrush;      ///< Thumb-Farbe im eingeschalteten Zustand
    double thumbOnOpacity;    ///< Thumb-Opazität (0.0–1.0) eingeschaltet
    QColor trackOnBrush;      ///< Track-Farbe eingeschaltet
    double trackOnOpacity;    ///< Track-Opazität eingeschaltet

    QColor thumbOffBrush;     ///< Thumb-Farbe ausgeschaltet
    double thumbOffOpacity;   ///< Thumb-Opazität ausgeschaltet
    QColor trackOffBrush;     ///< Track-Farbe ausgeschaltet
    double trackOffOpacity;   ///< Track-Opazität ausgeschaltet

    QColor thumbDisabled;      ///< Thumb-Farbe deaktiviert
    double thumbDisabledOpacity; ///< Thumb-Opazität deaktiviert
    QColor trackDisabled;      ///< Track-Farbe deaktiviert
    double trackDisabledOpacity; ///< Track-Opazität deaktiviert

    QColor textColor;          ///< Schriftfarbe
    double disabledTextOpacity;///< Schrift-Opazität wenn deaktiviert

    Animation thumbBrushAnimation; ///< Farbanimation des Thumbs
    Animation trackBrushAnimation; ///< Farbanimation des Tracks
    Animation thumbPosAnimation;   ///< Positionsanimation des Thumbs
};

/**
 * @brief Zeichnet eine Ellipse mit weichem Schatten in eine Pixmap.
 * @param radius     Radius der Ellipse.
 * @param elevation  Blur-Radius für den Schatten (größer = weicher).
 * @param color      Farbe der Ellipse.
 * @return Fertige Pixmap (Größe: radius*2 x radius*2).
 *
 * @note Verwendet qt_blurImage (private Qt-API). Auf Plattformen ohne
 *       qtwidgets-Modul nicht verfügbar.
 */
inline QPixmap drawShadowEllipse(qreal radius, qreal elevation, const QColor& color) {
    auto px = QPixmap(radius * 2, radius * 2);
    px.fill(Qt::transparent);

    { // Ellipse zeichnen
        QPainter p(&px);
        p.setBrush(color);
        p.setPen(Qt::NoPen);
        p.setRenderHint(QPainter::Antialiasing, true);
        p.drawEllipse(QRectF(0, 0, px.size().width(), px.size().height()).center(),
                      radius - elevation, radius - elevation);
    }

    QImage tmp(px.size(), QImage::Format_ARGB32_Premultiplied);
    tmp.setDevicePixelRatio(px.devicePixelRatioF());
    tmp.fill(0);
    QPainter tmpPainter(&tmp);
    tmpPainter.setCompositionMode(QPainter::CompositionMode_Source);
    tmpPainter.drawPixmap(QPointF(), px);
    tmpPainter.end();

    // blur the alpha channel
    QImage blurred(tmp.size(), QImage::Format_ARGB32_Premultiplied);
    blurred.setDevicePixelRatio(px.devicePixelRatioF());
    blurred.fill(0);
    {
        QPainter blurPainter(&blurred);
        qt_blurImage(&blurPainter, tmp, elevation * 4., true, false);
    }

    tmp = blurred;

    return QPixmap::fromImage(tmp);
}

} // namespace Style

#endif // STYLE_H
