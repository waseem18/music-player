#ifndef MP_PYTHREAD_HPP
#define MP_PYTHREAD_HPP

#include <Python.h>
#include <pythread.h>

#include <boost/noncopyable.hpp>
#include <boost/function.hpp>


struct PyMutex {
	PyThread_type_lock l;
	bool enabled;
	PyMutex(); ~PyMutex();
	PyMutex(const PyMutex&) : PyMutex() {} // ignore
	PyMutex& operator=(const PyMutex&) { return *this; } // ignore
	void lock();
	bool lock_nowait();
	void unlock();
};

struct PyScopedLock : boost::noncopyable {
	PyMutex& mutex;
	PyScopedLock(PyMutex& m);
	~PyScopedLock();
};

struct PyScopedUnlock : boost::noncopyable {
	PyMutex& mutex;
	PyScopedUnlock(PyMutex& m);
	~PyScopedUnlock();
};

struct PyScopedGIL : boost::noncopyable {
	PyGILState_STATE gstate;
	PyScopedGIL() { gstate = PyGILState_Ensure(); }
	~PyScopedGIL() { PyGILState_Release(gstate); }
};

struct PyScopedGIUnlock : boost::noncopyable {
	PyScopedGIL gstate; // in case we didn't had the GIL
	PyThreadState* _save;
	PyScopedGIUnlock() : _save(NULL) { Py_UNBLOCK_THREADS }
	~PyScopedGIUnlock() { Py_BLOCK_THREADS }
};


struct PyThread : boost::noncopyable {
	PyMutex lock;
	bool running;
	bool stopSignal;
	boost::function<void(PyMutex& lock, bool& stopSignal)> func;
	long ident;
	PyThread(); ~PyThread();
	bool start();
	void wait();
	void stop();
};

extern "C" {
// this is defined in <sys/mman.h>. systems which don't have that should provide a dummy/wrapper
int mlock(const void *addr, size_t len);
}


#endif // PYTHREAD_HPP
