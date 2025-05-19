// main.cpp
#include <iostream>
#include <string>
#include "ConsoleUI.hpp"
#include "CourseService.hpp"
#include "RequestService.hpp"
#include "PgCourseRepository.hpp"
#include "PgRequestRepository.hpp"
#include "PgUserRepository.hpp"
#include "AuthenticationService.hpp"

int main() {

    system("chcp 1251");


    const std::string connInfo =
        "host=localhost port=5432 dbname=cources "
        "user=postgres password=123321 client_encoding=WIN1251";


    auto courseRepo = std::make_unique<PgCourseRepository>(connInfo);
    auto requestRepo = std::make_unique<PgRequestRepository>(connInfo);
    auto userRepo = std::make_unique<PgUserRepository>(connInfo);

    AuthenticationService authSvc(*userRepo);

    std::string username, password;
    std::cout << "Логин: ";
    std::getline(std::cin, username);
    std::cout << "Пароль: ";
    std::getline(std::cin, password);

    User currentUser = authSvc.login(username, password);

    CourseService  courseSvc(std::move(courseRepo));
    RequestService requestSvc(std::move(requestRepo), courseSvc);

    ConsoleUI ui(courseSvc, requestSvc, currentUser);
    ui.run();

    return 0;
}