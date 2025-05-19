#include "Course.hpp"
#include <stdexcept>

// Конструктор
Course::Course(const std::string& id,
    const std::string& title,
    const std::string& description)
    : id_(id)
    , title_(title)
    , description_(description)
    , status_(Status::Draft)
{
}

// Геттеры
const std::string& Course::getId() const { return id_; }
const std::string& Course::getTitle() const { return title_; }
const std::string& Course::getDescription() const { return description_; }
Course::Status Course::getStatus() const { return status_; }
const std::vector<std::string>& Course::getMaterials() const { return materials_; }

// Изменение полей
void Course::setTitle(const std::string& newTitle) { title_ = newTitle; }
void Course::setDescription(const std::string& newDescription) { description_ = newDescription; }
void Course::addMaterial(const std::string& materialPath) { materials_.push_back(materialPath); }

// Переходы статуса
void Course::submitForReview() {
    if (status_ != Status::Draft)
        throw std::logic_error("Course must be in Draft to submit for review");
    status_ = Status::UnderReview;
}
void Course::approve() {
    if (status_ != Status::UnderReview)
        throw std::logic_error("Course must be under review to approve");
    status_ = Status::Published;
}
void Course::reject() {
    if (status_ != Status::UnderReview)
        throw std::logic_error("Course must be under review to reject");
    status_ = Status::Draft;
}
void Course::archive() {
    if (status_ != Status::Published)
        throw std::logic_error("Course must be published to archive");
    status_ = Status::Archived;
}
void Course::restore() {
    if (status_ != Status::Archived)
        throw std::logic_error("Course must be archived to restore");
    status_ = Status::Published;
}
void Course::remove() {
    if (status_ != Status::Draft && status_ != Status::Archived)
        throw std::logic_error("Course must be in Draft or Archived to remove");
    status_ = Status::Deleted;
}

// Прямое задание статуса
void Course::setStatus(Status newStatus) {
    status_ = newStatus;
}
