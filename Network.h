#ifndef NETWORK_H
#define NETWORK_H

#include <iostream>
#include <string>
#include <vector>

/**
 * @brief Базовий абстрактний клас для всіх мережевих пристроїв.
 */
class Device {
protected:
    /// @brief Унікальний числовий ідентифікатор пристрою.
    int id_;
    /// @brief Ідентифікаційне ім'я пристрою (наприклад, "R1", "H1").
    std::string name_;
public:
    /**
     * @brief Конструктор пристрою.
     * @param id Числовий ідентифікатор.
     * @param name Ідентифікаційне ім'я.
     */
    Device(int id, std::string name) : id_(id), name_(std::move(name)) {}
    virtual ~Device() = default;

    /// @brief Повертає ID пристрою.
    int id() const { return id_; }
    /// @brief Повертає ім'я пристрою.
    const std::string& name() const { return name_; }

    /**
     * @brief Чисто віртуальний метод, що повертає тип пристрою (динамічний поліморфізм).
     * @return std::string Рядок, що ідентифікує тип (наприклад, "Router", "Host").
     */
    virtual std::string kind() const = 0;
};

// ... (NetworkDevice, Router, Switch, Host - тут можна додати їхню документацію за аналогією)

/**
 * @brief Структура, що описує властивості каналу зв'язку.
 */
struct Link {
    /// @brief Затримка передачі, мілісекунди.
    double latencyMs{1.0};
    /// @brief Пропускна здатність, мегабіт/с.
    double bandwidthMbps{100.0};
    /// @brief Надійність каналу (0.0 до 1.0).
    double reliability{0.999};

    /**
     * @brief Обчислює загальну вартість (час у секундах) для передачі пакета вказаного розміру.
     * @param bytes Розмір корисного навантаження в байтах.
     * @return double Загальний час передачі в секундах (Latency + Payload Time).
     */
    double costForBytes(std::size_t bytes) const {
        double secondsLatency = latencyMs / 1000.0;
        double secondsPayload = (bandwidthMbps > 0.0)
            ? (static_cast<double>(bytes) * 8.0 / (bandwidthMbps * 1'000'000.0)) // 8 біт на байт
            : 1e9;
        return secondsLatency + secondsPayload;
    }
};

/**
 * @brief Клас, що представляє пакет даних, який передається мережею.
 */
class Packet {
    // ... (поля)
public:
    // ... (конструктор та геттери)

    /**
     * @brief Зменшує значення Time-To-Live (TTL) на 1.
     */
    void decTTL() { --ttl_; }

    /**
     * @brief Додає ім'я поточного вузла до історії проходження пакета.
     * @param nodeName Ім'я вузла, через який пройшов пакет.
     */
    void addHop(const std::string& nodeName) { hops_.push_back(nodeName); }
};

#endif //NETWORK_H