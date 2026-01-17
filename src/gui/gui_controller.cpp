// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT


#include "gui_controller.h"
#include "taskfiltermodel.h"
#include "taskmodel.h"
#include "tagmodel.h"
#include "tagfiltermodel.h"
#include "date_utils.h"
#include "data_controller.h"
#include "Clock.h"

#include "gui/qt_logger.h"
#include "core/utils.h"
#include "core/task.h"
#include "core/context.h"
#include "core/data_provider.h"

#include <QTimer>
#include <QHash>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QtGlobal>
#include <QUuid>
#include <QDateTime>
#include <QKeyEvent>

#include <cstdlib>
#include <string>
#include <utility>

using namespace pointless;

namespace {
GuiController *s_instance = nullptr; // NOLINT // clazy:exclude=non-pod-global-static

class EventFilter : public QObject
{
public:
    explicit EventFilter(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

    bool eventFilter(QObject * /*obj*/, QEvent *event) override
    {
        if (event->type() == QEvent::KeyPress) {
            auto *keyEvent = static_cast<QKeyEvent *>(event); // NOLINT
            if (keyEvent->key() == Qt::Key_Escape) {
                if (GuiController::instance()->isEditing()) {
                    GuiController::instance()->setIsEditing(false);
                    return true;
                }
            }

            if (keyEvent->key() == Qt::Key_N) {
                if (!GuiController::instance()->isEditing()) {
                    GuiController::instance()->setTaskBeingEdited({}, {});
                    return true;
                }
            }
        }
        return false;
    }
};

}

GuiController::GuiController(QObject *parent)
    : QObject(parent)
    , _dataController(new DataController(this))
{
    Q_ASSERT(s_instance == nullptr);

    if (qApp) // might be running in tests without qApp
        qApp->installEventFilter(new EventFilter(this));

#ifdef POINTLESS_DEVELOPER_MODE
    // TODO: A better place to put it ?
    if (!_dataController->loginWithDefaults()) {
        pointless::abort("Failed to login with default credentials in developer mode");
    }
#else
    // TODO: A better place to put it ?
    _dataController->loginWithDefaults();
#endif

    navigatorGotoToday();

    QTimer::singleShot(0, this, &GuiController::refresh);
}

bool GuiController::isDebug()
{
#ifdef QT_DEBUG
    return true;
#else
    return false;
#endif
}

void GuiController::refresh()
{
    auto refreshResult = _dataController->refresh();
    if (!refreshResult) {
        P_LOG_ERROR("GuiController::refresh: {}", refreshResult.error());
        return;
    }
}

TaskFilterModel *GuiController::taskFilterModel() const
{
    if (_taskFilterModel == nullptr) {
        // Lazy initialization to break a circular dependency between GuiController and TaskFilterModel
        _taskFilterModel = new TaskFilterModel(const_cast<GuiController *>(this));
    }

    return _taskFilterModel;
}

TaskModel *GuiController::taskModel() const
{
    return _dataController->taskModel();
}

TagModel *GuiController::tagModel() const
{
    return _dataController->tagModel();
}

TagFilterModel *GuiController::tagFilterModel() const
{
    if (_tagFilterModel == nullptr) {
        _tagFilterModel = new TagFilterModel(const_cast<GuiController *>(this));
    }

    return _tagFilterModel;
}

GuiController::ViewType GuiController::currentViewType() const
{
    return _currentViewType;
}

QDate GuiController::navigatorStartDate() const
{
    return _navigatorStartDate;
}

QDate GuiController::navigatorEndDate() const
{
    return _navigatorStartDate.addDays(6);
}

QString GuiController::navigatorPrettyStartDate() const
{
    return _navigatorStartDate.toString(QStringLiteral("MMM d"));
}

QString GuiController::navigatorPrettyEndDate() const
{
    return navigatorEndDate().toString(QStringLiteral("MMM d"));
}

void GuiController::setNavigatorStartDate(QDate date)
{
    if (_navigatorStartDate == date) {
        return;
    }
    _navigatorStartDate = date;
    emit navigatorStartDateChanged();
    emit navigatorEndDateChanged();
}

void GuiController::navigatorGotoToday()
{
    QDate today = Gui::Clock::today();
    QDate monday = Gui::DateUtils::firstMondayOfWeek(today);
    P_LOG_INFO("Today: {}, First Monday: {}", today.toString(Qt::ISODate), monday.toString(Qt::ISODate));
    setNavigatorStartDate(Gui::DateUtils::firstMondayOfWeek(Gui::Clock::today()));
}

void GuiController::navigatorGotoNextWeek()
{
    setNavigatorStartDate(_navigatorStartDate.addDays(7));
}

void GuiController::navigatorGotoPreviousWeek()
{
    setNavigatorStartDate(_navigatorStartDate.addDays(-7));
}

void GuiController::setCurrentViewType(ViewType viewType)
{
    if (_currentViewType == viewType) {
        return;
    }

    _currentViewType = viewType;
    emit currentViewTypeChanged();
}

QDate GuiController::dateInEditor() const
{
    return _dateInEditor;
}

void GuiController::setDateInEditor(QDate date)
{
    P_LOG_DEBUG("Setting dateInEditor to {} ; isNull={}", date.toString(Qt::ISODate), date.isNull());
    if (_dateInEditor == date) {
        return;
    }
    _dateInEditor = date;
    emit dateInEditorChanged();
}

bool GuiController::isMobile()
{
    return isAndroid() || isIOS();
}

bool GuiController::isAndroid()
{
#ifdef Q_OS_ANDROID
    return true;
#else
    return false;
#endif
}

bool GuiController::isIOS()
{
#ifdef Q_OS_IOS
    return true;
#else
    return false;
#endif
}

QString GuiController::uuidBeingEdited() const
{
    return _uuidBeingEdited;
}

void GuiController::setUuidBeingEdited(const QString &uuid)
{
    if (_uuidBeingEdited == uuid) {
        return;
    }

    _uuidBeingEdited = uuid;
    emit uuidBeingEditedChanged();
}

bool GuiController::isEditing() const
{
    return _isEditing;
}

void GuiController::setIsEditing(bool isEditing)
{
    if (_isEditing == isEditing) {
        return;
    }

    _isEditing = isEditing;
    emit isEditingChanged();
}

void GuiController::addNewTask(QString title, const QString &tag, bool isEvening)
{
    P_LOG_INFO("GuiController::addNewTask: Adding new task with title '{}' and date '{}'",
               title.toStdString(), _dateInEditor.toString(Qt::ISODate).toStdString());

    auto guard = qScopeGuard([this] { clearTaskBeingEdited(); });

    QString processedTitle = std::move(title);
    QString extractedTag;

    QStringList tokens = processedTitle.split(' ', Qt::SkipEmptyParts);
    if (!tokens.isEmpty()) {
        const QString &lastToken = tokens.last();
        if (lastToken.startsWith(':') && lastToken.length() > 1) {
            extractedTag = lastToken.mid(1);
            tokens.removeLast();
            processedTitle = tokens.join(' ');
        }
    }

    if (processedTitle.isEmpty()) {
        P_LOG_ERROR("Won't add task with empty title");
        return;
    }

    pointless::core::Task task;
    task.uuid = QUuid::createUuid().toString(QUuid::WithoutBraces).toStdString();
    task.title = processedTitle.toStdString();

    if (_dateInEditor.isValid()) {
        task.dueDate = Gui::DateUtils::qdateToTimepoint(_dateInEditor);
    }

    if (!extractedTag.isEmpty()) {
        task.tags.push_back(extractedTag.toStdString());
    } else if (!tag.isEmpty()) {
        task.tags.push_back(tag.toStdString());
    }

    if (_currentViewType == ViewType::Soon) {
        task.tags.emplace_back("soon");
    } else if (_currentViewType == ViewType::Week) {
        task.tags.emplace_back("current");
    }

    if (isEvening) {
        task.tags.emplace_back("evening");
    }

    _dataController->taskModel()->addTask(task);
}

QString GuiController::colorFromTag(const QString &tagName) const
{
    // TODO: Store color per tag in the database
    static const QStringList colors = {
        "#2ECC71",
        "#3498DB",
        "#9B5934",
        "#FF8A00"
    };
    if (tagName.isEmpty())
        return QStringLiteral("#555555");
    const auto index = qHash(tagName) % colors.size();
    return colors.at(static_cast<int>(index));
}

bool GuiController::isVerbose()
{
    static bool is = std::getenv("POINTLESS_IS_VERBOSE") != nullptr;
    return is;
}

void GuiController::dumpTaskDebug(const QString &taskUuid) const
{
    const auto *task = _dataController->taskModel()->taskForUuid(taskUuid);
    if (task == nullptr) {
        P_LOG_ERROR("Invalid task UUID: {}", taskUuid);
        return;
    }
    task->dumpDebug();
}

void GuiController::dumpDebug() const
{
    core::LocalData &localData = _dataController->localData();
    const auto &data = localData.data();
    P_LOG_INFO("needsLocalSave={} ; needsUpload={} ; Data={} ; LocalData={}; revision={}; numTasks={}", data.needsLocalSave, data.needsUpload,
               static_cast<const void *>(&data), static_cast<const void *>(&localData), data.revision(), data.taskCount());
}

QString GuiController::windowTitle() const
{
    if (core::Context::self().dataProviderType() == IDataProvider::Type::TestSupabase) {
        return QStringLiteral("Pointless (test user)");
    }
    return QStringLiteral("Pointless");
}

DataController *GuiController::dataController() const
{
    return _dataController;
}

/** static */
GuiController *GuiController::create(QQmlEngine *qmlEngine, QJSEngine *jsEngine)
{
    Q_UNUSED(qmlEngine)
    Q_UNUSED(jsEngine)

    if (s_instance == nullptr) {
        pointless::abort("Initialize GuiController before using in QML");
    }

    return s_instance;
}

/** static */
GuiController *GuiController::instance()
{
    if (s_instance == nullptr) {
        s_instance = new GuiController();
    }

    return s_instance;
}

void GuiController::onBackClicked()
{
    clearTaskBeingEdited();
}

void GuiController::clearTaskBeingEdited()
{
    setIsEditing(false);
    setDateInEditor({});
    setUuidBeingEdited({});
}

void GuiController::setTaskBeingEdited(const QString &uuid, QDate date)
{
    setUuidBeingEdited(uuid);
    setDateInEditor(date);
    setIsEditing(true);
}

void GuiController::moveTaskToCurrent(const QString &taskUuid)
{
    auto *task = _dataController->taskModel()->taskForUuid(taskUuid);
    if (task == nullptr) {
        P_LOG_ERROR("Invalid task UUID: {}", taskUuid);
        return;
    }
}

void GuiController::moveTaskToSoon(const QString &taskUuid)
{
    auto *task = _dataController->taskModel()->taskForUuid(taskUuid);
    if (task == nullptr) {
        P_LOG_ERROR("Invalid task UUID: {}", taskUuid);
        return;
    }
}

void GuiController::moveTaskToLater(const QString &taskUuid)
{
    auto *task = _dataController->taskModel()->taskForUuid(taskUuid);
    if (task == nullptr) {
        P_LOG_ERROR("Invalid task UUID: {}", taskUuid);
        return;
    }
}

void GuiController::moveTaskToTomorrow(const QString &taskUuid)
{
    auto *task = _dataController->taskModel()->taskForUuid(taskUuid);
    if (task == nullptr) {
        P_LOG_ERROR("Invalid task UUID: {}", taskUuid);
        return;
    }
}

void GuiController::moveTaskToMonday(const QString &taskUuid)
{
    auto *task = _dataController->taskModel()->taskForUuid(taskUuid);
    if (task == nullptr) {
        P_LOG_ERROR("Invalid task UUID: {}", taskUuid);
        return;
    }
}

void GuiController::deleteTask(const QString &taskUuid)
{
    auto *task = _dataController->taskModel()->taskForUuid(taskUuid);
    if (task == nullptr) {
        P_LOG_ERROR("Invalid task UUID: {}", taskUuid);
        return;
    }
}
