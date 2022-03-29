#include <atomic>

class mcs_lock {
public:
    class scoped_lock;

private:
    std::atomic<scoped_lock *> _tail;

public:
    // Запрет конструктора копирования
    mcs_lock(const mcs_lock &) = delete;
    // Запрет операции копирования
    mcs_lock &operator=(const mcs_lock &) = delete;
    // Начальная инициализация _tail
    mcs_lock() : _tail(nullptr) {}

    class scoped_lock {
        mcs_lock &_lock;
        volatile scoped_lock *_next;
        volatile bool _owned;

    public:
        // Запрет конструктора копирования
        scoped_lock(const scoped_lock &) = delete;
        // Запрет операции копирования
        scoped_lock &operator=(const scoped_lock &) = delete;

        // Конструктор
        explicit scoped_lock(mcs_lock &lock) :
                _lock(lock), _next(nullptr), _owned(false) {
            auto tail = _lock._tail.exchange(this);
            // Поток становится последним в очередь
            // Если в очереди есть поток перед,
            // то ожидание пока поток не разблокируют
            if (tail != nullptr) {
                tail->_next = this;
                while (!_owned) {
                }
            }
        }
        // Деструктор
        ~scoped_lock() {
            scoped_lock *tail = this;
            if (!_lock._tail.compare_exchange_strong(tail, nullptr)) {
                // Поток единственный в очереди, жидание появления следующего потока
                while (_next == nullptr) {
                }
                // Разблокировка следующего потока
                _next->_owned = true;
            }
        }
    };
};