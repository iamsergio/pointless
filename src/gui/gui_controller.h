// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include "taskfiltermodel.h"
#include "taskmodel.h"
#include "tagmodel.h"
#include "tagfiltermodel.h"

#include <QDate>
#include <QObject>
#include <QQmlEngine>
#include <QtQml/qqmlregistration.h>

class DataController;

class GuiController : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    Q_PROPERTY(GuiController::ViewType currentViewType READ currentViewType WRITE setCurrentViewType NOTIFY currentViewTypeChanged)
    Q_PROPERTY(QDate navigatorStartDate READ navigatorStartDate NOTIFY navigatorStartDateChanged)
    Q_PROPERTY(QDate navigatorEndDate READ navigatorEndDate NOTIFY navigatorEndDateChanged)
    Q_PROPERTY(QString navigatorPrettyStartDate READ navigatorPrettyStartDate NOTIFY navigatorStartDateChanged)
    Q_PROPERTY(QString navigatorPrettyEndDate READ navigatorPrettyEndDate NOTIFY navigatorEndDateChanged)
    Q_PROPERTY(TaskFilterModel *taskFilterModel READ taskFilterModel CONSTANT)
    Q_PROPERTY(TaskModel *taskModel READ taskModel CONSTANT)
    Q_PROPERTY(TagModel *tagModel READ tagModel CONSTANT)
    Q_PROPERTY(TagFilterModel *tagFilterModel READ tagFilterModel CONSTANT)
    Q_PROPERTY(bool isDebug READ isDebug CONSTANT)
    Q_PROPERTY(bool isMobile READ isMobile CONSTANT)
    Q_PROPERTY(bool isAndroid READ isAndroid CONSTANT)
    Q_PROPERTY(bool isIOS READ isIOS CONSTANT)
    Q_PROPERTY(bool isVerbose READ isVerbose CONSTANT)
    Q_PROPERTY(bool isEditing READ isEditing NOTIFY isEditingChanged)
    Q_PROPERTY(QString uuidBeingEdited READ uuidBeingEdited NOTIFY uuidBeingEditedChanged)
    Q_PROPERTY(QDate dateInEditor READ dateInEditor NOTIFY dateInEditorChanged)
    Q_PROPERTY(QString windowTitle READ windowTitle CONSTANT)
    Q_PROPERTY(bool isAuthenticated READ isAuthenticated NOTIFY isAuthenticatedChanged)
    Q_PROPERTY(QString defaultLoginUsername READ defaultLoginUsername CONSTANT)
    Q_PROPERTY(QString loginError READ loginError NOTIFY loginErrorChanged)
    Q_PROPERTY(bool taskMenuVisible READ taskMenuVisible NOTIFY taskMenuUuidChanged)
    Q_PROPERTY(QString taskMenuUuid READ taskMenuUuid NOTIFY taskMenuUuidChanged)
    Q_PROPERTY(bool moveToCurrentVisible READ moveToCurrentVisible NOTIFY taskMenuUuidChanged)
    Q_PROPERTY(bool moveToSoonVisible READ moveToSoonVisible NOTIFY taskMenuUuidChanged)
    Q_PROPERTY(bool moveToLaterVisible READ moveToLaterVisible NOTIFY taskMenuUuidChanged)
    Q_PROPERTY(bool moveToTomorrowVisible READ moveToTomorrowVisible NOTIFY taskMenuUuidChanged)
    Q_PROPERTY(bool moveToEveningVisible READ moveToEveningVisible NOTIFY taskMenuUuidChanged)
public:
    [[nodiscard]] TaskFilterModel *taskFilterModel() const;
    [[nodiscard]] TaskModel *taskModel() const;
    [[nodiscard]] TagModel *tagModel() const;
    [[nodiscard]] TagFilterModel *tagFilterModel() const;
    [[nodiscard]] QString defaultLoginUsername() const;

    enum class ViewType : uint8_t {
        Week,
        Soon,
        Later
    };
    Q_ENUM(ViewType)

    static GuiController *instance();

    [[nodiscard]] ViewType currentViewType() const;
    void setCurrentViewType(ViewType viewType);

    [[nodiscard]] QDate navigatorStartDate() const;
    [[nodiscard]] QDate navigatorEndDate() const;
    [[nodiscard]] QString navigatorPrettyStartDate() const;
    [[nodiscard]] QString navigatorPrettyEndDate() const;
    void setNavigatorStartDate(QDate date);
    Q_INVOKABLE void navigatorGotoToday();
    Q_INVOKABLE void navigatorGotoNextWeek();
    Q_INVOKABLE void navigatorGotoPreviousWeek();
    Q_INVOKABLE void refresh();
    Q_INVOKABLE void addNewTask(QString title, const QString &tag, bool isEvening);
    [[nodiscard]] Q_INVOKABLE QString colorFromTag(const QString &tagName) const;
    [[nodiscard]] static bool isDebug();
    [[nodiscard]] static bool isMobile();
    [[nodiscard]] static bool isAndroid();
    [[nodiscard]] static bool isIOS();
    [[nodiscard]] static bool isVerbose();
    [[nodiscard]] bool isEditing() const;
    void setIsEditing(bool isEditing);
    [[nodiscard]] QString uuidBeingEdited() const;
    void setUuidBeingEdited(const QString &uuid);
    [[nodiscard]] QDate dateInEditor() const;
    Q_INVOKABLE void setDateInEditor(QDate date);

    [[nodiscard]] QString windowTitle() const;
    [[nodiscard]] bool isAuthenticated() const;
    [[nodiscard]] QString loginError() const;
    [[nodiscard]] bool taskMenuVisible() const;
    [[nodiscard]] QString taskMenuUuid() const;
    [[nodiscard]] bool moveToCurrentVisible() const;
    [[nodiscard]] bool moveToSoonVisible() const;
    [[nodiscard]] bool moveToLaterVisible() const;
    [[nodiscard]] bool moveToTomorrowVisible() const;
    [[nodiscard]] bool moveToEveningVisible() const;

    Q_INVOKABLE void setTaskMenuUuid(const QString &uuid);
    Q_INVOKABLE void login(const QString &email, const QString &password);
    Q_INVOKABLE void logout();

    [[nodiscard]] DataController *dataController() const;

    Q_INVOKABLE void dumpTaskDebug(const QString &taskUuid) const;
    Q_INVOKABLE void dumpDebug() const;
    Q_INVOKABLE void onBackClicked();

    Q_INVOKABLE void setTaskBeingEdited(const QString &uuid, QDate date);
    Q_INVOKABLE void clearTaskBeingEdited();
    Q_INVOKABLE void moveTaskToCurrent(const QString &taskUuid);
    Q_INVOKABLE void moveTaskToSoon(const QString &taskUuid);
    Q_INVOKABLE void moveTaskToLater(const QString &taskUuid);
    Q_INVOKABLE void moveTaskToTomorrow(const QString &taskUuid);
    Q_INVOKABLE void moveTaskToNextMonday(const QString &taskUuid);
    Q_INVOKABLE void moveTaskToEvening(const QString &taskUuid);
    Q_INVOKABLE void deleteTask(const QString &taskUuid);

    static GuiController *create(QQmlEngine *qmlEngine, QJSEngine *jsEngine);

Q_SIGNALS:
    void currentViewTypeChanged();
    void navigatorStartDateChanged();
    void navigatorEndDateChanged();
    void isEditingChanged();
    void uuidBeingEditedChanged();
    void dateInEditorChanged();
    void isAuthenticatedChanged();
    void loginErrorChanged();
    void taskMenuUuidChanged();

private:
    explicit GuiController(QObject *parent = nullptr);
    ViewType _currentViewType = ViewType::Week;
    bool _isEditing = false;
    QString _uuidBeingEdited;
    QDate _dateInEditor;
    QDate _navigatorStartDate;
    QString _loginError;
    QString _taskMenuUuid;
    DataController *const _dataController;
    mutable TaskFilterModel *_taskFilterModel = nullptr;
    mutable TagFilterModel *_tagFilterModel = nullptr;
};
