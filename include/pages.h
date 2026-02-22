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
    Q_PROPERTY(int page_value READ page_value WRITE set_page NOTIFY page_changed) // this makes start available as a QML property

public:
    explicit pages(QObject *parent = nullptr);

    int page_value() const;

    QString getPageInfo(int page);

public slots:
    void set_page(int value);

signals:
    void page_changed();


private:
    int p_value;
};

#endif // PAGES_H
