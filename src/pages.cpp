#include "include/pages.h"

pages::pages(QObject *parent)
    : QObject(parent), p_value(-1)
{
}

int pages::page_value() const
{
    return p_value;
}

void pages::set_page(int value)
{
    if (value != p_value){
        p_value = value;
        emit page_changed();
    }
}


QString pages::getPageInfo(int page){

    QString pageInfo = "Homepage";

    switch (page) {
    case 0:
        pageInfo = "Play";
        break;
    case 1:
        pageInfo = "Editor";
        break;
    case 2:
        pageInfo = "Settings";
        break;
    default:
        break;
    }

    return pageInfo;
}
