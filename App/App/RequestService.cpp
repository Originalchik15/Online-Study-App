#include "RequestService.hpp"
#include <stdexcept>
#include <chrono>

RequestService::RequestService(std::unique_ptr<RequestRepository> repo,
    CourseService& courseService)
    : repo_(std::move(repo))
    , courseService_(courseService)
{
}

void RequestService::authorizeTeacher(const User& user) const {
    if (user.getRole() != User::Role::Teacher) {
        throw std::runtime_error(
            "Only teachers may create requests");
    }
}

void RequestService::authorizeAdmin(const User& user) const {
    if (user.getRole() != User::Role::Admin) {
        throw std::runtime_error(
            "Only administrators may approve or reject requests");
    }
}

void RequestService::requestCreate(const User& teacher,
    const std::string& title,
    const std::string& description)
{
    authorizeTeacher(teacher);
    // Формируем новую заявку: courseId пуст, т.к. курс ещё не создан
    Request r(
        /* id */               0,
        /* courseId */         "",
        Request::Type::Create,
        /* newTitle */         title,
        /* newDescription */   description,
        /* requestedBy */      teacher.getId(),
        /* status */           Request::State::Pending,
        /* createdAt */        std::chrono::system_clock::now()
    );
    repo_->add(r);
}

void RequestService::requestEdit(const User& teacher,
    const std::string& courseId,
    const std::string& newTitle,
    const std::string& newDescription)
{
    authorizeTeacher(teacher);
    Request r(
        0,
        courseId,
        Request::Type::Edit,
        newTitle,
        newDescription,
        teacher.getId(),
        Request::State::Pending,
        std::chrono::system_clock::now()
    );
    repo_->add(r);
}

void RequestService::requestDelete(const User& teacher,
    const std::string& courseId)
{
    authorizeTeacher(teacher);
    Request r(
        0,
        courseId,
        Request::Type::Delete,
        "",      // нет новых данных
        "",
        teacher.getId(),
        Request::State::Pending,
        std::chrono::system_clock::now()
    );
    repo_->add(r);
}

std::vector<Request> RequestService::listPending(const User& admin) const {
    authorizeAdmin(admin);
    return repo_->findAllPending();
}

void RequestService::approve(int requestId, const User& admin) {
    authorizeAdmin(admin);

    // Получаем заявку
    Request r = repo_->findById(requestId);
    if (r.getStatus() != Request::State::Pending) {
        throw std::runtime_error("Request already processed");
    }

    // Выполняем операцию от имени администратора
    switch (r.getType()) {
    case Request::Type::Create:
        courseService_.createCourse(
            admin,
            r.getNewTitle(),
            r.getNewDescription()
        );
        break;

    case Request::Type::Edit:
        courseService_.editCourse(
            admin,
            r.getCourseId(),
            r.getNewTitle(),
            r.getNewDescription()
        );
        break;

    case Request::Type::Delete:
        courseService_.deleteCourse(
            admin,
            r.getCourseId()
        );
        break;
    }

    // Устанавливаем статус заявки и сохраняем
    r.setStatus(Request::State::Approved);
    repo_->update(r);
}

void RequestService::reject(int requestId, const User& admin) {
    authorizeAdmin(admin);

    Request r = repo_->findById(requestId);
    if (r.getStatus() != Request::State::Pending) {
        throw std::runtime_error("Request already processed");
    }

    r.setStatus(Request::State::Rejected);
    repo_->update(r);
}
