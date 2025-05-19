#include "CourseService.hpp"
#include <stdexcept>
#include <algorithm>
#include <iterator>
#include <vector>

CourseService::CourseService(std::unique_ptr<CourseRepository> repo)
    : repo_(std::move(repo)) {
}

Course CourseService::createCourse(const User& user,
    const std::string& title,
    const std::string& description) {
    authorizeCreate(user);
    validateCourseData(title, description);

    // 1. ѕолучаем все курсы и вычисл€ем следующий ID
    auto all = repo_->findAll();
    long nextId = 1;
    for (auto& c : all) {
        // предполагаем, что старые ID Ч это строки, содержащие целые числа
        try {
            long idNum = std::stol(c.getId());
            if (idNum >= nextId) nextId = idNum + 1;
        }
        catch (...) {
            // если не получилось конвертировать Ч пропускаем
        }
    }

    // 2. ‘ормируем строковый ID
    std::string id = std::to_string(nextId);

    // 3. —оздаЄм и сохран€ем курс
    Course course(id, title, description);
    repo_->add(course);
    return course;
}

void CourseService::editCourse(const User& user,
    const std::string& courseId,
    const std::string& newTitle,
    const std::string& newDescription) {
    Course existing = repo_->findById(courseId);
    authorizeEdit(user, existing);
    validateCourseData(newTitle, newDescription);
    existing.setTitle(newTitle);
    existing.setDescription(newDescription);
    repo_->update(existing);
}

void CourseService::deleteCourse(const User& user,
    const std::string& courseId) {
    Course existing = repo_->findById(courseId);
    authorizeDelete(user, existing);
    repo_->remove(courseId);
}

Course CourseService::getCourseDetails(const User& user,
    const std::string& courseId) {
    Course existing = repo_->findById(courseId);
    authorizeView(user, existing);
    return existing;
}

std::vector<Course> CourseService::listAllCourses(const User& user) {
    auto all = repo_->findAll();
    std::vector<Course> result;
    std::copy_if(all.begin(), all.end(), std::back_inserter(result),
        [&](const Course& c) { return user.canViewCourse(c); });
    return result;
}

void CourseService::authorizeCreate(const User& user) {
    if (!user.canCreateCourse())
        throw std::runtime_error("User not authorized to create courses");
}

void CourseService::authorizeEdit(const User& user, const Course& course) {
    if (!user.canEditCourse(course))
        throw std::runtime_error("User not authorized to edit this course");
}

void CourseService::authorizeDelete(const User& user, const Course& course) {
    if (!user.canDeleteCourse(course))
        throw std::runtime_error("User not authorized to delete this course");
}

void CourseService::authorizeView(const User& user, const Course& course) {
    if (!user.canViewCourse(course))
        throw std::runtime_error("User not authorized to view this course");
}

void CourseService::validateCourseData(const std::string& title,
    const std::string& description) {
    if (title.empty())
        throw std::runtime_error("Course title cannot be empty");
    if (description.empty())
        throw std::runtime_error("Course description cannot be empty");
}
