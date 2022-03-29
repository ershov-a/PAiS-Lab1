#include <atomic>
#include <algorithm>
#include <functional>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <chrono>
#include <list>
#include <set>
#include "mcs_lock.h"

// spinner для инкремента
template<class lock, class lock_object>
void spinner_inc(lock *lck, int *val, std::size_t n) {
    for (std::size_t i = 0; i < n; ++i) {
        // Блокировка при создании (в конструкторе)
        lock_object lk(*lck);
        (*val)++;
        // Разблокировка при удалении (деструктор)
    }
}

// spinner для list
template<class lock, class lock_object>
void spinner_list(lock *lck, std::list<int> *val, std::size_t n) {
    for (std::size_t i = 0; i < n; ++i) {
        // Блокировка при создании (в конструкторе)
        lock_object lk(*lck);
        (*val).push_back(i);
        // Разблокировка при удалении (деструктор)
    }
}

// spinner для map
template<class lock, class lock_object>
void spinner_set(lock *lck, std::set<int> *val, std::size_t n) {
    for (std::size_t i = 0; i < n; ++i) {
        // Блокировка при создании (в конструкторе)
        lock_object lk(*lck);
        (*val).insert(i);
        // Разблокировка при удалении (деструктор)
    }
}

template<class lock, class lock_object>
void lock_test(std::size_t nthreads, int cnt) {
    // Массив потоков
    std::vector<std::thread> threads;
    // Lock и значение, которое инкременируется
    lock lck;
    // Переменная для инкремента
    int value = 0;
    // Переменная для list
    std::list<int> value_list;
    // Переменная для set
    std::set<int> value_set;

    // Начало отсчета времени работы
    auto begin = std::chrono::steady_clock::now();
    /*
     * Генерирует потоки, добавляя их в вектор threads "с конца":
     *   std::back_inserter - добавление с конца,
     *   nthreads - количество потоков,
     *   генератор создает поток, запускающий spinner_inc с заданным числом инкрементов.
     * * */

    // Генератор для инкремента
/*    std::generate_n(std::back_inserter(threads), nthreads,
                    [&lck, &value, cnt]() {
                        return std::move(std::thread(&spinner_inc<lock, lock_object>, &lck, &value, cnt));
                    });*/

    // Генератор для list
/*    std::generate_n(std::back_inserter(threads), nthreads,
                    [&lck, cnt, &value_list]() {
                        return std::move(std::thread(&spinner_list<lock, lock_object>, &lck, &value_list, cnt));
                    });*/

    // Генератор для set
    std::generate_n(std::back_inserter(threads), nthreads,
                    [&lck, cnt, &value_set]() {
                        return std::move(std::thread(&spinner_set<lock, lock_object>, &lck, &value_set, cnt));
                    });


    // Ожидание завершения работы потоков
    std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
    // Конец отсчета времени
    auto end = std::chrono::steady_clock::now();
    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);

    // Вывод результата
    std::cout << nthreads << ", " << elapsed_ms.count() << " ms" << std::endl;
}

int main() {
    // Количество параллельных потоков, поддерживаемое системой
    int nthreads = std::thread::hardware_concurrency();
    // Количество итераций потоке
    int cnt = 10000000;

    std::cout << "Number of hardware thread contexts on machine " << nthreads << std::endl;
    std::cout << "Operations count " << cnt << std::endl;

    // Бенчмарк std::mutex
    std::cout << "*** std::mutex lock_test ***" << std::endl;
    std::cout << "thread, exec time" << std::endl;
    for (std::size_t i = 1; i <= nthreads; ++i) {
        lock_test<std::mutex, std::unique_lock<std::mutex> >(i, cnt);
    }

    // Бенчмарк MCS
    std::cout << "***  mcs_lock lock_test ***" << std::endl;
    std::cout << "thread, exec time" << std::endl;
    for (std::size_t i = 1; i <= nthreads; ++i) {
        lock_test<mcs_lock, mcs_lock::scoped_lock>(i, cnt);
    }

    return 0;
}
