use crate::scheduler::{Pid, Process};
use crate::schedulers::ProcessControlBlock;
use crate::{ProcessState, Scheduler, Syscall};
use std::collections::VecDeque;
use std::num::NonZeroUsize;

use crate::SchedulingDecision;
use crate::StopReason;
use crate::SyscallResult;

pub struct RoundRobin {
    pub ready_processes: VecDeque<ProcessControlBlock>,
    pub waiting_processes: VecDeque<ProcessControlBlock>,
    pub timeslice: NonZeroUsize,
    pub minimum_remaining_timeslice: usize,
    pub remaining_timeslice: usize,
    pub next_pid: usize,
    pub sleep: bool,
    pub sleep_time: usize,
}

impl RoundRobin {
    // functie care actualizeaza timpul total pentru toate procesele din Scheduler
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

    // functie care muta din coada de waiting procesele care si-au incheiat perioada de sleep
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

    // functie care muta din coada de waiting procesele care asteptau semnalul primit ca paramentru
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

    // functie care verifica daca un proces se replanifica in continuare
    fn check_if_reschedule(&mut self) -> bool {
        self.remaining_timeslice >= self.minimum_remaining_timeslice
    }

    // functie care returneaza pid-ul primului proces din coada de ready
    fn get_running_process_pid(&mut self) -> usize {
        let first_element = self.ready_processes.pop_front().unwrap();
        let copy_first_element = first_element.clone();
        self.ready_processes.push_front(first_element);
        copy_first_element.pid
    }

    // functie care veridica daca procesul cu pid-ul 1 exista
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

    // functie care verifica daca primul proces din coada este in starea de running
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

    // functie care verifica daca s-a ajuns la starea de Deadlock
    // verifica daca toate procesele asteapta un semnal
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

    // functie care verifica daca toate procesele se afla in starea de sleep
    // in caz afirmativ, returneaza si timpul minim pentru care procesorul trebuie sa intre in starea de sleep
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

impl Scheduler for RoundRobin {
    fn next(&mut self) -> SchedulingDecision {
        // se verifica daca iteratia trecuta procesul a intrat in starea de sleep
        if self.sleep {
            self.sleep = false;
            self.actualise_timings(self.sleep_time);
            self.sleep_time = 0;
        }

        // se verifica daca exista proces in stare de running
        if self.check_if_process_is_running() {
            // se verifica daca se replanifica procesul
            if self.check_if_reschedule() {
                SchedulingDecision::Run {
                    pid: Pid::new(self.get_running_process_pid()),
                    timeslice: NonZeroUsize::new(self.remaining_timeslice).unwrap(),
                }
            } else {
                let mut current_element = self.ready_processes.pop_front().unwrap();
                current_element.state = ProcessState::Ready;
                self.ready_processes.push_back(current_element);
                self.remaining_timeslice = self.timeslice.get();

                // se alege primul proces din coada de ready si il ruleaza pe acesta
                let mut first_element = self.ready_processes.pop_front().unwrap();
                first_element.state = ProcessState::Running;
                self.ready_processes.push_front(first_element);
                SchedulingDecision::Run {
                    pid: Pid::new(self.get_running_process_pid()),
                    timeslice: NonZeroUsize::new(self.remaining_timeslice).unwrap(),
                }
            }
        // nu exista procese care pot fi planificate
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

            // verificare daca procesorul intra in starea de sleep
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

            // se ia primul proces din coada de ready si se planifica acesta
            self.remaining_timeslice = self.timeslice.get();
            let mut first_element = self.ready_processes.pop_front().unwrap();
            first_element.state = ProcessState::Running;
            self.ready_processes.push_front(first_element);
            SchedulingDecision::Run {
                pid: Pid::new(self.get_running_process_pid()),
                timeslice: NonZeroUsize::new(self.remaining_timeslice).unwrap(),
            }
        }
    }

    fn stop(&mut self, reason: StopReason) -> SyscallResult {
        match reason {
            StopReason::Syscall { syscall, remaining } => match syscall {
                Syscall::Fork(priority) => {
                    // creare nou proces cu urmatorul pid valabil si prioritatea primita
                    let new_process = ProcessControlBlock::new(self.next_pid, priority);

                    // incrementare urmatorul pid valid si adaugare in coada de ready
                    self.next_pid += 1;

                    // se verifica daca este primul proces creat
                    if self.ready_processes.is_empty() {
                        self.ready_processes.push_back(new_process.clone());
                        self.remaining_timeslice = remaining;
                    } else {
                        let mut running_process = self.ready_processes.pop_front().unwrap();

                        // actualizarea timpului de rulare si syscall pentru procesul curent
                        let execution_time = self.remaining_timeslice - remaining - 1;

                        running_process.syscall += 1;
                        running_process.execute += execution_time;

                        // readaugarea acestuia in coada si actualizarea timpului total pentru toate procesele
                        self.ready_processes.push_front(running_process);
                        self.remaining_timeslice = remaining;
                        self.actualise_timings(execution_time + 1);

                        // adaugarea noului proces in coada
                        self.ready_processes.push_back(new_process.clone());
                    }
                    SyscallResult::Pid(Pid::new(new_process.pid))
                }
                Syscall::Sleep(sleep_time) => {
                    if self.ready_processes.is_empty() {
                        return SyscallResult::NoRunningProcess;
                    }

                    // se extrage din coada de procese procesul care ruleaza
                    let mut running_process = self.ready_processes.pop_front().unwrap();

                    // se schimba starea acestuia din running -> waiting
                    running_process.state = ProcessState::Waiting { event: None };
                    running_process.sleep = sleep_time;

                    // se actualizeaza timpii aferente
                    let execution_time = self.remaining_timeslice - remaining - 1;
                    running_process.execute += execution_time;
                    running_process.total += execution_time + 1;
                    running_process.syscall += 1;

                    // se actualizeaza timpul total pentru toate procesele
                    self.actualise_timings(execution_time + 1);

                    self.waiting_processes.push_back(running_process);
                    SyscallResult::Success
                }
                Syscall::Signal(signal_code) => {
                    if self.ready_processes.is_empty() {
                        return SyscallResult::NoRunningProcess;
                    }

                    let mut running_process = self.ready_processes.pop_front().unwrap();

                    // se actualizeaza timpul de executie pentru procesul running
                    let execution_time = self.remaining_timeslice - remaining - 1;
                    running_process.syscall += 1;
                    running_process.execute += execution_time;

                    // se actualizeaza timpul total pentru toate procesele si se scot din coada
                    // de waiting procesele care asteptau semnalul 
                    self.ready_processes.push_front(running_process);
                    self.remaining_timeslice = remaining;
                    self.actualise_timings(execution_time + 1);
                    self.update_waiting_processes(signal_code);
                    SyscallResult::Success
                }
                Syscall::Wait(event_number) => {
                    if self.ready_processes.is_empty() {
                        return SyscallResult::NoRunningProcess;
                    }
                    let mut running_process = self.ready_processes.pop_front().unwrap();

                     // se actualizeaza timpul de executie pentru procesul running
                    let execution_time = self.remaining_timeslice - remaining - 1;
                    running_process.syscall += 1;
                    running_process.execute += execution_time;

                    // se schimba starea acestuia
                    running_process.state = ProcessState::Waiting {
                        event: Some(event_number),
                    };

                    self.waiting_processes.push_back(running_process);
                    self.remaining_timeslice = remaining;
                    self.actualise_timings(execution_time + 1);
                    SyscallResult::Success
                }
                Syscall::Exit => {
                    if self.ready_processes.is_empty() {
                        return SyscallResult::NoRunningProcess;
                    }

                    // se scoate din coada procesul running
                    let _running_process = self.ready_processes.pop_front().unwrap();
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

                let mut running_process = self.ready_processes.pop_front().unwrap();
                running_process.execute += self.remaining_timeslice;
                running_process.state = ProcessState::Ready;
                running_process.total += self.remaining_timeslice;
                self.actualise_timings(self.remaining_timeslice);

                // se adauga la finalul cozii de ready procesul
                self.ready_processes.push_back(running_process);

                SyscallResult::Success
            }
        }
    }

    // functie care returneaza o lista cu toate procesele din scheduler
    fn list(&mut self) -> Vec<&dyn Process> {
        self.ready_processes
            .iter()
            .chain(self.waiting_processes.iter())
            .map(|process| process as &dyn Process)
            .collect()
    }
}
