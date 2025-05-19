#include "ConsoleUI.hpp"
#include <iostream>
#include <limits>
#include <algorithm>

ConsoleUI::ConsoleUI(CourseService& cs,
    RequestService& rs,
    const User& user)
    : courseService_(cs)
    , requestService_(rs)
    , currentUser_(user)
{
}


void ConsoleUI::run() {
    while (true) {
        showMainMenu();
        int choice;
        std::cout << "Выбор: ";
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Неверный ввод.\n";
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        switch (choice) {
        case 1: handleCreate(); break;
        case 2: handleEdit();   break;
        case 3: handleDelete(); break;
        case 4: handleView();   break;
        case 5: handleList();   break;
        case 6:
            handleListRequests();
            break;
        case 7: {
            auto rid = promptString("ID заявки для одобрения: ");
            requestService_.approve(std::stoi(rid), currentUser_);
            std::cout << "Заявка одобрена.\n";
            break;
        }
        case 8: {
            auto rid = promptString("ID заявки для отклонения: ");
            requestService_.reject(std::stoi(rid), currentUser_);
            std::cout << "Заявка отклонена.\n";
            break;
        }
        case 0: std::cout << "Выход...\n"; return;
        default: std::cout << "Неверный выбор.\n";
        }
        pause();
    }
}

void ConsoleUI::showMainMenu() const {
    std::cout << "\n=== Меню управления курсами ===\n"
        << "1. Создать курс / Сделать заявку (Teacher)\n"
        << "2. Редактировать курс / Сделать заявку (Teacher)\n"
        << "3. Удалить курс / Сделать заявку (Teacher)\n"
        << "4. Просмотреть детали курса\n"
        << "5. Список всех курсов\n";
    if (currentUser_.getRole() == User::Role::Admin) {
        std::cout << "6. Список заявок на обработку\n"
            << "7. Одобрить заявку\n"
            << "8. Отклонить заявку\n";
    }
    std::cout << "0. Выход\n";
}


void ConsoleUI::handleCreate() {
    try {
        auto title = promptString("Название: ");
        auto desc = promptString("Описание: ");

        if (currentUser_.getRole() == User::Role::Teacher) {
            // Учитель — только заявка
            requestService_.requestCreate(currentUser_, title, desc);
            std::cout << "Заявка на создание курса отправлена.\n";
        }
        else { // Admin и другие — прямое создание
            auto course = courseService_.createCourse(currentUser_, title, desc);
            std::cout << "Курс создан. ID = " << course.getId() << "\n";
        }
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

        if (currentUser_.getRole() == User::Role::Teacher) {
            requestService_.requestEdit(currentUser_, id, title, desc);
            std::cout << "Заявка на редактирование курса отправлена.\n";
        }
        else {
            courseService_.editCourse(currentUser_, id, title, desc);
            std::cout << "Курс обновлён.\n";
        }
    }
    catch (const std::exception& e) {
        std::cout << "Ошибка: " << e.what() << "\n";
    }
}


void ConsoleUI::handleDelete() {
    try {
        auto id = promptString("ID курса: ");

        if (currentUser_.getRole() == User::Role::Teacher) {
            requestService_.requestDelete(currentUser_, id);
            std::cout << "Заявка на удаление курса отправлена.\n";
        }
        else {
            courseService_.deleteCourse(currentUser_, id);
            std::cout << "Курс удалён.\n";
        }
    }
    catch (const std::exception& e) {
        std::cout << "Ошибка: " << e.what() << "\n";
    }
}


void ConsoleUI::handleView() {
    try {
        auto id = promptString("ID курса: ");
        auto c = courseService_.getCourseDetails(currentUser_, id);
        std::cout << "ID: " << c.getId() << "\n"
            << "Название: " << c.getTitle() << "\n"
            << "Описание: " << c.getDescription() << "\n"
            << "Статус: " << static_cast<int>(c.getStatus()) << "\n"
            << "Материалы:\n";
        for (auto& m : c.getMaterials())
            std::cout << " - " << m << "\n";
    }
    catch (const std::exception& e) {
        std::cout << "Ошибка: " << e.what() << "\n";
    }
}

void ConsoleUI::handleList() {
    auto list = courseService_.listAllCourses(currentUser_);
    if (list.empty()) {
        std::cout << "Нет доступных курсов.\n";
        return;
    }
    std::cout << "Список курсов:\n";
    for (auto& c : list)
        std::cout << c.getId() << ": " << c.getTitle()
        << " (" << static_cast<int>(c.getStatus()) << ")\n";
}

std::string ConsoleUI::promptString(const std::string& prompt) const {
    std::cout << prompt;
    std::string s;
    std::getline(std::cin, s);
    return s;
}

void ConsoleUI::pause() const {
    std::cout << "Нажмите ENTER для продолжения...";
    std::cin.get();
}
void ConsoleUI::handleListRequests() {
    try {
        auto pending = requestService_.listPending(currentUser_);
        if (pending.empty()) {
            std::cout << "Нет заявок на обработку.\n";
            return;
        }
        std::cout << "\n=== Список заявок ===\n";
        for (const auto& r : pending) {
            // распечатать ID, тип, target-course, новые данные, кто запросил
            std::cout << "ID=" << r.getId()
                << ", Тип=";
            switch (r.getType()) {
            case Request::Type::Create: std::cout << "Create"; break;
            case Request::Type::Edit:   std::cout << "Edit";   break;
            case Request::Type::Delete: std::cout << "Delete"; break;
            }
            if (r.getType() != Request::Type::Create) {
                std::cout << ", CourseID=" << r.getCourseId();
            }
            if (r.getType() == Request::Type::Create ||
                r.getType() == Request::Type::Edit) {
                std::cout << ", NewTitle=\"" << r.getNewTitle()
                    << "\", NewDesc=\"" << r.getNewDescription() << "\"";
            }
            std::cout << ", By=" << r.getRequestedBy()
                << ", Status=";
            switch (r.getStatus()) {
            case Request::State::Pending:  std::cout << "Pending";  break;
            case Request::State::Approved: std::cout << "Approved"; break;
            case Request::State::Rejected: std::cout << "Rejected"; break;
            }
            std::cout << "\n";
        }
    }
    catch (const std::exception& ex) {
        std::cout << "Ошибка при получении заявок: " << ex.what() << "\n";
    }
}