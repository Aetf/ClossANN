#ifndef AWESOMEICONPROVIDER_H
#define AWESOMEICONPROVIDER_H

#include "QtAwesome.h"

class AwesomeIconProvider
{
public:
    AwesomeIconProvider() = delete;

    static QtAwesome *instance();
    static void drop();

private:
    static QtAwesome *awesome;
};

#endif // AWESOMEICONPROVIDER_H
