// Course User - модели предметной области
// CourseRepository - виртуальный класс описывает CRUD операции
// FileCourseRepositry - реализация репозитория
// CourseService - проверка прав, валидации. Обновляет репозиторий
// ConsoleUI - меню в консоли
// Exception - Объявление ошибок
#include "ConsoleUI.hpp"
#include "FileCourseRepository.hpp"
#include <memory>

int main() {
    system("chcp 1251");
    auto repo = std::make_unique<FileCourseRepository>(
        "C:\\Users\\USER\\Desktop\\kursovaya\\App\\App\\courses.txt"
    );
    CourseService service(std::move(repo));

    // Пока что «жёстко» создаём пользователя-преподавателя
    User currentUser("u1", "Иван Иванов", User::Role::Teacher);

    // Передаём сервис и пользователя в UI
    ConsoleUI ui(service, currentUser);
    ui.run();
    return 0;
}

