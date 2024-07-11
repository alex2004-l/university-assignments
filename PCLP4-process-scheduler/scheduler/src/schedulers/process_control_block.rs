use crate::Process;
use crate::{Pid, ProcessState};

#[derive(Debug, Clone)]
pub struct ProcessControlBlock {
    pub pid: usize,
    pub priority: i8,
    pub actual_priority: i8,
    pub total: usize,
    pub syscall: usize,
    pub execute: usize,
    pub sleep: usize,
    pub state: ProcessState,
    pub vruntime: String,
    pub extra: usize,
}

// impl PartialEq for ProcessControlBlock {
//     fn eq(&self, other: &Self) -> bool {
//         if self.priority == other.priority {
//             true
//         } else {
//             false
//         }
//     }
//     fn ne(&self, other: &Self) -> bool {
//         if self.priority == other.priority {
//             false
//         } else {
//             true
//         }
//     }
// }

// impl PartialOrd for ProcessControlBlock {
//     fn partial_cmp(&self, other: &Self) -> Option<std::cmp::Ordering> {
//         self.priority.partial_cmp(&other.priority)
//     }
// }

impl Process for ProcessControlBlock {
    fn pid(&self) -> Pid {
        Pid::new(self.pid)
    }
    fn priority(&self) -> i8 {
        self.priority
    }
    fn timings(&self) -> (usize, usize, usize) {
        (self.total, self.syscall, self.execute)
    }
    fn state(&self) -> ProcessState {
        self.state
    }
    fn extra(&self) -> String {
        self.clone().vruntime
    }
}

impl ProcessControlBlock {
    pub fn dec_priority(&mut self) {
        if self.priority != 0 {
            self.priority -= 1;
        }
    }

    pub fn inc_priority(&mut self) {
        if self.actual_priority > self.priority {
            self.priority += 1;
        }
    }
}

impl ProcessControlBlock {
    pub fn new(pid: usize, priority: i8) -> ProcessControlBlock {
        ProcessControlBlock {
            pid,
            priority,
            actual_priority: priority,
            total: 0,
            syscall: 0,
            execute: 0,
            sleep: 0,
            state: ProcessState::Ready,
            extra: 0,
            vruntime: String::from(""),
        }
    }
}
