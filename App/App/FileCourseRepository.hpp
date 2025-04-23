#pragma once

#include "CourseRepository.hpp"
#include "Course.hpp"
#include <string>
#include <vector>

//Поля '|', материалы ';'
class FileCourseRepository : public CourseRepository {
public:
    explicit FileCourseRepository(const string& filepath);
    ~FileCourseRepository() override = default;

    void add(const Course& course) override;
    void update(const Course& course) override;
    void remove(const string& courseId) override;
    Course findById(const string& courseId) const override;
    std::vector<Course> findAll() const override;

private:
    string filepath_;

    std::vector<Course> loadAll() const;
    void saveAll(const std::vector<Course>& courses) const;

    static std::vector<string> split(const string& s, char delimiter);
    static string join(const std::vector<string>& parts, char delimiter);
};
