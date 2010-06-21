#ifndef SYSTEMMENUSHEET_H
#define SYSTEMMENUSHEET_H

#include <QuickGUI.h>
#include <boost/utility.hpp>

class SystemMenuSheet: public boost::noncopyable
{
public:
    SystemMenuSheet();
    ~SystemMenuSheet() {}
    void Activate();
private:
    QuickGUI::Sheet* mSheet;
};

#endif // SYSTEMMENUSHEET_H
