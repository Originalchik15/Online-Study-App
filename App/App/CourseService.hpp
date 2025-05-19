#pragma once

#include <memory>
#include <vector>
#include <string>
#include "CourseRepository.hpp"
#include "Course.hpp"
#include "User.hpp"

class CourseService {
public:
    explicit CourseService(std::unique_ptr<CourseRepository> repo);

    Course createCourse(const User& user,
        const std::string& title,
        const std::string& description);
    void editCourse(const User& user,
        const std::string& courseId,
        const std::string& newTitle,
        const std::string& newDescription);
    void deleteCourse(const User& user,
        const std::string& courseId);
    Course getCourseDetails(const User& user,
        const std::string& courseId);
    std::vector<Course> listAllCourses(const User& user);

private:
    std::unique_ptr<CourseRepository> repo_;

    void authorizeCreate(const User& user);
    void authorizeEdit(const User& user, const Course& course);
    void authorizeDelete(const User& user, const Course& course);
    void authorizeView(const User& user, const Course& course);

    void validateCourseData(const std::string& title,
        const std::string& description);
};
