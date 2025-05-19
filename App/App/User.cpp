#include "User.hpp"
#include <stdexcept>
// конструктор, геттеры
User::User(const std::string& id, const std::string& name, Role role)
	: id_(id), name_(name), role_(role) {
}
const std::string& User::getId()   const { return id_; }
const std::string& User::getName() const { return name_; }
User::Role          User::getRole() const { return role_; }

// права
bool User::canCreateCourse() const { return role_ == Role::Teacher || role_ == Role::Admin; }
bool User::canEditCourse(const Course&) const { return canCreateCourse(); }
bool User::canDeleteCourse(const Course&) const { return canCreateCourse(); }
bool User::canViewCourse(const Course& c) const { return c.getStatus() != Course::Status::Deleted; }

std::string User::roleToString(Role r) {
    switch (r) {
    case Role::Student: return "Student";
    case Role::Teacher: return "Teacher";
    case Role::Admin:   return "Admin";
    }
    // На всякий случай, если придёт неизвестный enum:
    throw std::invalid_argument("Unknown User::Role in roleToString");
}

User::Role User::stringToRole(const std::string& s) {
    if (s == "Student") return Role::Student;
    if (s == "Teacher") return Role::Teacher;
    if (s == "Admin")   return Role::Admin;
    // Если строка не соответствует ни одному значению, бросаем:
    throw std::invalid_argument("Invalid role string: " + s);
}