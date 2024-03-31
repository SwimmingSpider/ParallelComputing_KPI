#include <iostream>
#include <thread>
#include <atomic>
#include <vector>
#include <limits>
#include <random>



std::vector<int> generateRandomInts(int size, int min = std::numeric_limits<int>::min(), int max = std::numeric_limits<int>::max()) {

    if (size <= 0) {
        throw std::invalid_argument("Size must be positive");
    }

    std::vector<int> result;
    result.reserve(size);

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<int> dist(min, max);

    for (int i = 0; i < size; ++i) {
        result.push_back(dist(gen));
    }

    return result;
}
void findDivisibleBy19(const std::vector<int>& data, std::atomic<int>& count, std::atomic<int>& minVal, int start, int end) {
    int localCount = 0;
    int localMin = std::numeric_limits<int>::max();

    for (int i = start; i < end; ++i) {
        if (data[i] % 19 == 0) {
            ++localCount;
            localMin = std::min(localMin, data[i]);
        }
    }

    int expectedCount = count.load();
    while (!count.compare_exchange_weak(expectedCount, expectedCount + localCount)) {

    }

    int expectedMin = minVal.load();
    while (localMin < expectedMin && !minVal.compare_exchange_weak(expectedMin, localMin)) {

    }
}

int main() {
    std::vector<int> data = generateRandomInts(1000000);
    std::atomic<int> count(0);
    std::atomic<int> minVal(std::numeric_limits<int>::max());

    int numThreads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;

    int chunkSize = data.size() / numThreads;
    for (int i = 0; i < numThreads; ++i) {
        int start = i * chunkSize;
        int end = (i + 1 == numThreads) ? data.size() : (i + 1) * chunkSize;
        threads.emplace_back(findDivisibleBy19, std::ref(data), std::ref(count), std::ref(minVal), start, end);
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Count of elements divisible by 19: " << count.load() << "\n";
    std::cout << "Minimum element divisible by 19: " << minVal.load() << "\n";

    return 0;
}