#ifndef PROGSETTINGS_H
#define PROGSETTINGS_H

#include <QString>

namespace SETTINGS
{
    constexpr int DEFAULT_PROGBAR_DELAY_BEFORE_HIDE = 3000;
    constexpr bool DEFAULT_PROGBAR_DISPLAY_TEXT = false;

    const QString PDFPAGERANGESPECIFICATOR_P_MIME_TYPE = "pdfpagerangespecificator/pointer";
    const QString DOCUMENTRANGESPECIFICATOR_MIME_TYPE = "documentrangespecificator/type";
}

#endif // PROGSETTINGS_H
