#include "PgCourseRepository.hpp"
#include <stdexcept>
#include <sstream>

// Статусы ↔ строки
std::string PgCourseRepository::statusToString(Course::Status st) {
    switch (st) {
    case Course::Status::Draft:       return "Draft";
    case Course::Status::UnderReview: return "UnderReview";
    case Course::Status::Published:   return "Published";
    case Course::Status::Archived:    return "Archived";
    case Course::Status::Deleted:     return "Deleted";
    }
    return "";
}
Course::Status PgCourseRepository::stringToStatus(const std::string& s) {
    if (s == "Draft")       return Course::Status::Draft;
    if (s == "UnderReview") return Course::Status::UnderReview;
    if (s == "Published")   return Course::Status::Published;
    if (s == "Archived")    return Course::Status::Archived;
    if (s == "Deleted")     return Course::Status::Deleted;
    throw std::runtime_error("Unknown status: " + s);
}

PgCourseRepository::PgCourseRepository(const std::string& connInfo)
    : conn_(PQconnectdb(connInfo.c_str()))
{
    if (PQstatus(conn_) != CONNECTION_OK) {
        std::string err = PQerrorMessage(conn_);
        PQfinish(conn_);
        throw std::runtime_error("Postgres connect failed: " + err);
    }
    if (PQsetClientEncoding(conn_, "WIN1251") != 0) {
        std::string err = PQerrorMessage(conn_);
        PQfinish(conn_);
        throw std::runtime_error("Failed to set client encoding: " + err);
    }
    initSchema();
}

PgCourseRepository::~PgCourseRepository() {
    if (conn_) PQfinish(conn_);
}

void PgCourseRepository::initSchema() {
    exec(R"(
      CREATE TABLE IF NOT EXISTS courses (
        id          VARCHAR(64) PRIMARY KEY,
        title       TEXT        NOT NULL,
        description TEXT        NOT NULL,
        status      VARCHAR(16) NOT NULL
      );
      CREATE TABLE IF NOT EXISTS materials (
        course_id VARCHAR(64) REFERENCES courses(id) ON DELETE CASCADE,
        path      TEXT        NOT NULL
      );
    )");
}

void PgCourseRepository::exec(const std::string& sql) const {
    PGresult* res = PQexec(conn_, sql.c_str());
    auto stat = PQresultStatus(res);
    if (stat != PGRES_COMMAND_OK && stat != PGRES_TUPLES_OK) {
        std::string err = PQerrorMessage(conn_);
        PQclear(res);
        throw std::runtime_error("SQL error: " + err);
    }
    PQclear(res);
}

std::string PgCourseRepository::escape(const std::string& s) const {
    char* buf = PQescapeLiteral(conn_, s.c_str(), s.size());
    if (!buf) throw std::runtime_error("Escape failed");
    std::string out(buf);
    PQfreemem(buf);
    return out;  // уже в кавычках
}

void PgCourseRepository::add(const Course& course) {
    std::ostringstream sql;
    sql << "INSERT INTO courses(id, title, description, status) VALUES ("
        << escape(course.getId()) << ", "
        << escape(course.getTitle()) << ", "
        << escape(course.getDescription()) << ", "
        << escape(statusToString(course.getStatus())) << ")";
    exec(sql.str());

    for (auto& m : course.getMaterials()) {
        std::ostringstream ms;
        ms << "INSERT INTO materials(course_id, path) VALUES ("
            << escape(course.getId()) << ", "
            << escape(m) << ")";
        exec(ms.str());
    }
}

void PgCourseRepository::update(const Course& course) {
    std::ostringstream sql;
    sql << "UPDATE courses SET "
        << "title=" << escape(course.getTitle()) << ", "
        << "description=" << escape(course.getDescription()) << ", "
        << "status=" << escape(statusToString(course.getStatus()))
        << " WHERE id=" << escape(course.getId());
    exec(sql.str());

    exec("DELETE FROM materials WHERE course_id=" + escape(course.getId()));
    for (auto& m : course.getMaterials()) {
        std::ostringstream ms;
        ms << "INSERT INTO materials(course_id, path) VALUES ("
            << escape(course.getId()) << ", "
            << escape(m) << ")";
        exec(ms.str());
    }
}

void PgCourseRepository::remove(const std::string& courseId) {
    exec("DELETE FROM courses WHERE id=" + escape(courseId));
}

Course PgCourseRepository::findById(const std::string& courseId) const {
    PGresult* res = PQexec(conn_,
        ("SELECT title,description,status FROM courses WHERE id=" +
            escape(courseId)).c_str());
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::string err = PQerrorMessage(conn_);
        PQclear(res);
        throw std::runtime_error("Query failed: " + err);
    }
    if (PQntuples(res) == 0) {
        PQclear(res);
        throw std::runtime_error("Course not found: " + courseId);
    }
    std::string title = PQgetvalue(res, 0, 0);
    std::string desc = PQgetvalue(res, 0, 1);
    std::string sts = PQgetvalue(res, 0, 2);
    PQclear(res);

    Course c(courseId, title, desc);
    c.setStatus(stringToStatus(sts));

    res = PQexec(conn_,
        ("SELECT path FROM materials WHERE course_id=" + escape(courseId)).c_str());
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::string err = PQerrorMessage(conn_);
        PQclear(res);
        throw std::runtime_error("Materials query failed: " + err);
    }
    for (int i = 0; i < PQntuples(res); ++i) {
        c.addMaterial(PQgetvalue(res, i, 0));
    }
    PQclear(res);
    return c;
}

std::vector<Course> PgCourseRepository::findAll() const {
    PGresult* res = PQexec(conn_, "SELECT id,title,description,status FROM courses");
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::string err = PQerrorMessage(conn_);
        PQclear(res);
        throw std::runtime_error("Query all failed: " + err);
    }
    std::vector<Course> list;
    int rows = PQntuples(res);
    for (int i = 0; i < rows; ++i) {
        std::string id = PQgetvalue(res, i, 0);
        std::string title = PQgetvalue(res, i, 1);
        std::string desc = PQgetvalue(res, i, 2);
        std::string sts = PQgetvalue(res, i, 3);
        Course c(id, title, desc);
        c.setStatus(stringToStatus(sts));
        list.push_back(std::move(c));
    }
    PQclear(res);
    return list;
}
