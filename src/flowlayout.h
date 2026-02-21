#ifndef FLOWLAYOUT_H
#define FLOWLAYOUT_H

/**
 * @file flowlayout.h
 * @brief Zeilenumbruch-Layout ähnlich dem Textfluss (Flow Layout).
 */

#include <QStyle>
#include <QLayoutItem>
#include <QLayout>
#include <QWidget>
#include <QRect>
#include <QPoint>

/**
 * @brief Layout-Manager der Widgets wie Wörter in einem Text umbricht.
 *
 * Widgets werden von links nach rechts angeordnet; wenn kein Platz mehr vorhanden
 * ist, wird eine neue Zeile begonnen. Die Layout-Höhe passt sich dynamisch an.
 *
 * @par Verwendung:
 * @code
 * FlowLayout *layout = new FlowLayout(widget);
 * layout->addWidget(new QPushButton("Eins"));
 * layout->addWidget(new QPushButton("Zwei"));
 * // ... beliebig viele Widgets
 * @endcode
 *
 * @note Implementierung basiert auf dem Qt-Beispiel "Flow Layout"
 *       (docs.qt.io/qt-6/qtwidgets-layouts-flowlayout-example.html).
 */
class FlowLayout : public QLayout
{
public:
    /**
     * @brief Konstruktor mit Parent-Widget.
     * @param parent    Parent-Widget (setzt automatisch als Layout).
     * @param margin    Rand in Pixeln; -1 = Style-Standard.
     * @param hSpacing  Horizontaler Abstand zwischen Widgets; -1 = Style-Standard.
     * @param vSpacing  Vertikaler Abstand zwischen Zeilen; -1 = Style-Standard.
     */
    explicit FlowLayout(QWidget *parent, int margin = -1, int hSpacing = -1, int vSpacing = -1);

    /**
     * @brief Freistehender Konstruktor (kein Parent-Widget).
     * @param margin    Rand in Pixeln; -1 = Style-Standard.
     * @param hSpacing  Horizontaler Abstand; -1 = Style-Standard.
     * @param vSpacing  Vertikaler Abstand; -1 = Style-Standard.
     */
    explicit FlowLayout(int margin = -1, int hSpacing = -1, int vSpacing = -1);
    ~FlowLayout();

    void addItem(QLayoutItem *item) override;

    /** @brief Horizontaler Abstand (explizit oder Style-Standard). */
    int horizontalSpacing() const;
    /** @brief Vertikaler Abstand zwischen Zeilen (explizit oder Style-Standard). */
    int verticalSpacing() const;

    Qt::Orientations expandingDirections() const override;
    bool hasHeightForWidth() const override;

    /** @brief Berechnet die benötigte Höhe bei gegebener Breite. */
    int heightForWidth(int) const override;
    int count() const override;
    QLayoutItem *itemAt(int index) const override;
    QSize minimumSize() const override;
    void setGeometry(const QRect &rect) override;
    QSize sizeHint() const override;
    QLayoutItem *takeAt(int index) override;

private:
    /**
     * @brief Kernalgorithmus des Layouts.
     * @param rect      Verfügbarer Bereich.
     * @param testOnly  true = nur Höhe berechnen, Positionen nicht setzen.
     * @return Benötigte Höhe.
     */
    int doLayout(const QRect &rect, bool testOnly) const;

    /** @brief Gibt den systemabhängigen Abstand für die gegebene Pixelmetrik zurück. */
    int smartSpacing(QStyle::PixelMetric pm) const;

    QList<QLayoutItem *> itemList; ///< Alle enthaltenen Layout-Elemente
    int m_hSpace;                  ///< Expliziter horizontaler Abstand (-1 = auto)
    int m_vSpace;                  ///< Expliziter vertikaler Abstand (-1 = auto)
};



#endif // FLOWLAYOUT_H
