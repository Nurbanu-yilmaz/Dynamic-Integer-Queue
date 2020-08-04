#include <iostream>
#include <random>
#include <mutex>
#include <thread>
#include <chrono>
#include <iomanip>
#include <ctime>
#include "HW8DynIntQueue.h"

using namespace std;

mutex m1;
mutex m2;
HW8DynIntQueue queue;
bool done;

int random_range(const int & min, const int & max) {
	static mt19937 generator(time(0));
	uniform_int_distribution<int> distribution(min, max);
	return distribution(generator);
}

void Customer(int arguments[]) {
	int id = 1;
	while (id <= arguments[0])
	{
		m1.lock();
		queue.enqueue(id);
		int size = queue.getCurrentSize();
		m1.unlock();
		m2.lock();
		time_t time = chrono::system_clock::to_time_t(chrono::system_clock::now());
		cout << "New customer with ID " << id << " has arrived (queue size is " << size << "): " << put_time(localtime(&time), "%T") << endl;
		m2.unlock();
		this_thread::sleep_for(chrono::seconds(random_range(arguments[2], arguments[3]))); //do shopping
		id++;
	}
	done = true;
}

void Cashier(int id, int arguments[]) {
	int customer;
	while (!done || queue.getCurrentSize() > 0)
	{
		m1.lock();
		int size = queue.getCurrentSize();
		if (id == 1 && size > 0)
		{
			queue.dequeue(customer);
			m1.unlock();
			m2.lock();
			time_t time = chrono::system_clock::to_time_t(chrono::system_clock::now());
			cout << "Cashier " << id << " started transaction with customer " << customer << " (queue size is " << size - 1 << "): " << put_time(localtime(&time), "%T") << endl;
			m2.unlock();
			this_thread::sleep_for(chrono::seconds(random_range(arguments[4], arguments[5]))); //deal with customer
			m2.lock();
			time = chrono::system_clock::to_time_t(chrono::system_clock::now());
			cout << "Cashier " << id << " finished transaction with customer " << customer << " " << put_time(localtime(&time), "%T") << endl;
			m2.unlock();
		}
		else if (id == 2 && size >= arguments[1])
		{
			queue.dequeue(customer);
			m1.unlock();
			m2.lock();
			time_t time = chrono::system_clock::to_time_t(chrono::system_clock::now());
			cout << "Cashier " << id << " started transaction with customer " << customer << " (queue size is " << size - 1 << "): " << put_time(localtime(&time), "%T") << endl;
			m2.unlock();
			this_thread::sleep_for(chrono::seconds(random_range(arguments[4], arguments[5]))); //deal with customer
			m2.lock();
			time = chrono::system_clock::to_time_t(chrono::system_clock::now());
			cout << "Cashier " << id << " finished transaction with customer " << customer << " " << put_time(localtime(&time), "%T") << endl;
			m2.unlock();
		}
		else
		{
			m1.unlock();
		}
	}
}

int main() {
	int arguments[6];
	cout << "Please enter the total number of customers: ";
	cin >> arguments[0];
	cout << "Please enter the number of customers waiting in the queue to open the second cashier: ";
	cin >> arguments[1];
	cout << "Please enter the inter-arrival time range between two customers: " << endl;
	cout << "Min: ";
	cin >> arguments[2];
	cout << "Max: ";
	cin >> arguments[3];
	cout << "Please enter the checkout time range of cashiers: " << endl;
	cout << "Min: ";
	cin >> arguments[4];
	cout << "Max: ";
	cin >> arguments[5];
	done = false;
	time_t time = chrono::system_clock::to_time_t(chrono::system_clock::now());
	cout << "Simulation starts " << put_time(localtime(&time), "%T") << endl;
	thread cashier1 = thread(Cashier, 1, arguments);
	thread cashier2 = thread(Cashier, 2, arguments);
	thread customer = thread(Customer, arguments);
	customer.join();
	cashier1.join();
	cashier2.join();
	time = chrono::system_clock::to_time_t(chrono::system_clock::now());
	cout << "End of the simulation ends: " << put_time(localtime(&time), "%T") << endl;
	return 0;
}