#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    ui->label->setText(ui->label->text().replace("{{applicationVersion}}", VER_PRODUCTVERSION_STR, Qt::CaseSensitive));
    layout()->setSizeConstraint( QLayout::SetFixedSize );
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
