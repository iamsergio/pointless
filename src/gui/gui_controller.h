// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include "taskfiltermodel.h"
#include "taskmodel.h"
#include "tagmodel.h"
#include "tagfiltermodel.h"
#include "error_controller.h"

#include <QDate>
#include <QObject>
#include <QQmlEngine>
#include <QtQml/qqmlregistration.h>
#include <QTimer>

class DataController;
class ErrorController;


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
    Q_PROPERTY(QString titleInEditor READ titleInEditor NOTIFY titleInEditorChanged)
    Q_PROPERTY(QString tagInEditor READ tagInEditor NOTIFY tagInEditorChanged)
    Q_PROPERTY(bool isEveningInEditor READ isEveningInEditor NOTIFY isEveningInEditorChanged)
    Q_PROPERTY(QDate dateInEditor READ dateInEditor NOTIFY dateInEditorChanged)
    Q_PROPERTY(QString windowTitle READ windowTitle CONSTANT)
    Q_PROPERTY(bool isAuthenticated READ isAuthenticated NOTIFY isAuthenticatedChanged)
    Q_PROPERTY(QString defaultLoginUsername READ defaultLoginUsername CONSTANT)
    Q_PROPERTY(bool taskMenuVisible READ taskMenuVisible NOTIFY taskMenuUuidChanged)
    Q_PROPERTY(QString taskMenuUuid READ taskMenuUuid NOTIFY taskMenuUuidChanged)
    Q_PROPERTY(bool moveToCurrentVisible READ moveToCurrentVisible NOTIFY taskMenuUuidChanged)
    Q_PROPERTY(bool moveToSoonVisible READ moveToSoonVisible NOTIFY taskMenuUuidChanged)
    Q_PROPERTY(bool moveToLaterVisible READ moveToLaterVisible NOTIFY taskMenuUuidChanged)
    Q_PROPERTY(bool moveToTomorrowVisible READ moveToTomorrowVisible NOTIFY taskMenuUuidChanged)
    Q_PROPERTY(bool moveToEveningVisible READ moveToEveningVisible NOTIFY taskMenuUuidChanged)
    Q_PROPERTY(bool isRefreshing READ isRefreshing NOTIFY isRefreshingChanged)
    Q_PROPERTY(bool isLoggingIn READ isLoggingIn NOTIFY isLoggingInChanged)
    Q_PROPERTY(ErrorController *errorController READ errorController CONSTANT)
    Q_PROPERTY(bool isOfflineMode READ isOfflineMode NOTIFY isOfflineModeChanged)
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
    Q_INVOKABLE void saveTask(QString title, const QString &tag, bool isEvening);
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
    [[nodiscard]] QString titleInEditor() const;
    [[nodiscard]] QString tagInEditor() const;
    [[nodiscard]] bool isEveningInEditor() const;
    [[nodiscard]] QDate dateInEditor() const;
    Q_INVOKABLE void setDateInEditor(QDate date);

    [[nodiscard]] QString windowTitle() const;
    [[nodiscard]] bool isAuthenticated() const;
    [[nodiscard]] bool taskMenuVisible() const;
    [[nodiscard]] QString taskMenuUuid() const;
    [[nodiscard]] bool moveToCurrentVisible() const;
    [[nodiscard]] bool moveToSoonVisible() const;
    [[nodiscard]] bool moveToLaterVisible() const;
    [[nodiscard]] bool moveToTomorrowVisible() const;
    [[nodiscard]] bool moveToEveningVisible() const;
    [[nodiscard]] bool isRefreshing() const;
    [[nodiscard]] bool isLoggingIn() const;

    Q_INVOKABLE void setTaskMenuUuid(const QString &uuid);
    Q_INVOKABLE void login(const QString &email, const QString &password);
    Q_INVOKABLE void logout();
    Q_INVOKABLE void enableOfflineMode();

    [[nodiscard]] bool isOfflineMode() const;

    [[nodiscard]] DataController *dataController() const;
    [[nodiscard]] ErrorController *errorController() const;

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
    void titleInEditorChanged();
    void tagInEditorChanged();
    void isEveningInEditorChanged();
    void navigatorStartDateChanged();
    void navigatorEndDateChanged();
    void isEditingChanged();
    void uuidBeingEditedChanged();
    void dateInEditorChanged();
    void isAuthenticatedChanged();
    void taskMenuUuidChanged();
    void isRefreshingChanged();
    void isLoggingInChanged();
    void isOfflineModeChanged();

private:
    explicit GuiController(QObject *parent = nullptr);
    ViewType _currentViewType = ViewType::Week;
    bool _isEditing = false;
    QString _uuidBeingEdited;
    QString _titleInEditor;
    QString _tagInEditor;
    bool _isEveningInEditor = false;
    QDate _dateInEditor;
    QDate _navigatorStartDate;
    QString _taskMenuUuid;
    bool _isRefreshing = false;
    bool _isLoggingIn = false;
    bool _isOfflineMode = false;
    DataController *const _dataController;
    ErrorController *const _errorController;
    mutable TaskFilterModel *_taskFilterModel = nullptr;
    mutable TagFilterModel *_tagFilterModel = nullptr;
};
