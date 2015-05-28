#include "awesomeiconprovider.h"

QtAwesome *AwesomeIconProvider::awesome = nullptr;

QtAwesome *AwesomeIconProvider::instance()
{
    if (!awesome) {
        awesome = new QtAwesome;
        awesome->initFontAwesome();
    }
    return awesome;
}

void AwesomeIconProvider::drop()
{
    if (awesome) {
        delete awesome;
        awesome = nullptr;
    }
}
