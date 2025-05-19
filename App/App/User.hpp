// User.hpp
#pragma once

#include <string>
#include "Course.hpp"

class User {
public:
    enum class Role { Student, Teacher, Admin };

    User(const std::string& id, const std::string& name, Role role);

    const std::string& getId() const;
    const std::string& getName() const;
    Role getRole() const;

    bool canCreateCourse() const;
    bool canEditCourse(const Course& course) const;
    bool canDeleteCourse(const Course& course) const;
    bool canViewCourse(const Course& course) const;

    // Новые методы для аутентификации
    static std::string roleToString(Role r);
    static Role        stringToRole(const std::string& s);

private:
    std::string id_;
    std::string name_;
    Role        role_;
};
