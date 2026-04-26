<!-- SPDX-FileCopyrightText: 2025 Sergio Martins -->
<!-- SPDX-License-Identifier: MIT -->

<script>
  let { tasks = [], tags = [] } = $props();

  let activeTab = $state('week');
  let toggleOn = $state(false);

  const DAY_NAMES = ['SUNDAY', 'MONDAY', 'TUESDAY', 'WEDNESDAY', 'THURSDAY', 'FRIDAY', 'SATURDAY'];
  const MONTH_NAMES = ['Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec'];

  const BUILTIN_TAGS = new Set(['soon', 'current', 'evening']);

  function tagIsBuiltin(name) { return BUILTIN_TAGS.has(name); }

  function pendingTasks() { return tasks.filter(t => !t.isDone); }

  function trimTime(d) {
    const r = new Date(d); r.setHours(0, 0, 0, 0); return r;
  }

  function getWeekStart(date) {
    const d = new Date(date);
    const day = d.getDay();
    const daysFromMonday = day === 0 ? 6 : day - 1;
    d.setDate(d.getDate() - daysFromMonday);
    d.setHours(0, 0, 0, 0);
    return d;
  }

  function thisWeeksMonday() {
    return getWeekStart(new Date());
  }

  function nextMonday(monday) {
    const d = new Date(monday); d.setDate(d.getDate() + 7); return d;
  }

  function isThisWeek(millis) {
    if (!millis) return false;
    const monday = thisWeeksMonday();
    return millis >= monday.getTime() && millis < nextMonday(monday).getTime();
  }

  function isOverdue(task) {
    if (!task.dueDate || task.isDone) return false;
    return task.dueDate < Date.now();
  }

  function isCurrent(task) {
    return (task.tags ?? []).includes('current') || isThisWeek(task.dueDate) || isOverdue(task);
  }

  function isSoon(task) {
    if (isCurrent(task)) return false;
    if ((task.tags ?? []).includes('soon')) return true;
    if (!task.dueDate) return false;
    return task.dueDate <= Date.now() + 15 * 86400000 && task.dueDate >= Date.now();
  }

  function isLater(task) { return !isSoon(task) && !isCurrent(task); }

  function taskNonBuiltinTag(task) {
    return (task.tags ?? []).find(t => !tagIsBuiltin(t)) ?? '';
  }

  function isSameDay(millis, date) {
    const a = trimTime(new Date(millis));
    const b = trimTime(date);
    return a.getTime() === b.getTime();
  }

  let weekOffset = $state(0);

  let weekStart = $derived.by(() => {
    const base = getWeekStart(new Date());
    base.setDate(base.getDate() + weekOffset * 7);
    return base;
  });

  let weekEnd = $derived.by(() => {
    const end = new Date(weekStart);
    end.setDate(end.getDate() + 6);
    return end;
  });

  let weekRangeLabel = $derived.by(() => {
    const s = weekStart;
    const e = weekEnd;
    return `${MONTH_NAMES[s.getMonth()]} ${s.getDate()} - ${MONTH_NAMES[e.getMonth()]} ${e.getDate()}`;
  });

  let weekDays = $derived.by(() => {
    const days = [];
    for (let i = 0; i < 7; i++) {
      const d = new Date(weekStart);
      d.setDate(weekStart.getDate() + i);
      days.push(d);
    }
    return days;
  });

  function tasksForDay(day) {
    return pendingTasks().filter(t => t.dueDate && isSameDay(t.dueDate, day));
  }

  let soonTasks = $derived(pendingTasks().filter(t => isSoon(t)));
  let laterTasks = $derived(pendingTasks().filter(t => isLater(t)));

  function formatDayHeader(date) {
    return `${DAY_NAMES[date.getDay()]}, ${date.getDate()}`;
  }
</script>

<div id="main-screen" class="main-screen">
  <div class="tab-bar" id="tab-bar">
    <div class="tabs">
      <button
        class="tab {activeTab === 'week' ? 'active' : ''}"
        onclick={() => activeTab = 'week'}
      >Week</button>
      <button
        class="tab {activeTab === 'soon' ? 'active' : ''}"
        onclick={() => activeTab = 'soon'}
      >Soon</button>
      <button
        class="tab {activeTab === 'later' ? 'active' : ''}"
        onclick={() => activeTab = 'later'}
      >Later</button>
    </div>
    <button class="goals-btn" aria-label="Goals">
      <i class="fa-solid fa-flag-checkered"></i>
    </button>
  </div>

  <div class="week-nav" id="week-nav">
    <button class="nav-arrow" onclick={() => weekOffset -= 1} aria-label="Previous week">
      <i class="fa-solid fa-chevron-left"></i>
    </button>
    <span class="week-range">{weekRangeLabel}</span>
    <button class="nav-arrow" onclick={() => weekOffset += 1} aria-label="Next week">
      <i class="fa-solid fa-chevron-right"></i>
    </button>
  </div>

  <hr class="separator">

  {#if activeTab === 'week'}
    <div class="day-sections">
      {#each weekDays as day}
        {@const dayTasks = tasksForDay(day)}
        <div class="day-section">
          <div class="day-header">
            <span class="day-label">{formatDayHeader(day)}</span>
            <button class="add-day-btn" aria-label="Add task">
              <i class="fa-solid fa-plus"></i>
            </button>
          </div>
          {#each dayTasks as task (task.uuid)}
            <div class="task-row">
              <span class="task-title">{task.title}</span>
              {#if taskNonBuiltinTag(task)}
                <span class="task-tag">{taskNonBuiltinTag(task)}</span>
              {/if}
            </div>
          {/each}
        </div>
      {/each}
    </div>
  {:else if activeTab === 'soon'}
    <div class="task-list">
      {#each soonTasks as task (task.uuid)}
        <div class="task-row">
          <span class="task-title">{task.title}</span>
          {#if taskNonBuiltinTag(task)}
            <span class="task-tag">{taskNonBuiltinTag(task)}</span>
          {/if}
        </div>
      {/each}
      {#if soonTasks.length === 0}
        <p class="empty-msg">No upcoming tasks</p>
      {/if}
    </div>
  {:else}
    <div class="task-list">
      {#each laterTasks as task (task.uuid)}
        <div class="task-row">
          <span class="task-title">{task.title}</span>
          {#if taskNonBuiltinTag(task)}
            <span class="task-tag">{taskNonBuiltinTag(task)}</span>
          {/if}
        </div>
      {/each}
      {#if laterTasks.length === 0}
        <p class="empty-msg">No tasks for later</p>
      {/if}
    </div>
  {/if}

  <div class="bottom-bar" id="bottom-bar">
    <div class="bottom-left">
      <button class="bar-icon-btn" aria-label="Menu">
        <i class="fa-solid fa-bars"></i>
      </button>
      <button
        class="toggle-switch {toggleOn ? 'on' : 'off'}"
        role="switch"
        aria-checked={toggleOn}
        aria-label="Toggle"
        onclick={() => toggleOn = !toggleOn}
      >
        <span class="toggle-knob"></span>
      </button>
    </div>
    <div class="bottom-right">
      <button class="bar-icon-btn" aria-label="Sync">
        <i class="fa-solid fa-arrows-rotate"></i>
      </button>
      <button class="add-task-btn" aria-label="Add task">
        <i class="fa-solid fa-plus"></i>
      </button>
    </div>
  </div>
</div>

<style>
  .main-screen {
    flex: 1;
    display: flex;
    flex-direction: column;
    background: var(--bg-main);
    color: var(--text-white);
    overflow: hidden;
    position: relative;
  }

  /* Tab bar */
  .tab-bar {
    display: flex;
    align-items: center;
    justify-content: space-between;
    padding: 14px 16px 10px;
    flex-shrink: 0;
  }

  .tabs {
    display: flex;
    gap: 10px;
  }

  .tab {
    padding: 10px 26px;
    border: none;
    border-radius: 999px;
    font-size: 18px;
    font-weight: 600;
    cursor: pointer;
    background: var(--btn-inactive);
    color: var(--text-white);
    transition: background 0.15s;
  }

  .tab.active {
    background: var(--btn-active);
  }

  .tab:not(.active):hover {
    background: #525355;
  }

  .goals-btn {
    width: 46px;
    height: 46px;
    background: var(--btn-inactive);
    border: none;
    border-radius: 8px;
    color: var(--text-secondary);
    font-size: 20px;
    cursor: pointer;
    display: flex;
    align-items: center;
    justify-content: center;
    transition: color 0.15s;
  }

  .goals-btn:hover {
    color: var(--text-white);
  }

  /* Week navigator */
  .week-nav {
    display: flex;
    align-items: center;
    justify-content: center;
    gap: 24px;
    padding: 8px 16px 12px;
    flex-shrink: 0;
  }

  .nav-arrow {
    background: none;
    border: none;
    color: var(--text-secondary);
    font-size: 18px;
    cursor: pointer;
    padding: 4px 10px;
    transition: color 0.15s;
  }

  .nav-arrow:hover {
    color: var(--text-white);
  }

  .week-range {
    font-size: 19px;
    color: var(--text-secondary);
    min-width: 180px;
    text-align: center;
  }

  /* Separator */
  .separator {
    border: none;
    border-top: 1px solid #444548;
    margin: 0;
    flex-shrink: 0;
  }

  /* Day sections */
  .day-sections {
    flex: 1;
    overflow-y: auto;
    padding-bottom: 80px;
  }

  .day-section {
    padding: 0 16px;
  }

  .task-list {
    flex: 1;
    overflow-y: auto;
    padding: 8px 16px 80px;
  }

  .task-row {
    display: flex;
    align-items: center;
    justify-content: space-between;
    padding: 10px 0;
    border-bottom: 1px solid #3a3b3d;
  }

  .task-title {
    font-size: 15px;
    color: var(--text-white);
  }

  .task-tag {
    font-size: 12px;
    color: var(--text-secondary);
    background: #3a3b3d;
    border-radius: 4px;
    padding: 2px 7px;
    flex-shrink: 0;
  }

  .empty-msg {
    color: var(--text-secondary);
    font-size: 15px;
    text-align: center;
    padding: 32px 0;
  }

  .day-header {
    display: flex;
    align-items: center;
    justify-content: space-between;
    padding: 16px 0 10px;
  }

  .day-label {
    font-size: 17px;
    font-weight: 700;
    color: var(--text-white);
    letter-spacing: 0.03em;
  }

  .add-day-btn {
    background: none;
    border: none;
    color: var(--text-secondary);
    font-size: 18px;
    cursor: pointer;
    padding: 4px 8px;
    transition: color 0.15s;
  }

  .add-day-btn:hover {
    color: var(--text-white);
  }

  /* Bottom bar */
  .bottom-bar {
    position: absolute;
    bottom: 0;
    left: 0;
    right: 0;
    display: flex;
    align-items: center;
    justify-content: space-between;
    padding: 14px 20px;
    background: var(--bg-main);
  }

  .bottom-left,
  .bottom-right {
    display: flex;
    align-items: center;
    gap: 18px;
  }

  .bar-icon-btn {
    background: none;
    border: none;
    color: var(--accent);
    font-size: 26px;
    cursor: pointer;
    padding: 4px;
    display: flex;
    align-items: center;
    justify-content: center;
    transition: opacity 0.15s;
  }

  .bar-icon-btn:hover {
    opacity: 0.8;
  }

  /* Toggle switch */
  .toggle-switch {
    width: 52px;
    height: 30px;
    border-radius: 999px;
    border: none;
    cursor: pointer;
    position: relative;
    transition: background 0.2s;
    padding: 0;
    display: flex;
    align-items: center;
  }

  .toggle-switch.off {
    background: #555658;
  }

  .toggle-switch.on {
    background: var(--accent);
  }

  .toggle-knob {
    position: absolute;
    width: 24px;
    height: 24px;
    border-radius: 50%;
    background: #ffffff;
    top: 3px;
    transition: left 0.2s;
  }

  .toggle-switch.off .toggle-knob {
    left: 3px;
  }

  .toggle-switch.on .toggle-knob {
    left: 25px;
  }

  /* Add task circle button */
  .add-task-btn {
    width: 48px;
    height: 48px;
    border-radius: 50%;
    border: 3px solid var(--accent);
    background: none;
    color: var(--accent);
    font-size: 22px;
    cursor: pointer;
    display: flex;
    align-items: center;
    justify-content: center;
    transition: background 0.15s;
  }

  .add-task-btn:hover {
    background: rgba(14, 165, 233, 0.15);
  }
</style>
