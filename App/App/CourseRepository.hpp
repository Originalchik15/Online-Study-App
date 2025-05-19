#pragma once

#include <vector>
#include <string>
#include "Course.hpp"

// Интерфейс репозитория курсов
class CourseRepository {
public:
    virtual ~CourseRepository() = default;

    virtual void add(const Course& course) = 0;
    virtual void update(const Course& course) = 0;
    virtual void remove(const std::string& courseId) = 0;
    virtual Course findById(const std::string& courseId) const = 0;
    virtual std::vector<Course> findAll() const = 0;
};
