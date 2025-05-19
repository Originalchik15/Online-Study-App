#pragma once

#include "CourseRepository.hpp"
#include "Course.hpp"
#include <libpq-fe.h>
#include <string>
#include <vector>

class PgCourseRepository : public CourseRepository {
public:
    // Формат connInfo:
    // "host=localhost port=5432 dbname=cources user=postgres password=..."
    explicit PgCourseRepository(const std::string& connInfo);
    ~PgCourseRepository() override;

    void add(const Course& course) override;
    void update(const Course& course) override;
    void remove(const std::string& courseId) override;
    Course findById(const std::string& courseId) const override;
    std::vector<Course> findAll() const override;

private:
    PGconn* conn_;

    void initSchema();
    void exec(const std::string& sql) const;
    std::string escape(const std::string& s) const;

    static std::string statusToString(Course::Status st);
    static Course::Status stringToStatus(const std::string& s);
};
