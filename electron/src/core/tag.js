// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

'use strict';

const BUILTIN_TAG_SOON = 'soon';
const BUILTIN_TAG_CURRENT = 'current';
const BUILTIN_TAG_EVENING = 'evening';

const BUILTIN_TAGS = new Set([BUILTIN_TAG_SOON, BUILTIN_TAG_CURRENT, BUILTIN_TAG_EVENING]);

function tagIsBuiltin(name) {
    return BUILTIN_TAGS.has(name);
}

class Tag {
    constructor({ revision = -1, needsSyncToServer = false, name = '' } = {}) {
        this.revision = revision;
        this.needsSyncToServer = needsSyncToServer;
        this.name = name;
    }

    isBuiltin() {
        return tagIsBuiltin(this.name);
    }

    static fromJSON(obj) {
        return new Tag({
            revision: obj.revision ?? -1,
            needsSyncToServer: obj.needsSyncToServer ?? false,
            name: obj.name ?? '',
        });
    }
}

module.exports = { Tag, tagIsBuiltin, BUILTIN_TAG_SOON, BUILTIN_TAG_CURRENT, BUILTIN_TAG_EVENING };
