/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
 *               2016 ~ 2018 dragondjf
 *
 * Author:     dragondjf<dingjiangfeng@deepin.com>
 *
 * Maintainer: dragondjf<dingjiangfeng@deepin.com>
 *             zccrs<zhangjide@deepin.com>
 *             Tangtong<tangtong@deepin.com>
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

#include "dtoolbar.h"
#include "dfmcrumbbar.h"
#include "historystack.h"
#include "historystack.h"
#include "windowmanager.h"
#include "dfileservices.h"
#include "dfmeventdispatcher.h"
#include "dfmcrumbitem.h"
#include "dfmaddressbar.h"

#include "dfmevent.h"
#include "app/define.h"
#include "app/filesignalmanager.h"

#include "dfilemenumanager.h"

#include "singleton.h"
#include "views/dfileview.h"
#include "views/dfilemanagerwindow.h"
#include "views/dfmactionbutton.h"

#include <DButtonBox>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>
#include <QToolButton>

const int DToolBar::ButtonWidth = 24;
const int DToolBar::ButtonHeight = 24;
static const QSize iconSize(16, 16);

/*!
 * \class DToolBar
 *
 * \brief DToolBar is the toolbar (topbar) of the file manager
 *
 * This widget is the container of the navigation arrows, switching view buttons, menu button,
 * search button and the crumb bar (and also the address bar).
 *
 * \sa DFMCrumbBar, DFMAddressBar
 */

DToolBar::DToolBar(QWidget *parent) : QFrame(parent)
{
    initData();
    initUI();
    initConnect();
}

DToolBar::~DToolBar()
{
    delete m_navStack;
}

void DToolBar::initData()
{
//    m_navStack = new HistoryStack(65536);
}

void DToolBar::initUI()
{
    setFocusPolicy(Qt::NoFocus);
    initAddressToolBar();
    initContollerToolBar();

    m_detailButton = new QPushButton(this);
    m_detailButton->setFixedWidth(ButtonWidth);
    m_detailButton->setFixedHeight(ButtonHeight);
    m_detailButton->setObjectName("detailButton");
    m_detailButton->setCheckable(true);
    m_detailButton->setFocusPolicy(Qt::NoFocus);
    m_detailButton->setIcon(QIcon::fromTheme("dfm_rightview_detail"));
    m_detailButton->setIconSize(iconSize);
    m_detailButton->setFlat(true);

    QHBoxLayout* mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_addressToolBar);
    mainLayout->addSpacing(22);
    mainLayout->addWidget(m_contollerToolBar);

    mainLayout->addSpacing(26);
    mainLayout->addWidget(m_detailButton);

    mainLayout->addSpacing(0);
    mainLayout->setContentsMargins(14, 0, 14, 0);
    setLayout(mainLayout);
}

void DToolBar::initAddressToolBar()
{
    m_addressToolBar = new QFrame;
    m_addressToolBar->setObjectName("AddressToolBar");
    //m_addressToolBar->setFixedHeight(40);

    QHBoxLayout * backForwardLayout = new QHBoxLayout;

    m_backButton = new DButtonBoxButton(QStyle::SP_ArrowBack);
    m_backButton->setDisabled(true);
    m_backButton->setFixedWidth(36);

    m_forwardButton = new DButtonBoxButton(QStyle::SP_ArrowForward);
    m_forwardButton->setDisabled(true);
    m_forwardButton->setFixedWidth(36);

    QList<DButtonBoxButton*> buttonList;
    buttonList << m_backButton << m_forwardButton;

    DButtonBox* buttonBox = new DButtonBox(this);
    buttonBox->setButtonList(buttonList, false);
    buttonBox->setFocusPolicy(Qt::NoFocus);

    m_searchButton = new QPushButton(this);
    m_searchButton->setObjectName("searchButton");
    m_searchButton->setFixedSize(ButtonWidth, ButtonHeight);
    m_searchButton->setFocusPolicy(Qt::NoFocus);
    m_searchButton->setFlat(true);
    m_searchButton->setIcon(QIcon::fromTheme("search"));
    m_searchButton->setIconSize(iconSize);


    backForwardLayout->addWidget(buttonBox);
    backForwardLayout->setSpacing(0);
    backForwardLayout->setContentsMargins(0, 0, 0, 0);


    QFrame * crumbAndSearch = new QFrame;
    m_crumbWidget = new DFMCrumbBar(this);
    crumbAndSearch->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QHBoxLayout * comboLayout = new QHBoxLayout;
    comboLayout->addWidget(m_crumbWidget);
    comboLayout->addWidget(m_searchButton);
    comboLayout->setSpacing(10);
    comboLayout->setContentsMargins(0, 0, 0, 0);

    crumbAndSearch->setLayout(comboLayout);

    QHBoxLayout* mainLayout = new QHBoxLayout;
    mainLayout->addLayout(backForwardLayout);
    mainLayout->addWidget(crumbAndSearch);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(10);
    m_addressToolBar->setLayout(mainLayout);
}

void DToolBar::initContollerToolBar()
{
    m_contollerToolBar = new QFrame;
    m_contollerToolBar->setObjectName("ContollerToolBar");
    m_contollerToolBar->setFrameShape(QFrame::NoFrame);
    m_contollerToolBar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    m_contollerToolBarContentLayout = new QHBoxLayout(m_contollerToolBar);
    m_contollerToolBarContentLayout->setContentsMargins(1, 1, 1, 1);
    m_contollerToolBarContentLayout->setSpacing(22);
}

void DToolBar::initConnect()
{
    connect(m_detailButton, &QPushButton::clicked,this, &DToolBar::detailButtonClicked);
    connect(m_backButton, &DButtonBoxButton::clicked, this, &DToolBar::onBackButtonClicked);
    connect(m_forwardButton, &DButtonBoxButton::clicked, this, &DToolBar::onForwardButtonClicked);
    connect(m_crumbWidget, &DFMCrumbBar::addressBarContentEntered, this, &DToolBar::searchBarTextEntered);
    connect(m_crumbWidget, &DFMCrumbBar::crumbItemClicked, this, &DToolBar::crumbSelected);
    connect(m_crumbWidget, &DFMCrumbBar::crumbListItemSelected, this, [this](const DUrl &url){
        DFMEventDispatcher::instance()->processEvent<DFMChangeCurrentUrlEvent>(m_crumbWidget, url, window());
    });
    connect(m_crumbWidget, &DFMCrumbBar::addressBarShown, this, &DToolBar::searchBarActivated);
    connect(m_crumbWidget, &DFMCrumbBar::addressBarHidden, this, &DToolBar::searchBarDeactivated);
    connect(m_searchButton, &QPushButton::clicked, this, &DToolBar::onSearchButtonClicked);
    connect(fileSignalManager, &FileSignalManager::currentUrlChanged, this, &DToolBar::currentUrlChanged);
    connect(fileSignalManager, &FileSignalManager::requestSearchCtrlF, this, &DToolBar::handleHotkeyCtrlF);
    connect(fileSignalManager, &FileSignalManager::requestSearchCtrlL, this, &DToolBar::handleHotkeyCtrlL);
    connect(this, &DToolBar::toolbarUrlChanged, m_crumbWidget, &DFMCrumbBar::updateCurrentUrl);

    DFileManagerWindow *window = qobject_cast<DFileManagerWindow*>(parent());

    if (window) {
        connect(window, &DFileManagerWindow::currentViewStateChanged, this, [this, window] {
            if (window->currentViewState() == DFMBaseView::ViewBusy)
                m_crumbWidget->playAddressBarAnimation();
            else
                m_crumbWidget->stopAddressBarAnimation();
        });
    }
}

DFMCrumbBar *DToolBar::getCrumbWidget()
{
    return m_crumbWidget;
}

void DToolBar::searchBarActivated()
{
    toggleSearchButtonState(true);
}

void DToolBar::searchBarDeactivated()
{
    toggleSearchButtonState(false);
}

/**
 * \brief DToolBar::searchBarTextEntered
 *
 * Set the tab bar when return press is detected
 * on search bar.
 */
void DToolBar::searchBarTextEntered(const QString textEntered)
{
    QString text = textEntered;

    if (text.isEmpty()) {
        //m_searchBar->clearText();
        return;
    }

    const QString &currentDir = QDir::currentPath();
    const DUrl &currentUrl = qobject_cast<DFileManagerWindow*>(topLevelWidget())->currentUrl();

    if (currentUrl.isLocalFile()) {
        QDir::setCurrent(currentUrl.toLocalFile());
    }

    DUrl inputUrl = DUrl::fromUserInput(text, false); ///###: here, judge whether the text is a local file path.

    QDir::setCurrent(currentDir);

    DFMEventDispatcher::instance()->processEvent<DFMChangeCurrentUrlEvent>(this, inputUrl, window());
}

void DToolBar::onSearchButtonClicked()
{
    if (!m_searchButtonAsbState) {
        m_crumbWidget->showAddressBar("");
    } else {
        // toggle asb visible
        DFileManagerWindow* dfmWindow = qobject_cast<DFileManagerWindow*>(window());
        bool oldState = dfmWindow->isAdvanceSearchBarVisible();
        dfmWindow->toggleAdvanceSearchBar(!oldState);
        m_searchButton->setDown(!oldState);
    }
}

void DToolBar::crumbSelected(const DFMCrumbItem* item)
{
    DFMEventDispatcher::instance()->processEvent<DFMChangeCurrentUrlEvent>(m_crumbWidget, item->url(), window());
}

void DToolBar::currentUrlChanged(const DFMEvent &event)
{
    if (event.windowId() != WindowManager::getWindowId(this)) {
        return;
    }

    if (event.sender() == m_crumbWidget) {
        pushUrlToHistoryStack(event.fileUrl());
        return;
    }

    emit toolbarUrlChanged(event.fileUrl());

    if (event.sender() == this) {
        return;
    }

    pushUrlToHistoryStack(event.fileUrl());
}

void DToolBar::back()
{
    DUrl url = m_navStack->back();

    if(!url.isEmpty())
    {
        updateBackForwardButtonsState();
        DFMEventDispatcher::instance()->processEvent<DFMChangeCurrentUrlEvent>(this, url, window());
    }
}

void DToolBar::forward()
{
    DUrl url = m_navStack->forward();

    if(!url.isEmpty())
    {
        updateBackForwardButtonsState();
        DFMEventDispatcher::instance()->processEvent<DFMChangeCurrentUrlEvent>(this, url, window());
    }
}

void DToolBar::handleHotkeyCtrlF(quint64 winId)
{
    if (winId == WindowManager::getWindowId(this)) {
        onSearchButtonClicked();
    }
}

void DToolBar::handleHotkeyCtrlL(quint64 winId)
{
    if (winId == WindowManager::getWindowId(this)) {
        m_crumbWidget->showAddressBar(qobject_cast<DFileManagerWindow*>(topLevelWidget())->currentUrl());
    }
}

void DToolBar::pushUrlToHistoryStack(DUrl url)
{
    if (!m_navStack) {
        return;
    }

    m_navStack->append(url);
    updateBackForwardButtonsState();
}

void DToolBar::addHistoryStack(){
    m_navStacks.append(new HistoryStack(65536));
}

void DToolBar::switchHistoryStack(const int index){
    m_navStack = m_navStacks.at(index);
    if(!m_navStack)
        return;
    updateBackForwardButtonsState();
}

namespace DEEPIN_QT_THEME {
extern QThreadStorage<QString> colorScheme;
extern void(*setFollowColorScheme)(bool);
extern bool(*followColorScheme)();
}

/*!
 * \brief Switch Search Button State
 *
 * \param asb Make search button control Advance Search Bar
 */
void DToolBar::toggleSearchButtonState(bool asb)
{
    if (DEEPIN_QT_THEME::followColorScheme
            && (*DEEPIN_QT_THEME::followColorScheme)() ) {
        const QWidget *widget = m_searchButton;
        const QPalette &pal = widget->palette();
        DEEPIN_QT_THEME::colorScheme.setLocalData(pal.windowText().color().name());
    }

    if (asb) {
        m_searchButton->setObjectName("filterButton");
        m_searchButton->setIcon(QIcon::fromTheme("dfm_view_filter"));
        m_searchButton->style()->unpolish(m_searchButton);
        m_searchButton->style()->polish(m_searchButton);
        m_searchButton->setFlat(true);
        m_searchButtonAsbState = true;
    } else {
        m_searchButton->style()->unpolish(m_searchButton);
        m_searchButton->style()->polish(m_searchButton);
        m_searchButton->setIcon(QIcon::fromTheme("search"));
        m_searchButton->setDown(false);
        m_searchButtonAsbState = false;
        DFileManagerWindow* dfmWindow = qobject_cast<DFileManagerWindow*>(window());
        dfmWindow->toggleAdvanceSearchBar(false);
    }
}

void DToolBar::removeNavStackAt(int index){
    m_navStacks.removeAt(index);

    if(index < m_navStacks.count())
        m_navStack = m_navStacks.at(index);
    else
        m_navStack = m_navStacks.at(m_navStacks.count()-1);

    if(!m_navStack)
        return;
    if(m_navStack->size() > 1)
        m_backButton->setEnabled(true);
    else
        m_backButton->setEnabled(false);

    if(m_navStack->isLast())
        m_forwardButton->setEnabled(false);
    else
        m_forwardButton->setEnabled(true);
}

void DToolBar::moveNavStacks(int from, int to){
    m_navStacks.move(from,to);
}

int DToolBar::navStackCount() const{
    return m_navStacks.count();
}

void DToolBar::updateBackForwardButtonsState()
{
    if(m_navStack->size() <= 1){
        m_backButton->setEnabled(false);
        m_forwardButton->setEnabled(false);
    }
    else{
        if(m_navStack->isFirst())
            m_backButton->setEnabled(false);
        else
            m_backButton->setEnabled(true);

        if(m_navStack->isLast())
            m_forwardButton->setEnabled(false);
        else
            m_forwardButton->setEnabled(true);
    }
}

void DToolBar::setCustomActionList(const QList<QAction *> &list)
{
    m_actionList = list;

    for (DFMActionButton *button : m_contollerToolBar->findChildren<DFMActionButton*>()) {
        m_contollerToolBarContentLayout->removeWidget(button);
        button->deleteLater();
    }

    for (int i = 0; i < list.count(); ++i) {
        DFMActionButton *button = new DFMActionButton(this);
        button->setFixedSize(ButtonWidth - 2, ButtonHeight - 2);
        button->setFocusPolicy(Qt::NoFocus);
        button->setAction(list.at(i));
        button->setIconSize(iconSize);

        m_contollerToolBarContentLayout->addWidget(button);

        if (list.at(i)->isVisible()) {
            button->show();
        }
    }

    m_contollerToolBar->setHidden(list.isEmpty());

    if (m_detailButton) {
        m_detailButton->setHidden(list.isEmpty());
        if(m_detailButton->isChecked() && list.isEmpty())
            m_detailButton->click();
    }
}

void DToolBar::triggerActionByIndex(int index)
{
    if (QAction *action = m_actionList.value(index)) {
        if (action->isVisible()) {
            action->trigger();
        }
    }
}

void DToolBar::onBackButtonClicked()
{
    DFMEventDispatcher::instance()->processEvent(dMakeEventPointer<DFMBackEvent>(this), qobject_cast<DFileManagerWindow*>(window()));
}

void DToolBar::onForwardButtonClicked()
{
    DFMEventDispatcher::instance()->processEvent(dMakeEventPointer<DFMForwardEvent>(this), qobject_cast<DFileManagerWindow*>(window()));
}
