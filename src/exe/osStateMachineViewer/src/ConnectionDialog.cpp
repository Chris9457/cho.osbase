// \brief Declaration of the class ConnectionDialog

#include "ConnectionDialog.h"
#include "ui_connectiondialog.h"

namespace NS_OSBASE::statemachineviewer {

    /*
     * \class ConnectionDialog
     */
    ConnectionDialog::ConnectionDialog(QWidget *pParent) : QDialog(pParent), ui(new Ui::ConnectionDialog) {
        connectUi();
    }

    ConnectionDialog::~ConnectionDialog() {
        delete ui;
    }

    void ConnectionDialog::accept() {
        QDialog::accept();

        m_hostName = ui->hostEdit->text();
        m_port     = ui->portEdit->text().toUShort();
        m_realm    = ui->realmEdit->text();
    }

    const QString &ConnectionDialog::getHost() const {
        return m_hostName;
    }

    unsigned short ConnectionDialog::getPort() const {
        return m_port;
    }

    const QString &ConnectionDialog::getRealm() const {
        return m_realm;
    }

    void ConnectionDialog::connectUi() {
        ui->setupUi(this);

        ui->portEdit->setValidator(new QIntValidator(0, 65535, this));

        ui->hostEdit->setText("localhost");
        ui->portEdit->setText("8080");
        ui->realmEdit->setText("osbase");
    }
} // namespace NS_OSBASE::statemachineviewer
