#pragma once

#include "CourseService.hpp"
#include "RequestService.hpp"
#include "User.hpp"
#include <string>

class ConsoleUI {
public:
    ConsoleUI(CourseService& courseSvc,
        RequestService& requestSvc,
        const User& currentUser);

    void run();

private:
    CourseService& courseService_;
    RequestService& requestService_;
    User currentUser_;

    void showMainMenu() const;
    void handleCreate();
    void handleEdit();
    void handleDelete();
    void handleView();
    void handleList();
    void handleListRequests();

    std::string promptString(const std::string& prompt) const;
    void pause() const;
};
