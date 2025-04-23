#include "ConsoleUI.hpp"
#include <iostream>
#include <vector>

ConsoleUI::ConsoleUI(CourseService& service, const User& currentUser)
    : service_(service), currentUser_(currentUser) {
}

void ConsoleUI::run() {
    while (true) {
        showMainMenu();
        int choice;
        std::cout << "Выбор: ";
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Неверный ввод. Попробуйте снова.\n";
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
        case 1: handleCreate(); break;
        case 2: handleEdit();   break;
        case 3: handleDelete(); break;
        case 4: handleView();   break;
        case 5: handleList();   break;
        case 0: std::cout << "Выход...\n"; return;
        default: std::cout << "Неверный выбор.\n";
        }
        pause();
    }
}

void ConsoleUI::showMainMenu() const {
    std::cout << "\n=== Меню управления курсами ===\n";
    std::cout << "1. Создать курс\n";
    std::cout << "2. Редактировать курс\n";
    std::cout << "3. Удалить курс\n";
    std::cout << "4. Просмотреть детали курса\n";
    std::cout << "5. Список всех курсов\n";
    std::cout << "0. Выход\n";
}

void ConsoleUI::handleCreate() {
    try {
        auto title = promptString("Название: ");
        auto desc = promptString("Описание: ");
        auto course = service_.createCourse(currentUser_, title, desc);
        std::cout << "Курс успешно создан, его ID = "
            << course.getId() << "\n";
    }
    catch (const std::exception& e) {
        std::cout << "Ошибка: " << e.what() << "\n";
    }
}

void ConsoleUI::handleEdit() {
    try {
        auto id = promptString("ID курса: ");
        auto title = promptString("Новое название: ");
        auto desc = promptString("Новое описание: ");
        service_.editCourse(currentUser_, id, title, desc);
        std::cout << "Курс успешно обновлён.\n";
    }
    catch (const std::exception& e) {
        std::cout << "Ошибка: " << e.what() << "\n";
    }
}

void ConsoleUI::handleDelete() {
    try {
        auto id = promptString("ID курса: ");
        service_.deleteCourse(currentUser_, id);
        std::cout << "Курс удалён.\n";
    }
    catch (const std::exception& e) {
        std::cout << "Ошибка: " << e.what() << "\n";
    }
}

void ConsoleUI::handleView() {
    try {
        auto id = promptString("ID курса: ");
        auto c = service_.getCourseDetails(currentUser_, id);
        std::cout << "ID: " << c.getId() << "\n";
        std::cout << "Название: " << c.getTitle() << "\n";
        std::cout << "Описание: " << c.getDescription() << "\n";
        std::cout << "Статус: " << static_cast<int>(c.getStatus()) << "\n";
        std::cout << "Материалы:\n";
        for (const auto& m : c.getMaterials()) std::cout << " - " << m << "\n";
    }
    catch (const std::exception& e) {
        std::cout << "Ошибка: " << e.what() << "\n";
    }
}

void ConsoleUI::handleList() {
    auto courses = service_.listAllCourses(currentUser_);
    if (courses.empty()) {
        std::cout << "Нет доступных курсов.\n";
        return;
    }
    std::cout << "Список курсов:\n";
    for (const auto& c : courses) {
        std::cout << c.getId() << ": " << c.getTitle() << " (" << static_cast<int>(c.getStatus()) << ")\n";
    }
}

std::string ConsoleUI::promptString(const std::string& prompt) const {
    std::cout << prompt;
    std::string input;
    std::getline(std::cin, input);
    return input;
}

void ConsoleUI::pause() const {
    std::cout << "Нажмите ENTER для продолжения...";
    std::cin.get();
}
