// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

use chrono::{DateTime, Utc};

#[cfg(test)]
thread_local! {
    static TEST_NOW: std::cell::RefCell<Option<DateTime<Utc>>> = const { std::cell::RefCell::new(None) };
}

pub fn now() -> DateTime<Utc> {
    #[cfg(test)]
    {
        if let Some(t) = TEST_NOW.with(|cell| *cell.borrow()) {
            return t;
        }
    }
    Utc::now()
}

#[cfg(test)]
pub fn set_test_now(t: DateTime<Utc>) {
    TEST_NOW.with(|cell| *cell.borrow_mut() = Some(t));
}

#[cfg(test)]
pub fn reset() {
    TEST_NOW.with(|cell| *cell.borrow_mut() = None);
}

#[cfg(test)]
mod tests {
    use super::*;
    use chrono::TimeZone;

    #[test]
    fn test_now_returns_current_time() {
        reset();
        let before = Utc::now();
        let result = now();
        let after = Utc::now();
        assert!(result >= before && result <= after);
    }

    #[test]
    fn test_set_test_now() {
        let fixed = Utc.with_ymd_and_hms(2025, 6, 15, 12, 0, 0).unwrap();
        set_test_now(fixed);
        assert_eq!(now(), fixed);
        reset();
        assert_ne!(now(), fixed);
    }
}
