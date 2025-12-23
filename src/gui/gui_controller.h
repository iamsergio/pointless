// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include "core/data_provider.h"
#include "taskfiltermodel.h"

#include <QDate>
#include <QObject>
#include <QtQml/qqmlregistration.h>

#include <memory>

class TaskModel;
class TagModel;

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
    Q_PROPERTY(bool isDebug READ isDebug CONSTANT)
    Q_PROPERTY(bool isMobile READ isMobile CONSTANT)
    Q_PROPERTY(bool isAndroid READ isAndroid CONSTANT)
    Q_PROPERTY(bool isIOS READ isIOS CONSTANT)
    Q_PROPERTY(bool isVerbose READ isVerbose CONSTANT)
    Q_PROPERTY(bool isEditing READ isEditing WRITE setIsEditing NOTIFY isEditingChanged)
    Q_PROPERTY(QString uuidBeingEdited READ uuidBeingEdited WRITE setUuidBeingEdited NOTIFY uuidBeingEditedChanged)
public:
    [[nodiscard]] TaskFilterModel *taskFilterModel() const;
    enum class ViewType : uint8_t {
        Week,
        Soon,
        Later
    };
    Q_ENUM(ViewType)

    explicit GuiController(QObject *parent = nullptr);

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
    Q_INVOKABLE void addNewTask(const QString &title);
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

    Q_INVOKABLE void dumpTaskDebug(const QString &taskUuid) const;

Q_SIGNALS:
    void currentViewTypeChanged();
    void navigatorStartDateChanged();
    void navigatorEndDateChanged();
    void isEditingChanged();
    void uuidBeingEditedChanged();

private:
    ViewType _currentViewType = ViewType::Week;
    bool _isEditing = false;
    QString _uuidBeingEdited;
    QDate _navigatorStartDate;
    TaskModel *_taskModel = nullptr;
    TagModel *_tagModel = nullptr;
    TaskFilterModel *_taskFilterModel = nullptr;
    std::unique_ptr<IDataProvider> _dataProvider;
};
