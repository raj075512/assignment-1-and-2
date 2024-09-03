#include <iostream>
#include <queue>
#include <chrono>
#include <mutex>
#include<bits/stdc++.h>
using namespace std;

class SlidingWindowRateLimiter {
public:
    SlidingWindowRateLimiter(int timeWindowInMilliseconds, int bucketCapacity)
        : timeWindowInMilliseconds(timeWindowInMilliseconds), bucketCapacity(bucketCapacity) {}

    bool grantAccess() {
        lock_guard<mutex> lock(mtx);
        long long currentTime = currentTimeMillis();
        checkAndUpdateQueue(currentTime);
        if (slidingWindow.size() < bucketCapacity) {
            slidingWindow.push(currentTime);
            return true;
        }
        return false;
    }

private:
    queue<long long> slidingWindow;
    int timeWindowInMilliseconds;
    int bucketCapacity;
    mutex mtx;

    long long currentTimeMillis() {
        return chrono::duration_cast<chrono::milliseconds>(
            chrono::system_clock::now().time_since_epoch()
        ).count();
    }

    void checkAndUpdateQueue(long long currentTime) {
        while (!slidingWindow.empty() && (currentTime - slidingWindow.front()) >= timeWindowInMilliseconds) {
            slidingWindow.pop();
        }
    }
};

int main() {
    // Example usage
    int timeWindowInMilliseconds ;
    int bucketCapacity ;

    cin>>timeWindowInMilliseconds>>bucketCapacity;


    SlidingWindowRateLimiter rateLimiter(timeWindowInMilliseconds, bucketCapacity);

    for (int i = 0; i < 10; ++i) {
        if (rateLimiter.grantAccess()) {
            cout << "Message " << i + 1 << ": Access granted" << endl;
        } else {
            cout << "Message " << i + 1 << ": Access denied" << endl;
        }
        this_thread::sleep_for(std::chrono::milliseconds(10)); // Simulate time delay between requests
    }

    return 0;
}