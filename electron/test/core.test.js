// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

'use strict';

const { test, describe } = require('node:test');
const assert = require('node:assert/strict');
const { Tag, tagIsBuiltin } = require('../src/core/tag');
const { Task } = require('../src/core/task');
const { Data } = require('../src/core/data');

const TEST_JSON = JSON.stringify({
    revision: 3,
    tags: [
        { revision: 2, name: 'kde' },
        { revision: 2, name: 'kdab' },
        { revision: 2, name: 'soon' },
    ],
    tasks: [
        {
            revision: 2,
            uuid: '1',
            parentUuid: null,
            title: 'Task 1',
            isDone: false,
            isImportant: false,
            hideOnWeekends: false,
            tags: ['kde', 'kdab'],
            creationTimestamp: 1586385046574,
            modificationTimestamp: null,
            lastPomodoroDate: null,
            dueDate: null,
            completionDate: null,
            uuidInDeviceCalendar: null,
            deviceCalendarUuid: null,
            deviceCalendarName: null,
        },
        {
            revision: 2,
            uuid: '2',
            parentUuid: null,
            title: 'Task 2',
            isDone: true,
            isImportant: true,
            hideOnWeekends: false,
            tags: [],
            creationTimestamp: 1586385046574,
            modificationTimestamp: 1586385046574,
            lastPomodoroDate: null,
            dueDate: null,
            completionDate: null,
            uuidInDeviceCalendar: null,
            deviceCalendarUuid: null,
            deviceCalendarName: null,
        },
        {
            revision: 2,
            uuid: '3',
            parentUuid: '1',
            title: 'Task 3',
            isDone: false,
            isImportant: false,
            hideOnWeekends: null,
            isGoal: null,
            tags: ['soon'],
            creationTimestamp: 1586385046574,
            modificationTimestamp: null,
            lastPomodoroDate: null,
            dueDate: null,
            completionDate: null,
            uuidInDeviceCalendar: null,
            deviceCalendarUuid: null,
            deviceCalendarName: null,
        },
    ],
    deletedTaskUuids: ['deleted-uuid-1'],
    deletedTagNames: ['old-tag'],
});

describe('Tag', () => {
    test('fromJSON parses fields', () => {
        const tag = Tag.fromJSON({ revision: 5, name: 'kde' });
        assert.equal(tag.revision, 5);
        assert.equal(tag.name, 'kde');
        assert.equal(tag.needsSyncToServer, false);
    });

    test('isBuiltin returns true for builtin tags', () => {
        assert.ok(Tag.fromJSON({ name: 'soon' }).isBuiltin());
        assert.ok(Tag.fromJSON({ name: 'current' }).isBuiltin());
        assert.ok(Tag.fromJSON({ name: 'evening' }).isBuiltin());
    });

    test('isBuiltin returns false for user tags', () => {
        assert.ok(!Tag.fromJSON({ name: 'kde' }).isBuiltin());
    });

    test('tagIsBuiltin helper works', () => {
        assert.ok(tagIsBuiltin('soon'));
        assert.ok(!tagIsBuiltin('work'));
    });
});

describe('Task', () => {
    test('fromJSON parses scalar fields', () => {
        const raw = {
            revision: 7,
            uuid: 'abc',
            title: 'Hello',
            isDone: true,
            isImportant: true,
            tags: ['kde'],
            creationTimestamp: 1586385046574,
            modificationTimestamp: null,
            dueDate: null,
        };
        const task = Task.fromJSON(raw);
        assert.equal(task.uuid, 'abc');
        assert.equal(task.title, 'Hello');
        assert.equal(task.isDone, true);
        assert.equal(task.isImportant, true);
        assert.deepEqual(task.tags, ['kde']);
        assert.ok(task.creationTimestamp instanceof Date);
        assert.equal(task.creationTimestamp.getTime(), 1586385046574);
        assert.equal(task.modificationTimestamp, null);
        assert.equal(task.dueDate, null);
    });

    test('containsTag works', () => {
        const task = Task.fromJSON({ tags: ['kde', 'soon'], creationTimestamp: 0 });
        assert.ok(task.containsTag('kde'));
        assert.ok(!task.containsTag('missing'));
    });

    test('addTag adds unique tags only', () => {
        const task = new Task({ tags: ['kde'] });
        assert.ok(task.addTag('kdab'));
        assert.ok(!task.addTag('kde'));
        assert.deepEqual(task.tags, ['kde', 'kdab']);
    });

    test('removeBuiltinTags removes builtin tags', () => {
        const task = new Task({ tags: ['soon', 'current', 'kde'] });
        task.removeBuiltinTags();
        assert.deepEqual(task.tags, ['kde']);
    });

    test('tagName returns first non-builtin tag', () => {
        const task = new Task({ tags: ['soon', 'current', 'kde'] });
        assert.equal(task.tagName(), 'kde');
    });

    test('isCurrent via current tag', () => {
        const task = new Task({ tags: ['current'] });
        assert.ok(task.isCurrent());
    });

    test('isSoon via soon tag (not current)', () => {
        const task = new Task({ tags: ['soon'] });
        assert.ok(task.isSoon());
        assert.ok(!task.isCurrent());
    });

    test('isLater when no special tags and no due date', () => {
        const task = new Task({ tags: ['kde'] });
        assert.ok(task.isLater());
        assert.ok(!task.isSoon());
        assert.ok(!task.isCurrent());
    });

    test('isEvening via evening tag', () => {
        const task = new Task({ tags: ['evening'] });
        assert.ok(task.isEvening());
    });

    test('isOverdue returns false when no dueDate', () => {
        const task = new Task();
        assert.ok(!task.isOverdue());
    });

    test('isOverdue returns true for past due date on incomplete task', () => {
        const task = new Task({ dueDate: new Date(Date.now() - 86400000 * 2) });
        assert.ok(task.isOverdue());
    });

    test('isOverdue returns false for done task', () => {
        const task = new Task({ isDone: true, dueDate: new Date(Date.now() - 86400000 * 2) });
        assert.ok(!task.isOverdue());
    });

    test('shouldBeCleanedUp returns false for active task', () => {
        const task = new Task({ isDone: false });
        assert.ok(!task.shouldBeCleanedUp());
    });

    test('shouldBeCleanedUp returns true for old done task', () => {
        const task = new Task({
            isDone: true,
            modificationTimestamp: new Date(Date.now() - 86400000 * 20),
        });
        assert.ok(task.shouldBeCleanedUp());
    });

    test('shouldBeCleanedUp returns false for yearly task', () => {
        const task = new Task({
            isDone: true,
            isYearly: true,
            modificationTimestamp: new Date(Date.now() - 86400000 * 20),
        });
        assert.ok(!task.shouldBeCleanedUp());
    });

    test('null optional fields from JSON', () => {
        const task = Task.fromJSON({ isGoal: null, hideOnWeekends: null, creationTimestamp: 0 });
        assert.equal(task.isGoal, null);
        assert.equal(task.hideOnWeekends, null);
    });
});

describe('Data.fromJSONString', () => {
    test('parses revision', () => {
        const data = Data.fromJSONString(TEST_JSON);
        assert.equal(data.revision(), 3);
    });

    test('parses tasks', () => {
        const data = Data.fromJSONString(TEST_JSON);
        assert.equal(data.taskCount(), 3);
    });

    test('parses tags', () => {
        const data = Data.fromJSONString(TEST_JSON);
        assert.equal(data.tagCount(), 3);
    });

    test('parses deletedTaskUuids', () => {
        const data = Data.fromJSONString(TEST_JSON);
        assert.deepEqual(data.deletedTaskUuids(), ['deleted-uuid-1']);
    });

    test('parses deletedTagNames', () => {
        const data = Data.fromJSONString(TEST_JSON);
        assert.deepEqual(data.deletedTagNames(), ['old-tag']);
    });

    test('taskForUuid finds task', () => {
        const data = Data.fromJSONString(TEST_JSON);
        const task = data.taskForUuid('1');
        assert.ok(task);
        assert.equal(task.title, 'Task 1');
    });

    test('taskForUuid returns null for unknown uuid', () => {
        const data = Data.fromJSONString(TEST_JSON);
        assert.equal(data.taskForUuid('nope'), null);
    });

    test('taskForTitle finds task', () => {
        const data = Data.fromJSONString(TEST_JSON);
        const task = data.taskForTitle('Task 2');
        assert.ok(task);
        assert.equal(task.uuid, '2');
    });

    test('getPendingTasks returns non-done tasks', () => {
        const data = Data.fromJSONString(TEST_JSON);
        const pending = data.getPendingTasks();
        assert.equal(pending.length, 2);
        assert.ok(pending.every(t => !t.isDone));
    });

    test('getCompletedTasks returns done tasks', () => {
        const data = Data.fromJSONString(TEST_JSON);
        const done = data.getCompletedTasks();
        assert.equal(done.length, 1);
        assert.equal(done[0].uuid, '2');
    });

    test('getImportantTasks returns important tasks', () => {
        const data = Data.fromJSONString(TEST_JSON);
        const important = data.getImportantTasks();
        assert.equal(important.length, 1);
        assert.equal(important[0].uuid, '2');
    });

    test('getTasksByTag filters correctly', () => {
        const data = Data.fromJSONString(TEST_JSON);
        const kdeTasks = data.getTasksByTag('kde');
        assert.equal(kdeTasks.length, 1);
        assert.equal(kdeTasks[0].uuid, '1');
    });

    test('getTasksByParent filters by parentUuid', () => {
        const data = Data.fromJSONString(TEST_JSON);
        const children = data.getTasksByParent('1');
        assert.equal(children.length, 1);
        assert.equal(children[0].uuid, '3');
    });

    test('containsTag returns true for existing tag', () => {
        const data = Data.fromJSONString(TEST_JSON);
        assert.ok(data.containsTag('kde'));
        assert.ok(!data.containsTag('nonexistent'));
    });

    test('getTag returns tag by name', () => {
        const data = Data.fromJSONString(TEST_JSON);
        const tag = data.getTag('kde');
        assert.ok(tag);
        assert.equal(tag.name, 'kde');
    });

    test('getUsedTags returns tags used by tasks', () => {
        const data = Data.fromJSONString(TEST_JSON);
        const used = data.getUsedTags();
        const names = used.map(t => t.name);
        assert.ok(names.includes('kde'));
        assert.ok(names.includes('kdab'));
        assert.ok(names.includes('soon'));
    });

    test('getUnusedTags returns no tags when all are used', () => {
        const data = Data.fromJSONString(TEST_JSON);
        const unused = data.getUnusedTags();
        assert.equal(unused.length, 0);
    });

    test('removeTask removes by uuid', () => {
        const data = Data.fromJSONString(TEST_JSON);
        assert.ok(data.removeTask('2'));
        assert.equal(data.taskCount(), 2);
        assert.equal(data.taskForUuid('2'), null);
    });

    test('removeTask returns false for unknown uuid', () => {
        const data = Data.fromJSONString(TEST_JSON);
        assert.ok(!data.removeTask('nope'));
    });

    test('addTask increases count', () => {
        const data = Data.fromJSONString(TEST_JSON);
        const task = new Task({ uuid: 'new', title: 'New Task', creationTimestamp: new Date() });
        data.addTask(task);
        assert.equal(data.taskCount(), 4);
    });

    test('isEmpty returns false for loaded data', () => {
        const data = Data.fromJSONString(TEST_JSON);
        assert.ok(!data.isEmpty());
    });

    test('isEmpty returns true for empty data', () => {
        const data = new Data();
        assert.ok(data.isEmpty());
    });
});
