// Request.cpp
#include "Request.hpp"

// Конструктор
Request::Request(int id,
    const std::string& courseId,
    Type type,
    const std::string& newTitle,
    const std::string& newDescription,
    const std::string& requestedBy,
    State status,
    std::chrono::system_clock::time_point createdAt)
    : id_(id)
    , courseId_(courseId)
    , type_(type)
    , newTitle_(newTitle)
    , newDescription_(newDescription)
    , requestedBy_(requestedBy)
    , status_(status)
    , createdAt_(createdAt)
{
}

// Геттеры
int Request::getId() const {
    return id_;
}

const std::string& Request::getCourseId() const {
    return courseId_;
}

Request::Type Request::getType() const {
    return type_;
}

const std::string& Request::getNewTitle() const {
    return newTitle_;
}

const std::string& Request::getNewDescription() const {
    return newDescription_;
}

const std::string& Request::getRequestedBy() const {
    return requestedBy_;
}

Request::State Request::getStatus() const {
    return status_;
}

std::chrono::system_clock::time_point Request::getCreatedAt() const {
    return createdAt_;
}

// Сеттер статуса
void Request::setStatus(State st) {
    status_ = st;
}
