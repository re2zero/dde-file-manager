/*
 * Copyright (C) 2022 Uniontech Software Technology Co., Ltd.
 *
 * Author:     zhangsheng<zhangsheng@uniontech.com>
 *
 * Maintainer: max-lv<lvwujun@uniontech.com>
 *             lanxuesong<lanxuesong@uniontech.com>
 *             xushitong<xushitong@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "dialogmanager.h"

#include "dfm-base/dialogs/mountpasswddialog/mountaskpassworddialog.h"
#include "dfm-base/dialogs/mountpasswddialog/mountsecretdiskaskpassworddialog.h"
#include "dfm-base/dialogs/settingsdialog/settingdialog.h"
#include "dfm-base/dialogs/taskdialog/taskdialog.h"
#include "dfm-base/interfaces/abstractfileinfo.h"
#include "dfm-base/base/schemefactory.h"
#include "dfm_global_defines.h"
#include "dfm-base/file/local/localfilehandler.h"

DFMBASE_USE_NAMESPACE

DialogManager *DialogManager::instance()
{
    static DialogManager ins;
    return &ins;
}

DDialog *DialogManager::showQueryScanningDialog(const QString &title)
{   // stop scan device
    DDialog *d = new DDialog;
    d->setTitle(title);
    d->setAttribute(Qt::WA_DeleteOnClose);
    Qt::WindowFlags flags = d->windowFlags();
    d->setWindowFlags(flags | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
    d->setIcon(QIcon::fromTheme("dialog-warning"));
    d->addButton(QObject::tr("Cancel", "button"));
    d->addButton(QObject::tr("Stop", "button"), true, DDialog::ButtonWarning);   // 终止
    d->setMaximumWidth(640);
    d->show();
    return d;
}

void DialogManager::showErrorDialog(const QString &title, const QString &message)
{
    DDialog d(title, message);
    Qt::WindowFlags flags = d.windowFlags();
    // dialog show top
    d.setWindowFlags(flags | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
    d.setIcon(QIcon::fromTheme("dialog-error"));
    d.addButton(tr("Confirm", "button"), true, DDialog::ButtonRecommend);
    d.setMaximumWidth(640);
    d.exec();
}

int DialogManager::showMessageDialog(DialogManager::MessageType messageLevel, const QString &title, const QString &message, QString btnTxt)
{
    DDialog d(title, message);
    d.moveToCenter();
    QStringList buttonTexts;
    buttonTexts.append(btnTxt);
    d.addButtons(buttonTexts);
    d.setDefaultButton(0);
    if (messageLevel == kMsgWarn) {
        d.setIcon(QIcon::fromTheme("dialog-warning"));
    } else if (messageLevel == kMsgErr) {
        d.setIcon(QIcon::fromTheme("dialog-error"));
    } else {
        d.setIcon(QIcon::fromTheme("dialog-information"));
    }
    int code = d.exec();
    return code;
}

void DialogManager::showErrorDialogWhenMountDeviceFailed(dfmmount::DeviceError err)
{
    switch (err) {
    case dfmmount::DeviceError::UserErrorNetworkAnonymousNotAllowed:
        showErrorDialog(tr("Mount error"), tr("Anonymous mount is not allowed"));
        break;
    case dfmmount::DeviceError::UserErrorNetworkWrongPasswd:
        showErrorDialog(tr("Mount error"), tr("Wrong password is inputed"));
        break;
    case dfmmount::DeviceError::UserErrorUserCancelled:
        break;
    default:
        showErrorDialog(tr("Mount error"), tr("Error occured while mounting device"));
        qWarning() << "mount device failed: " << static_cast<int>(err);
        break;
    }
}

void DialogManager::showErrorDialogWhenUnmountDeviceFailed(dfmmount::DeviceError err)
{
    switch (err) {
    case dfmmount::DeviceError::UDisksErrorDeviceBusy:
        showErrorDialog(tr("The device was not safely unmounted"), tr("The device is busy, cannot remove now"));
        break;
    default:
        showErrorDialog(tr("The device was not safely unmounted"), tr("The device is busy, cannot remove now"));
        break;
    }
}

void DialogManager::showNoPermissionDialog(const QList<QUrl> &urls)
{
    qDebug() << urls << "no perssion";
    if (urls.isEmpty()) {
        return;
    }

    QFont f;
    f.setPixelSize(16);
    QFontMetrics fm(f);

    DDialog d;

    QIcon icon = QIcon::fromTheme("dialog-warning");
    if (urls.count() == 1) {

        d.setTitle(tr("You do not have permission to operate file/folder!"));
        QString message = urls.at(0).toLocalFile();

        if (fm.width(message) > Global::kMaxFileNameCharCount) {
            message = fm.elidedText(message, Qt::ElideMiddle, Global::kMaxFileNameCharCount);
        }

        d.setMessage(message);
        d.setIcon(icon);
    } else {

        QFrame *contentFrame = new QFrame;

        QLabel *iconLabel = new QLabel;
        iconLabel->setPixmap(icon.pixmap(64, 64));

        QLabel *titleLabel = new QLabel;
        titleLabel->setText(tr("Sorry, you don't have permission to operate the following %1 file/folder(s)!").arg(QString::number(urls.count())));

        QLabel *messageLabel = new QLabel;
        messageLabel->setScaledContents(true);

        QString message;
        for (int i = 0; i < urls.count(); i++) {
            if (i >= 10) {
                break;
            }
            QString s = QString("%1.%2").arg(QString::number(i + 1), urls.at(i).toLocalFile());
            if (fm.width(s) > Global::kMaxFileNameCharCount) {
                s = fm.elidedText(s, Qt::ElideMiddle, Global::kMaxFileNameCharCount);
            }
            message += s + "\n";
        }
        messageLabel->setText(message);

        QVBoxLayout *contentLayout = new QVBoxLayout;
        contentLayout->addWidget(iconLabel, 0, Qt::AlignCenter);
        contentLayout->addWidget(titleLabel, 0, Qt::AlignCenter);
        contentLayout->addWidget(messageLabel, 0, Qt::AlignCenter);
        contentLayout->setContentsMargins(0, 0, 0, 0);
        contentLayout->setSpacing(10);
        contentFrame->setLayout(contentLayout);

        d.addContent(contentFrame, Qt::AlignCenter);
    }

    d.addButton(tr("OK", "button"), true, DDialog::ButtonRecommend);
    d.exec();
}

/*!
 * \brief DialogService::addTask 添加一个文件操作任务，当收到这个任务的线程结束时，自己移除掉这个任务
 *
 * \param task 文件操作任务的处理器
 */
void DialogManager::addTask(const JobHandlePointer task)
{
    if (!taskdialog)
        taskdialog = new TaskDialog();

    taskdialog->addTask(task);
}

void DialogManager::showSetingsDialog(FileManagerWindow *window)
{
    Q_ASSERT(window);

    if (window->property("isSettingDialogShown").toBool()) {
        qWarning() << "isSettingDialogShown true";
        return;
    }
    window->setProperty("isSettingDialogShown", true);
    DSettingsDialog *dsd = new SettingDialog(window);
    dsd->show();
    connect(dsd, &DSettingsDialog::finished, [window] {
        window->setProperty("isSettingDialogShown", false);
    });
}

/*!
 * \brief DialogService::askPasswordForLockedDevice
 * \return the password user inputed.
 */
QString DialogManager::askPasswordForLockedDevice()
{
    MountSecretDiskAskPasswordDialog dialog(tr("Need password to unlock device"));
    return dialog.exec() == QDialog::Accepted ? dialog.getUerInputedPassword() : "";
}

bool DialogManager::askForFormat()
{
    DDialog dlg;
    dlg.setIcon(QIcon::fromTheme("dialog-warning"));
    dlg.addButton(tr("Cancel", "button"));
    dlg.addButton(tr("Format", "button"), true, DDialog::ButtonRecommend);
    dlg.setTitle(tr("To access the device, you must format the disk first. Are you sure you want to format it now?"));
    return dlg.exec() == QDialog::Accepted;
}

int DialogManager::showRunExcutableScriptDialog(const QUrl &url)
{
    DDialog d;
    const int maxDisplayNameLength = 250;

    AbstractFileInfoPointer info = InfoFactory::create<AbstractFileInfo>(url);

    const QString &fileDisplayName = info->fileDisplayName();
    const QString &fileDisplayNameNew = d.fontMetrics().elidedText(fileDisplayName, Qt::ElideRight, maxDisplayNameLength);
    const QString &message = tr("Do you want to run %1 or display its content?").arg(fileDisplayNameNew);
    const QString &tipMessage = tr("It is an executable text file.");
    QStringList buttonTexts;

    buttonTexts.append(tr("Cancel", "button"));
    buttonTexts.append(tr("Run", "button"));
    buttonTexts.append(tr("Run in terminal", "button"));
    buttonTexts.append(tr("Display", "button"));

    d.setIcon(QIcon::fromTheme("application-x-shellscript"));
    d.setTitle(message);
    d.setMessage(tipMessage);
    d.addButton(buttonTexts[0], true);
    d.addButton(buttonTexts[1], false);
    d.addButton(buttonTexts[2], false);
    d.addButton(buttonTexts[3], false, DDialog::ButtonRecommend);
    d.setDefaultButton(3);
    d.setFixedWidth(480);
    int code = d.exec();
    return code;
}

int DialogManager::showRunExcutableFileDialog(const QUrl &url)
{
    DDialog d;
    AbstractFileInfoPointer info = InfoFactory::create<AbstractFileInfo>(url);

    const int maxDisplayNameLength = 200;
    const QString &fileDisplayName = info->fileDisplayName();
    const QString &fileDisplayNameNew = d.fontMetrics().elidedText(fileDisplayName, Qt::ElideRight, maxDisplayNameLength);
    const QString &message = tr("Do you want to run %1?").arg(fileDisplayNameNew);
    const QString &tipMessage = tr("It is an executable file.");

    d.addButton(tr("Cancel", "button"));
    d.addButton(tr("Run in terminal", "button"));
    d.addButton(tr("Run", "button"), true, DDialog::ButtonRecommend);
    d.setTitle(message);
    d.setMessage(tipMessage);
    d.setIcon(info->fileIcon());
    int code = d.exec();
    return code;
}

DialogManager::DialogManager(QObject *parent)
    : QObject(parent)
{
}

DialogManager::~DialogManager()
{
}
