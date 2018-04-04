#include <iostream>
#include <thread>
#include <queue>
#include <list>
#include <ctime>
#include "time.h"
#include <random>
#include <mutex>

#define random(a,b) (rand()%(b-a+1)+a)

using namespace std;

int customerNo;
mutex threadLock;

class Customer {

private:
	int _id;
public:
	Customer(int id) {
		_id = id;
	}
	~Customer() {
	}

	int id() {
		return _id;
	}
};

void m_threadSleep(int sec, int nsec) {
	struct timespec sleepTime;
	struct timespec returnTime;
	sleepTime.tv_sec = sec;
	sleepTime.tv_nsec = nsec;
	nanosleep(&sleepTime, &returnTime);
}

Customer* nowServing;

class WaitingList {
private:
	int _size;
	queue<Customer*> _list;
public:
	WaitingList(int size = 0) {
		_size = size;
		_list = queue<Customer*>();
	};

	void enQueue(Customer* customer) {
		if (nowServing != nullptr)
			cout << "Barber cutting the hair of customer " << nowServing->id() << endl;
		else
			cout << "Barber sleeping" << endl;
		if (_list.size() < _size) {
			_list.push(customer);
			print();
		} else {
			print();
			cout << "Customer " << customer->id() << " leaving" << endl;
			delete customer;
		}
	}

	Customer* deQueue() {
		if (_list.size() == 0) return nullptr;
		Customer* customer = _list.front();
		_list.pop();
		return customer;
	}

	bool empty() {
		return _list.empty();
	}

	void print() {
		cout << "Waiting room: ";
		queue<Customer*> wait = queue<Customer*>(_list);
		while (!wait.empty()) {
			cout << wait.front()->id() << " ";
			wait.pop();
		}
		cout << endl;
	}



	~WaitingList() {
	};


};

WaitingList waitingList;

int getRandom(int n) {
	srand((unsigned)time(NULL));
	return random(1, n);
}



class Barber {
public:
	Barber() {
		nowServing = nullptr;
		while (true) {
			threadLock.lock();
			Customer* customer = waitingList.deQueue();
			if (customer != nullptr) {
				nowServing = customer;
				cout << "Barber cutting the hair of customer " << nowServing->id() << endl;
				waitingList.print();
				threadLock.unlock();
				m_threadSleep(getRandom(5),0);
				//Sleep(getRandom(5) * 1000);
				threadLock.lock();
				delete customer;
				nowServing = nullptr;
				if (waitingList.empty()) {
					cout << "Barber sleeping" << endl;
					waitingList.print();
				}
			}
			threadLock.unlock();
		}
	}
};

void startBarber() {
	Barber barber = Barber();
}

int main() {
	customerNo = 0;
	int waitingListSize;
	cout << "Input Waiting Room Size:";
	cin >> waitingListSize;
	waitingList = WaitingList(waitingListSize);
	thread b(startBarber);
	while (true) {
		threadLock.lock();
		waitingList.enQueue(new Customer(customerNo++));
		threadLock.unlock();
		m_threadSleep(3, 0);
	}
	return 0;
}