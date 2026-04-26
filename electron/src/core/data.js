// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

'use strict';

const { Task } = require('./task');
const { Tag } = require('./tag');

class Data {
    constructor() {
        this._revision = -1;
        this._tasks = [];
        this._tags = [];
        this._deletedTaskUuids = [];
        this._deletedTagNames = [];
    }

    static fromJSON(obj) {
        const d = new Data();
        d._revision = obj.revision ?? -1;
        d._tasks = (obj.tasks ?? []).map(t => Task.fromJSON(t));
        d._tags = (obj.tags ?? []).map(t => Tag.fromJSON(t));
        d._deletedTaskUuids = obj.deletedTaskUuids ?? [];
        d._deletedTagNames = obj.deletedTagNames ?? [];
        return d;
    }

    static fromJSONString(jsonStr) {
        return Data.fromJSON(JSON.parse(jsonStr));
    }

    revision() { return this._revision; }
    setRevision(r) { this._revision = r; }

    isEmpty() {
        return this._tasks.length === 0 && this._tags.length === 0;
    }

    // Task management

    taskCount() { return this._tasks.length; }

    taskAt(index) { return this._tasks[index]; }

    taskForUuid(uuid) {
        return this._tasks.find(t => t.uuid === uuid) ?? null;
    }

    taskForTitle(title) {
        return this._tasks.find(t => t.title === title) ?? null;
    }

    getAllTasks() { return this._tasks.slice(); }

    addTask(task) {
        this._tasks.push(task);
    }

    removeTask(uuid) {
        const idx = this._tasks.findIndex(t => t.uuid === uuid);
        if (idx === -1) return false;
        this._tasks.splice(idx, 1);
        return true;
    }

    updateTask(task, incrementRevision = false) {
        const idx = this._tasks.findIndex(t => t.uuid === task.uuid);
        if (idx === -1) return false;
        if (incrementRevision) task.revision++;
        this._tasks[idx] = task;
        return true;
    }

    clearTasks() { this._tasks = []; }

    getTasksByTag(tagName) {
        return this._tasks.filter(t => t.containsTag(tagName));
    }

    getPendingTasks() {
        return this._tasks.filter(t => !t.isDone);
    }

    getCompletedTasks() {
        return this._tasks.filter(t => t.isDone);
    }

    getImportantTasks() {
        return this._tasks.filter(t => t.isImportant);
    }

    getTasksByParent(parentUuid) {
        return this._tasks.filter(t => t.parentUuid === parentUuid);
    }

    // Tag management

    tagCount() { return this._tags.length; }

    tagAt(index) { return this._tags[index]; }

    allTags() { return this._tags.slice(); }

    containsTag(tagName) {
        return this._tags.some(t => t.name === tagName);
    }

    getTag(tagName) {
        return this._tags.find(t => t.name === tagName) ?? null;
    }

    addTag(tag) {
        this._tags.push(tag);
    }

    removeTag(tagName) {
        const idx = this._tags.findIndex(t => t.name === tagName);
        if (idx === -1) return false;
        this._tags.splice(idx, 1);
        return true;
    }

    clearTags() { this._tags = []; }

    getUsedTags() {
        const usedNames = new Set(this._tasks.flatMap(t => t.tags));
        return this._tags.filter(t => usedNames.has(t.name));
    }

    getUnusedTags() {
        const usedNames = new Set(this._tasks.flatMap(t => t.tags));
        return this._tags.filter(t => !usedNames.has(t.name));
    }

    // Deleted items

    deletedTaskUuids() { return this._deletedTaskUuids; }

    addDeletedTaskUuid(uuid) { this._deletedTaskUuids.push(uuid); }

    deletedTagNames() { return this._deletedTagNames; }

    addDeletedTagName(name) { this._deletedTagNames.push(name); }
}

module.exports = { Data };
