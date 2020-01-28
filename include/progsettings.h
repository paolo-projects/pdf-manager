#ifndef PROGSETTINGS_H
#define PROGSETTINGS_H

#include <QList>
#include <QString>
#include <QMimeData>
#include <QUrl>
#include <QFileInfo>

namespace SETTINGS
{
    constexpr int DEFAULT_PROGBAR_DELAY_BEFORE_HIDE = 3000;
    constexpr bool DEFAULT_PROGBAR_DISPLAY_TEXT = false;

    const QString PDFPAGERANGESPECIFICATOR_P_MIME_TYPE = "pdfpagerangespecificator/pointer";
    const QString DOCUMENTRANGESPECIFICATOR_MIME_TYPE = "documentrangespecificator/type";

    const QString PDFLIST_HINT = "Drop a PDF here to open it";
    const QString PAGESLIST_HINT = "Drop pages here or add them through the left controls";
    const QString PAGESPREVIEW_HINT = "Click on an open document to show a preview of its pages";
    const QList<QString> supportedImages = {"jpg", "jpeg", "png", "bmp", "gif"};

    static bool isUriListImage(const QMimeData* mimeData)
    {
        if(mimeData->hasFormat("text/uri-list"))
        {
            auto urls = mimeData->urls();
            for(auto url : urls)
            {
                QFileInfo fInfo(url.toLocalFile());
                bool fileimg = false;
                for(const auto& currentExt : supportedImages)
                {
                    if(fInfo.suffix().toLower() == currentExt)
                    {
                        fileimg = true;
                        break;
                    }
                }
                if(!fileimg)
                    return false;
            }
            return true;
        } else
            return false;
    }

#ifdef Q_OS_MACOS
    constexpr int HINTFONT_SIZE = 16;
#else
    constexpr int HINTFONT_SIZE = 12;
#endif
}

#endif // PROGSETTINGS_H
