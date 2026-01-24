// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT


#include "gui_controller.h"
#include "taskfiltermodel.h"
#include "taskmodel.h"
#include "tagmodel.h"
#include "tagfiltermodel.h"
#include "date_utils.h"
#include "data_controller.h"
#include "local_settings.h"
#include "utils.h"
#include "Clock.h"

#include "gui/qt_logger.h"
#include "core/utils.h"
#include "core/task.h"
#include "core/context.h"
#include "core/data_provider.h"
#include "core/tag.h"

#include <QTimer>
#include <QHash>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QtGlobal>
#include <QUuid>
#include <QDateTime>
#include <QKeyEvent>
#include <QStandardPaths>
#include <QSettings>

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
            auto *gc = GuiController::instance();
            if (keyEvent->key() == Qt::Key_Escape) {
                if (gc->isEditing()) {
                    gc->setIsEditing(false);
                    return true;
                }

                if (gc->taskMenuVisible()) {
                    gc->setTaskMenuUuid({});
                    return true;
                }
            }

            if (keyEvent->key() == Qt::Key_N) {
                if (!gc->isEditing()) {
                    gc->setTaskBeingEdited({}, {});
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

    connect(_dataController, &DataController::isAuthenticatedChanged, this, &GuiController::isAuthenticatedChanged);

    if (qApp) // might be running in tests without qApp
        qApp->installEventFilter(new EventFilter(this));

    const auto savedToken = _localSettings.accessToken();
    const auto savedUserId = _localSettings.userId();

    if (!savedToken.empty() && !savedUserId.empty()) {
        _dataController->setAccessToken(savedToken);
        _dataController->setUserId(savedUserId);
        P_LOG_DEBUG("Loaded saved authentication token");
        Q_EMIT isAuthenticatedChanged();
    }

    if (Gui::isAutoLogin() && !_dataController->isAuthenticated()) {
        if (!_dataController->loginWithDefaults()) {
            pointless::abort("Failed to login with default credentials in developer mode");
        }
        Q_EMIT isAuthenticatedChanged();
    }

    navigatorGotoToday();

    if (_dataController->isAuthenticated()) {
        QTimer::singleShot(0, this, &GuiController::refresh);
    }
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
        P_LOG_ERROR("GuiController::refresh: {}", refreshResult.error().toString());
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

bool GuiController::isAuthenticated() const
{
    return _dataController->isAuthenticated();
}

QString GuiController::defaultLoginUsername() const
{
    return QString::fromStdString(_dataController->defaultLoginUsername());
}

QString GuiController::loginError() const
{
    return _loginError;
}

void GuiController::login(const QString &email, const QString &password)
{
    _loginError.clear();
    Q_EMIT loginErrorChanged();

    if (_dataController->login(email.toStdString(), password.toStdString())) {
        _localSettings.setAccessToken(_dataController->accessToken());
        _localSettings.setUserId(_dataController->userId());
        _localSettings.save();
        P_LOG_DEBUG("Login successful, saved credentials");
        Q_EMIT isAuthenticatedChanged();
        QTimer::singleShot(0, this, &GuiController::refresh);
    } else {
        _loginError = "Login failed. Please check your email and password.";
        P_LOG_WARNING("Login failed for email: {}", email.toStdString());
        Q_EMIT loginErrorChanged();
    }
}

void GuiController::logout()
{
    _dataController->logout();
    _localSettings.clear();
    _localSettings.save();
    P_LOG_DEBUG("Logged out, cleared credentials");
    Q_EMIT isAuthenticatedChanged();
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

QString GuiController::titleInEditor() const
{
    return _titleInEditor;
}

QString GuiController::tagInEditor() const
{
    return _tagInEditor;
}

bool GuiController::isEveningInEditor() const
{
    return _isEveningInEditor;
}

void GuiController::saveTask(QString title, const QString &tag, bool isEvening) // NOLINT
{
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
        P_LOG_ERROR("Won't save task with empty title");
        return;
    }

    pointless::core::Task newTask;
    pointless::core::Task *task = nullptr;

    const bool isNew = _uuidBeingEdited.isEmpty();

    if (isNew) {
        task = &newTask;
        task->uuid = QUuid::createUuid().toString(QUuid::WithoutBraces).toStdString();
        // Set default context tags for new tasks
        if (_currentViewType == ViewType::Soon) {
            task->tags.emplace_back("soon");
        } else if (_currentViewType == ViewType::Week) {
            task->tags.emplace_back("current");
        }
    } else {
        task = taskModel()->taskForUuid(_uuidBeingEdited);
        if (task == nullptr) {
            P_LOG_ERROR("GuiController::saveTask: Task not found for UUID: {}", _uuidBeingEdited.toStdString());
            return;
        }
    }

    task->title = processedTitle.toStdString();

    std::string newTag = extractedTag.isEmpty() ? tag.toStdString() : extractedTag.toStdString();

    std::vector<std::string> newTags;
    // Keep internal builtins ("soon", "current")
    // "evening" is builtin but we control it via bool arg.
    // So keep "soon", "current", ignore "evening".
    for (const auto &t : task->tags) {
        if (pointless::core::tagIsBuiltin(t)) {
            if (t != core::BUILTIN_TAG_EVENING) {
                newTags.push_back(t);
            }
        }
    }

    if (!newTag.empty()) {
        newTags.push_back(newTag);
    }
    if (isEvening) {
        newTags.emplace_back(core::BUILTIN_TAG_EVENING);
    }

    task->setTags(newTags);

    if (_dateInEditor.isValid()) {
        task->dueDate = Gui::DateUtils::qdateToTimepoint(_dateInEditor);
    } else {
        task->dueDate = std::nullopt;
    }

    if (isNew) {
        taskModel()->addTask(*task);
    } else {
        taskModel()->updateTask(*task);
    }
}

void GuiController::addNewTask(QString title, const QString &tag, bool isEvening)
{
    setUuidBeingEdited({});
    saveTask(std::move(title), tag, isEvening);
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

    P_LOG_INFO("Config location: {}", QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation).toStdString());
    QSettings s;
    P_LOG_INFO("QSettings file: {}", s.fileName().toStdString());
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
        s_instance = new GuiController(qApp);
        QJSEngine::setObjectOwnership(s_instance, QJSEngine::CppOwnership);
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

    if (uuid.isEmpty()) {
        setDateInEditor(date);
        _titleInEditor.clear();
        _tagInEditor.clear();
        _isEveningInEditor = false;
    } else {
        const auto *task = taskModel()->taskForUuid(uuid);
        if (task != nullptr) {
            _titleInEditor = QString::fromStdString(task->title);
            _tagInEditor = QString::fromStdString(task->tagName());
            _isEveningInEditor = task->containsTag("evening");

            if (task->dueDate) {
                setDateInEditor(Gui::DateUtils::timepointToQDate(task->dueDate));
            } else {
                setDateInEditor(QDate());
            }
        }
    }

    emit titleInEditorChanged();
    emit tagInEditorChanged();
    emit isEveningInEditorChanged();
    setIsEditing(true);
}

void GuiController::moveTaskToCurrent(const QString &taskUuid)
{
    auto *task = _dataController->taskModel()->taskForUuid(taskUuid);
    if (task == nullptr) {
        P_LOG_ERROR("Invalid task UUID: {}", taskUuid);
        return;
    }
    task->removeBuiltinTags();
    task->addTag(pointless::core::BUILTIN_TAG_CURRENT);
    taskModel()->updateTask(*task);
}

void GuiController::moveTaskToSoon(const QString &taskUuid)
{
    auto *task = _dataController->taskModel()->taskForUuid(taskUuid);
    if (task == nullptr) {
        P_LOG_ERROR("Invalid task UUID: {}", taskUuid);
        return;
    }
    task->removeBuiltinTags();
    task->addTag(pointless::core::BUILTIN_TAG_SOON);
    task->dueDate = std::nullopt;
    taskModel()->updateTask(*task);
}

void GuiController::moveTaskToLater(const QString &taskUuid)
{
    auto *task = _dataController->taskModel()->taskForUuid(taskUuid);
    if (task == nullptr) {
        P_LOG_ERROR("Invalid task UUID: {}", taskUuid);
        return;
    }

    task->removeBuiltinTags();
    taskModel()->updateTask(*task);
}

void GuiController::moveTaskToTomorrow(const QString &taskUuid)
{
    auto *task = _dataController->taskModel()->taskForUuid(taskUuid);
    if (task == nullptr) {
        P_LOG_ERROR("Invalid task UUID: {}", taskUuid);
        return;
    }

    if (task->isDueTomorrow())
        return;

    const QDate tomorrow = Gui::Clock::today().addDays(1);
    task->dueDate = Gui::DateUtils::qdateToTimepoint(tomorrow);
    taskModel()->updateTask(*task);
}

void GuiController::moveTaskToEvening(const QString &taskUuid)
{
    auto *task = _dataController->taskModel()->taskForUuid(taskUuid);
    if (task == nullptr) {
        P_LOG_ERROR("Invalid task UUID: {}", taskUuid);
        return;
    }

    if (task->addTag(core::BUILTIN_TAG_EVENING)) {
        taskModel()->updateTask(*task);
    }
}

void GuiController::moveTaskToNextMonday(const QString &taskUuid)
{
    auto *task = _dataController->taskModel()->taskForUuid(taskUuid);
    if (task == nullptr) {
        P_LOG_ERROR("Invalid task UUID: {}", taskUuid);
        return;
    }

    const QDate nextMonday = Gui::DateUtils::nextMonday(Gui::Clock::today());
    task->dueDate = Gui::DateUtils::qdateToTimepoint(nextMonday);
    taskModel()->updateTask(*task);
}

void GuiController::deleteTask(const QString &taskUuid)
{
    auto *task = _dataController->taskModel()->taskForUuid(taskUuid);
    if (task == nullptr) {
        P_LOG_ERROR("Invalid task UUID: {}", taskUuid);
        return;
    }
}

bool GuiController::taskMenuVisible() const
{
    return !_taskMenuUuid.isEmpty();
}

QString GuiController::taskMenuUuid() const
{
    return _taskMenuUuid;
}

void GuiController::setTaskMenuUuid(const QString &uuid) // NOLINT
{
    if (_taskMenuUuid == uuid) {
        if (!uuid.isEmpty()) {
            // toggling also clears
            setTaskMenuUuid("");
        }
        return;
    }

    _taskMenuUuid = uuid;
    Q_EMIT taskMenuUuidChanged();
}

bool GuiController::moveToCurrentVisible() const
{
    const auto *task = _dataController->taskModel()->taskForUuid(_taskMenuUuid);
    return task != nullptr && !task->isCurrent();
}

bool GuiController::moveToSoonVisible() const
{
    const auto *task = _dataController->taskModel()->taskForUuid(_taskMenuUuid);
    return task != nullptr && !task->isSoon();
}

bool GuiController::moveToLaterVisible() const
{
    const auto *task = _dataController->taskModel()->taskForUuid(_taskMenuUuid);
    return task != nullptr && !task->isLater();
}

bool GuiController::moveToTomorrowVisible() const
{
    const auto *task = _dataController->taskModel()->taskForUuid(_taskMenuUuid);
    return task != nullptr && !task->isDueTomorrow();
}

bool GuiController::moveToEveningVisible() const
{
    const auto *task = _dataController->taskModel()->taskForUuid(_taskMenuUuid);
    return task != nullptr && !task->isEvening() && task->isCurrent();
}
