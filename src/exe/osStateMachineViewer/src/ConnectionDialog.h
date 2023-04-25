// \brief Declaration of the class ConnectionDialog

#pragma once
#include <QtWidgets>

QT_BEGIN_NAMESPACE
namespace Ui {
    class ConnectionDialog;
}
QT_END_NAMESPACE

namespace NS_OSBASE::statemachineviewer {

    /**
     * \brief Connection to the log service
     */
    class ConnectionDialog : public QDialog {
        Q_OBJECT

    public:
        ConnectionDialog(QWidget *pParent);
        ~ConnectionDialog() override;

        void accept() override;
        const QString &getHost() const;
        unsigned short getPort() const;

    private:
        void connectUi();

        Ui::ConnectionDialog *ui;
        QString m_hostName;
        unsigned short m_port;
    };
} // namespace NS_OSBASE::statemachineviewer
