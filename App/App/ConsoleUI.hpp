#pragma once

#include "CourseService.hpp"

// Консольный интерфейс для взаимодействия с пользователем
class ConsoleUI {
public:
    explicit ConsoleUI(CourseService& service, const User& currentUser);

    // Запуск основного цикла приложения
    void run();

private:
    CourseService& service_;
    User currentUser_;
    // Отображение меню и обработка выбора
    void showMainMenu() const;
    void handleCreate();
    void handleEdit();
    void handleDelete();
    void handleView();
    void handleList();

    // Вспомогательные функции ввода
    std::string promptString(const std::string& prompt) const;
    void pause() const;
};
