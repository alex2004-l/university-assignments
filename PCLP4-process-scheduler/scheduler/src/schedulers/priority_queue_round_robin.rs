use crate::scheduler::{Pid, Process};
use crate::schedulers::ProcessControlBlock;
use crate::{ProcessState, Scheduler, Syscall};
use std::collections::VecDeque;
use std::num::NonZeroUsize;

use crate::SchedulingDecision;
use crate::StopReason;
use crate::SyscallResult;

pub struct PriorityQueueRoundRobin {
    pub ready_processes: VecDeque<ProcessControlBlock>,
    pub waiting_processes: VecDeque<ProcessControlBlock>,
    pub timeslice: NonZeroUsize,
    pub minimum_remaining_timeslice: usize,
    pub remaining_timeslice: usize,
    pub next_pid: usize,
    pub sleep: bool,
    pub sleep_time: usize,
}

impl PriorityQueueRoundRobin {
    fn actualise_timings(&mut self, timeslice: usize) {
        for process in &mut self.ready_processes {
            process.total += timeslice;
        }
        for process in &mut self.waiting_processes {
            process.total += timeslice;
        }

        for process in &mut self.waiting_processes {
            if process.sleep > 0 {
                if process.sleep > timeslice {
                    process.sleep -= timeslice;
                } else {
                    process.sleep = 0;
                    process.state = ProcessState::Ready;
                }
            }
        }

        self.update_sleeping_processes();
    }

    fn update_sleeping_processes(&mut self) {
        self.waiting_processes
            .retain(|process| match process.state {
                ProcessState::Ready => {
                    self.ready_processes.push_back(process.clone());
                    false
                }
                _ => true,
            })
    }

    fn update_waiting_processes(&mut self, signal_code: usize) {
        self.waiting_processes
            .retain(|process| match process.state {
                ProcessState::Waiting { event } => match event {
                    Some(signal) => {
                        if signal == signal_code {
                            let mut copy = process.clone();
                            copy.state = ProcessState::Ready;
                            self.ready_processes.push_back(copy);
                            false
                        } else {
                            true
                        }
                    }
                    None => true,
                },
                _ => true,
            })
    }

    fn check_if_reschedule(&mut self) -> bool {
        self.remaining_timeslice >= self.minimum_remaining_timeslice
    }

    fn get_first_element(&mut self) -> usize {
        let first_element = self.ready_processes.pop_front().unwrap();
        let copy_first_element = first_element.clone();
        self.ready_processes.push_front(first_element);
        copy_first_element.pid
    }

    fn check_if_first_process_exists(&mut self) -> bool {
        if !self
            .waiting_processes
            .iter()
            .any(|process| process.pid == 1)
            && !self.ready_processes.iter().any(|process| process.pid == 1)
        {
            return false;
        }
        true
    }

    fn check_if_process_is_running(&mut self) -> bool {
        if self.ready_processes.is_empty() {
            return false;
        }
        let first_element = self.ready_processes.pop_front().unwrap();
        let copy_first_element = first_element.clone();
        self.ready_processes.push_front(first_element);
        match copy_first_element.state {
            ProcessState::Running => true,
            _ => false,
        }
    }

    fn check_if_all_processes_await_signal(&mut self) -> bool {
        if !self.ready_processes.is_empty() {
            return false;
        }
        let mut flag = true;
        for process in &mut self.waiting_processes {
            match process.state {
                ProcessState::Waiting { event } => match event {
                    Some(_) => (),
                    None => flag = false,
                },
                _ => flag = false,
            }
        }
        flag
    }

    fn check_if_all_processes_sleep(&mut self) -> (bool, usize) {
        if !self.ready_processes.is_empty() {
            return (false, 0);
        }
        let mut flag = true;
        let mut minimum_sleep_time = 10000_usize;
        for process in &mut self.waiting_processes {
            match process.state {
                ProcessState::Waiting { event } => match event {
                    Some(_) => (),
                    None => {
                        if minimum_sleep_time > process.sleep {
                            minimum_sleep_time = process.sleep;
                        }
                    }
                },
                _ => flag = false,
            }
        }
        (flag, minimum_sleep_time)
    }
}

impl Scheduler for PriorityQueueRoundRobin {
    fn next(&mut self) -> SchedulingDecision {
        if self.sleep {
            self.sleep = false;
            self.actualise_timings(self.sleep_time);
            self.sleep_time = 0;
        }
        if self.check_if_process_is_running() {
            if self.check_if_reschedule() {
                SchedulingDecision::Run {
                    pid: Pid::new(self.get_first_element()),
                    timeslice: NonZeroUsize::new(self.remaining_timeslice).unwrap(),
                }
            } else {
                let mut current_element = self.ready_processes.pop_front().unwrap();
                current_element.state = ProcessState::Ready;
                self.ready_processes.push_back(current_element);
                self.remaining_timeslice = self.timeslice.get();
                // sortarea proceselor in functie de prioritatea acestora
                self.ready_processes
                    .make_contiguous()
                    .sort_by(|a, b| b.priority.cmp(&a.priority));

                let mut first_element = self.ready_processes.pop_front().unwrap();
                first_element.state = ProcessState::Running;
                self.ready_processes.push_front(first_element);
                SchedulingDecision::Run {
                    pid: Pid::new(self.get_first_element()),
                    timeslice: NonZeroUsize::new(self.remaining_timeslice).unwrap(),
                }
            }
        } else if self.ready_processes.is_empty() {
            if self.waiting_processes.is_empty() {
                return SchedulingDecision::Done;
            }

            if !self.check_if_first_process_exists() {
                return SchedulingDecision::Panic;
            }

            if self.check_if_all_processes_await_signal() {
                return SchedulingDecision::Deadlock;
            }

            let (check_if_sleep, minimum_time) = self.check_if_all_processes_sleep();
            if check_if_sleep {
                self.sleep = true;
                self.sleep_time = minimum_time;
                return SchedulingDecision::Sleep(NonZeroUsize::new(minimum_time).unwrap());
            }

            SchedulingDecision::Panic
        } else {
            if !self.check_if_first_process_exists() {
                return SchedulingDecision::Panic;
            }

            self.remaining_timeslice = self.timeslice.get();
            // sortarea proceselor in functie de prioritatea acestora
            self.ready_processes
                .make_contiguous()
                .sort_by(|a, b| b.priority.partial_cmp(&a.priority).unwrap());
            let mut first_element = self.ready_processes.pop_front().unwrap();
            first_element.state = ProcessState::Running;
            self.ready_processes.push_front(first_element);
            SchedulingDecision::Run {
                pid: Pid::new(self.get_first_element()),
                timeslice: NonZeroUsize::new(self.remaining_timeslice).unwrap(),
            }
        }
    }

    fn stop(&mut self, reason: StopReason) -> SyscallResult {
        match reason {
            StopReason::Syscall { syscall, remaining } => match syscall {
                Syscall::Fork(priority) => {
                    let new_process = ProcessControlBlock::new(self.next_pid, priority);

                    self.next_pid += 1;

                    if self.ready_processes.is_empty() {
                        self.ready_processes.push_back(new_process.clone());
                        self.remaining_timeslice = remaining;
                    } else {
                        let mut process = self.ready_processes.pop_front().unwrap();
                        let execution_time = self.remaining_timeslice - remaining - 1;

                        process.syscall += 1;
                        process.execute += execution_time;
                        process.inc_priority();
                        self.ready_processes.push_front(process);
                        self.remaining_timeslice = remaining;
                        self.actualise_timings(execution_time + 1);
                        self.ready_processes.push_back(new_process.clone());
                    }
                    SyscallResult::Pid(Pid::new(new_process.pid))
                }
                Syscall::Sleep(sleep_time) => {
                    if self.ready_processes.is_empty() {
                        return SyscallResult::NoRunningProcess;
                    }

                    let mut current_process = self.ready_processes.pop_front().unwrap();
                    current_process.state = ProcessState::Waiting { event: None };
                    current_process.sleep = sleep_time;
                    current_process.syscall += 1;
                    current_process.inc_priority();

                    let execution_time = self.remaining_timeslice - remaining - 1;
                    current_process.execute += execution_time;
                    current_process.total += execution_time + 1;
                    self.actualise_timings(execution_time + 1);

                    self.waiting_processes.push_back(current_process);
                    SyscallResult::Success
                }
                Syscall::Signal(signal_code) => {
                    if self.ready_processes.is_empty() {
                        return SyscallResult::NoRunningProcess;
                    }

                    let mut current_process = self.ready_processes.pop_front().unwrap();
                    let execution_time = self.remaining_timeslice - remaining - 1;
                    current_process.syscall += 1;
                    current_process.execute += execution_time;
                    current_process.inc_priority();

                    self.ready_processes.push_front(current_process);
                    self.remaining_timeslice = remaining;
                    self.actualise_timings(execution_time + 1);
                    self.update_waiting_processes(signal_code);
                    SyscallResult::Success
                }
                Syscall::Wait(event_number) => {
                    if self.ready_processes.is_empty() {
                        return SyscallResult::NoRunningProcess;
                    }
                    let mut current_process = self.ready_processes.pop_front().unwrap();
                    let execution_time = self.remaining_timeslice - remaining - 1;
                    current_process.syscall += 1;
                    current_process.execute += execution_time;
                    current_process.inc_priority();
                    current_process.state = ProcessState::Waiting {
                        event: Some(event_number),
                    };
                    self.waiting_processes.push_back(current_process);
                    self.remaining_timeslice = remaining;
                    self.actualise_timings(execution_time + 1);
                    SyscallResult::Success

                }
                Syscall::Exit => {
                    if self.ready_processes.is_empty() {
                        return SyscallResult::NoRunningProcess;
                    }

                    let mut _current_process = self.ready_processes.pop_front().unwrap();
                    let execution_time = self.remaining_timeslice - remaining - 1;

                    self.remaining_timeslice = remaining;
                    self.actualise_timings(execution_time + 1);
                    SyscallResult::Success
                }
            },
            StopReason::Expired => {
                if self.ready_processes.is_empty() {
                    return SyscallResult::NoRunningProcess;
                }
                let mut current_process = self.ready_processes.pop_front().unwrap();
                current_process.execute += self.remaining_timeslice;
                current_process.state = ProcessState::Ready;
                current_process.total += self.remaining_timeslice;
                current_process.dec_priority();
                self.actualise_timings(self.remaining_timeslice);
                self.ready_processes.push_back(current_process);

                SyscallResult::Success
            }
        }
    }

    fn list(&mut self) -> Vec<&dyn Process> {
        self.ready_processes
            .iter()
            .chain(self.waiting_processes.iter())
            .map(|process| process as &dyn Process)
            .collect()
    }
}
