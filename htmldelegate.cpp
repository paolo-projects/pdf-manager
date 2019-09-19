#include "htmldelegate.h"

void HTMLDelegate::paint(QPainter* painter, const QStyleOptionViewItem & option, const QModelIndex &index) const
{
    painter->save();

    QStyleOptionViewItem options = QStyleOptionViewItem(option);
    initStyleOption(&options, index);

    QTextDocument doc;
    doc.setDocumentMargin(0);
    doc.setHtml(options.text);
    options.text = "";

    QRect rect = options.rect;
    options.rect = QRect(rect.x()+3, rect.y(), rect.width(), rect.height());

    QStyle* style;

    if(options.widget == nullptr)
        style = QApplication::style();
    else
        style = options.widget->style();

    style->drawControl(QStyle::CE_ItemViewItem, &options, painter);

    QAbstractTextDocumentLayout::PaintContext ctx;

    if (option.state & QStyle::State_Selected)
        ctx.palette.setColor(QPalette::Text, Qt::red);
                             //option.palette.color(QPalette::Active, QPalette::HighlightedText));

    QRect textRect = style->subElementRect(QStyle::SE_ItemViewItemText, &options);

    painter->setPen(Qt::gray);
    painter->drawLine(0, textRect.bottomRight().y(), textRect.bottomRight().x(), textRect.bottomRight().y());

    painter->translate(textRect.topLeft());
    doc.documentLayout()->draw(painter, ctx);

    painter->restore();
}

QSize HTMLDelegate::sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    QStyleOptionViewItem options = option;
    initStyleOption(&options, index);

    QTextDocument doc;
    doc.setHtml(options.text);
    doc.setTextWidth(options.rect.width());
    doc.adjustSize();
    return QSize(doc.idealWidth(), doc.size().height());
}
