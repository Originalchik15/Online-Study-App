#include "User.hpp"


User::User(const string& id, const string& name, Role role)
    : id_(id), name_(name), role_(role) {
}

const string& User::getId() const {
    return id_;
}

const string& User::getName() const {
    return name_;
}

User::Role User::getRole() const {
    return role_;
}

bool User::canCreateCourse() const {
    return role_ == Role::Teacher || role_ == Role::Admin;
}

bool User::canEditCourse(const Course& /*course*/) const {
    return canCreateCourse();
}

bool User::canDeleteCourse(const Course& /*course*/) const {
    return canCreateCourse();
}

bool User::canViewCourse(const Course& course) const {
    return course.getStatus() != Course::Status::Deleted;
}