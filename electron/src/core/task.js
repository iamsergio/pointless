// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

'use strict';

const { tagIsBuiltin, BUILTIN_TAG_SOON, BUILTIN_TAG_CURRENT, BUILTIN_TAG_EVENING } = require('./tag');

function _tsFromMillis(value) {
    if (value === null || value === undefined) return null;
    return new Date(value);
}

function _trimTime(date) {
    const d = new Date(date);
    d.setHours(0, 0, 0, 0);
    return d;
}

function _today() {
    return _trimTime(new Date());
}

function _thisWeeksMonday(date) {
    const d = _trimTime(date);
    const day = d.getDay(); // 0=Sun
    const daysBack = day === 0 ? 6 : day - 1;
    d.setDate(d.getDate() - daysBack);
    return d;
}

function _nextMonday(monday) {
    const d = new Date(monday);
    d.setDate(d.getDate() + 7);
    return d;
}

function _isThisWeek(date) {
    const monday = _thisWeeksMonday(new Date());
    return date >= monday && date < _nextMonday(monday);
}

function _isTomorrow(date) {
    const trimmed = _trimTime(date);
    const tomorrow = _today();
    tomorrow.setDate(tomorrow.getDate() + 1);
    return trimmed.getTime() === tomorrow.getTime();
}

class Task {
    constructor({
        revision = -1,
        needsSyncToServer = false,
        uuid = '',
        parentUuid = null,
        title = '',
        isDone = false,
        isGoal = null,
        isYearly = null,
        isImportant = false,
        hideOnWeekends = null,
        timesPerWeek = 1,
        lastCompletions = [],
        sectionName = '',
        tags = [],
        creationTimestamp = null,
        modificationTimestamp = null,
        lastPomodoroDate = null,
        dueDate = null,
        completionDate = null,
        uuidInDeviceCalendar = null,
        deviceCalendarUuid = null,
        deviceCalendarName = null,
        description = null,
    } = {}) {
        this.revision = revision;
        this.needsSyncToServer = needsSyncToServer;
        this.uuid = uuid;
        this.parentUuid = parentUuid;
        this.title = title;
        this.isDone = isDone;
        this.isGoal = isGoal;
        this.isYearly = isYearly;
        this.isImportant = isImportant;
        this.hideOnWeekends = hideOnWeekends;
        this.timesPerWeek = timesPerWeek;
        this.lastCompletions = lastCompletions;
        this.sectionName = sectionName;
        this.tags = tags;
        this.creationTimestamp = creationTimestamp;
        this.modificationTimestamp = modificationTimestamp;
        this.lastPomodoroDate = lastPomodoroDate;
        this.dueDate = dueDate;
        this.completionDate = completionDate;
        this.uuidInDeviceCalendar = uuidInDeviceCalendar;
        this.deviceCalendarUuid = deviceCalendarUuid;
        this.deviceCalendarName = deviceCalendarName;
        this.description = description;
    }

    containsTag(tagName) {
        return this.tags.includes(tagName);
    }

    isDueIn(days) {
        if (!this.dueDate) return false;
        const now = Date.now();
        const due = this.dueDate.getTime();
        return due <= now + days * 86400000 && due >= now;
    }

    isDueTomorrow() {
        if (!this.dueDate) return false;
        return _isTomorrow(this.dueDate);
    }

    isDueThisWeek() {
        if (!this.dueDate) return false;
        return _isThisWeek(this.dueDate);
    }

    isOverdue() {
        if (!this.dueDate || this.isDone) return false;
        return this.dueDate.getTime() < Date.now();
    }

    isCurrent() {
        return this.containsTag(BUILTIN_TAG_CURRENT) || this.isDueThisWeek() || this.isOverdue();
    }

    isSoon() {
        if (this.isCurrent()) return false;
        return this.containsTag(BUILTIN_TAG_SOON) || this.isDueIn(15);
    }

    isLater() {
        return !this.isSoon() && !this.isCurrent();
    }

    isEvening() {
        return this.containsTag(BUILTIN_TAG_EVENING);
    }

    shouldBeCleanedUp() {
        if (this.isYearly === true) return false;
        const twoWeeksAgo = Date.now() - 14 * 86400000;
        if (!this.isDone) return false;
        if (this.modificationTimestamp) {
            return this.modificationTimestamp.getTime() < twoWeeksAgo;
        }
        return true;
    }

    tagName() {
        for (const tag of this.tags) {
            if (!tagIsBuiltin(tag)) return tag;
        }
        return '';
    }

    addTag(tag) {
        if (tag && !this.containsTag(tag)) {
            this.tags.push(tag);
            return true;
        }
        return false;
    }

    removeBuiltinTags() {
        this.tags = this.tags.filter(t => !tagIsBuiltin(t));
    }

    static fromJSON(obj) {
        return new Task({
            revision: obj.revision ?? -1,
            needsSyncToServer: obj.needsSyncToServer ?? false,
            uuid: obj.uuid ?? '',
            parentUuid: obj.parentUuid ?? null,
            title: obj.title ?? '',
            isDone: obj.isDone ?? false,
            isGoal: obj.isGoal ?? null,
            isYearly: obj.isYearly ?? null,
            isImportant: obj.isImportant ?? false,
            hideOnWeekends: obj.hideOnWeekends ?? null,
            timesPerWeek: obj.timesPerWeek ?? 1,
            lastCompletions: obj.lastCompletions ?? [],
            sectionName: obj.sectionName ?? '',
            tags: obj.tags ?? [],
            creationTimestamp: _tsFromMillis(obj.creationTimestamp),
            modificationTimestamp: _tsFromMillis(obj.modificationTimestamp),
            lastPomodoroDate: _tsFromMillis(obj.lastPomodoroDate),
            dueDate: _tsFromMillis(obj.dueDate),
            completionDate: _tsFromMillis(obj.completionDate),
            uuidInDeviceCalendar: obj.uuidInDeviceCalendar ?? null,
            deviceCalendarUuid: obj.deviceCalendarUuid ?? null,
            deviceCalendarName: obj.deviceCalendarName ?? null,
            description: obj.description ?? null,
        });
    }
}

module.exports = { Task };
