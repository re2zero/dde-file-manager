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
#ifndef GLOBALEVENTRECEIVER_H
#define GLOBALEVENTRECEIVER_H

#include "dfmplugin_utils_global.h"

#include <QObject>
#include <QUrl>

QT_BEGIN_NAMESPACE
class QMimeData;
QT_END_NAMESPACE

DPUTILS_BEGIN_NAMESPACE

class GlobalEventReceiver : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(GlobalEventReceiver)

public:
    static GlobalEventReceiver *instance();
    void initEventConnect();

public slots:
    void handleOpenAsAdmin(const QUrl &url);
    bool handleSetMouseStyle(const QUrl &toUrl, const QList<QUrl> &fromUrls, void *type);
    bool handleDragDropCompress(const QUrl &toUrl, const QList<QUrl> &fromUrls);
    bool handleSetMouseStyleOnDesktop(int viewIndex, const QMimeData *mime, const QPoint &viewPos, void *extData);
    bool handleDragDropCompressOnDesktop(int viewIndex, const QMimeData *md, const QPoint &viewPos, void *extData);

private:
    explicit GlobalEventReceiver(QObject *parent = nullptr);
};

DPUTILS_END_NAMESPACE

#endif   // GLOBALEVENTRECEIVER_H
