#pragma once

#include <memory>
#include <vector>
#include <string>
#include "RequestRepository.hpp"
#include "CourseService.hpp"
#include "User.hpp"
#include "Request.hpp"

class RequestService {
public:
    // repo — репозиторий заявок, courseService — сервис работы с курсами
    RequestService(std::unique_ptr<RequestRepository> repo,
        CourseService& courseService);

    // Учитель создаёт заявку на операции с курсом
    void requestCreate(const User& teacher,
        const std::string& title,
        const std::string& description);
    void requestEdit(const User& teacher,
        const std::string& courseId,
        const std::string& newTitle,
        const std::string& newDescription);
    void requestDelete(const User& teacher,
        const std::string& courseId);

    // Админ просматривает и обрабRequestService.cppатывает
    std::vector<Request> listPending(const User& admin) const;
    void approve(int requestId, const User& admin);
    void reject(int requestId, const User& admin);

private:
    void authorizeTeacher(const User& user) const;
    void authorizeAdmin(const User& user) const;

    std::unique_ptr<RequestRepository> repo_;
    CourseService& courseService_;
};
