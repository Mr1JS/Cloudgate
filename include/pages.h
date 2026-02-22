/**
 * @file pages.h
 * @brief Defines the pages class for managing application page navigation
 */

#ifndef PAGES_H
#define PAGES_H

#include <QObject>

class pages : public QObject
{

    Q_OBJECT

    /// Current page index (bridge to qml)
    Q_PROPERTY(int page_value READ page_value WRITE set_page NOTIFY page_changed) // this makes start available as a QML property

public:

    /// QObject based Constructor
    explicit pages(QObject *parent = nullptr);

    /// @return current page
    int page_value() const;

    /// @return info of page
    QString getPageInfo(int page);

public slots:

    /// Set page
    void set_page(int value);

signals:

    /// Emit that the page changed
    void page_changed();


private:

    /// Current page
    int p_value;
};

#endif // PAGES_H
