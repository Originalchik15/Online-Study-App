#pragma once

#include <vector>
#include <string>
#include "Course.hpp"

class CourseRepository {
public:
    virtual ~CourseRepository() = default;

    virtual void add(const Course& course) = 0;
    virtual void update(const Course& course) = 0;
    virtual void remove(const string& courseId) = 0;

    virtual Course findById(const string& courseId) const = 0;

    virtual std::vector<Course> findAll() const = 0;
};