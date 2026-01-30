// This file implements a thread pool for managing concurrent execution of tasks, similar to the C++ thread pool.

use std::sync::{Arc, Mutex, OnceLock};
use std::thread;
use std::sync::mpsc;
// use std::time::Duration;

pub struct ThreadPool {
    workers: Vec<Worker>,
    sender: Option<mpsc::Sender<Job>>, // changed to Option
}

type Job = Box<dyn FnOnce() + Send + 'static>;

impl ThreadPool {
    // private constructor used only inside this module
    fn new_internal(size: usize) -> ThreadPool {
        let (sender, receiver) = mpsc::channel();
        let receiver = Arc::new(Mutex::new(receiver));
        
        let mut workers = Vec::with_capacity(size);
        for id in 0..size {
            workers.push(Worker::new(id, Arc::clone(&receiver)));
        }

        ThreadPool { workers, sender: Some(sender) } // wrap sender
    }

    pub fn execute<F>(&self, f: F)
    where
        F: FnOnce() + Send + 'static,
    {
        let job = Box::new(f);
        // use the sender inside the Option by reference
        self.sender.as_ref().expect("ThreadPool sender dropped").send(job).unwrap();
    }
}

// global singleton
static GLOBAL_POOL: OnceLock<Arc<ThreadPool>> = OnceLock::new();

/// Initialize the global pool (idempotent). Returns an Arc clone to the singleton.
pub fn init_global_pool(threads: usize) -> Arc<ThreadPool> {
    GLOBAL_POOL
        .get_or_init(|| Arc::new(ThreadPool::new_internal(threads.max(1))))
        .clone()
}

/// Get the global pool. Must call `init_global_pool` first (or it will be lazily created with 1 thread).
pub fn global_pool() -> Arc<ThreadPool> {
    GLOBAL_POOL
        .get_or_init(|| Arc::new(ThreadPool::new_internal(1)))
        .clone()
}

struct Worker {
    id: usize,
    thread: Option<thread::JoinHandle<()>>,
}

impl Worker {
    fn new(id: usize, receiver: Arc<Mutex<mpsc::Receiver<Job>>>) -> Worker {
        let thread = thread::spawn(move || loop {
            let job = receiver.lock().unwrap().recv().unwrap();
            println!("Worker {} got a job; executing.", id);
            job();
        });

        Worker {
            id,
            thread: Some(thread),
        }
    }
}

impl Drop for ThreadPool {
    fn drop(&mut self) {
        // take ownership of the sender and drop it so receivers see channel closed
        if let Some(sender) = self.sender.take() {
            drop(sender);
        }

        for worker in &mut self.workers {
            println!("Shutting down worker {}", worker.id);
            if let Some(thread) = worker.thread.take() {
                thread.join().unwrap();
            }
        }
    }
}