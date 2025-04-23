#include "FileCourseRepository.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdexcept>

namespace {
    std::string statusToString(Course::Status status) {
        switch (status) {
        case Course::Status::Draft:       return "Draft";
        case Course::Status::UnderReview: return "UnderReview";
        case Course::Status::Published:   return "Published";
        case Course::Status::Archived:    return "Archived";
        case Course::Status::Deleted:     return "Deleted";
        default:                          return "";
        }
    }

    Course::Status stringToStatus(const std::string& s) {
        if (s == "Draft")       return Course::Status::Draft;
        if (s == "UnderReview") return Course::Status::UnderReview;
        if (s == "Published")   return Course::Status::Published;
        if (s == "Archived")    return Course::Status::Archived;
        if (s == "Deleted")     return Course::Status::Deleted;
        throw std::runtime_error("Unknown course status: " + s);
    }
} 

FileCourseRepository::FileCourseRepository(const std::string& filepath)
    : filepath_(filepath) {
}

void FileCourseRepository::add(const Course& course) {
    auto courses = loadAll();

    auto it = std::find_if(courses.begin(), courses.end(),
        [&](const Course& c) { return c.getId() == course.getId(); });
    if (it != courses.end()) {
        throw std::runtime_error("Course with ID already exists: " + course.getId());
    }
    courses.push_back(course);
    saveAll(courses);
}

void FileCourseRepository::update(const Course& course) {
    auto courses = loadAll();
    bool found = false;
    for (auto& c : courses) {
        if (c.getId() == course.getId()) {
            c = course;
            found = true;
            break;
        }
    }
    if (!found) {
        throw std::runtime_error("Course not found: " + course.getId());
    }
    saveAll(courses);
}

void FileCourseRepository::remove(const std::string& courseId) {
    auto courses = loadAll();
    auto originalSize = courses.size();
    courses.erase(
        std::remove_if(courses.begin(), courses.end(),
            [&](const Course& c) { return c.getId() == courseId; }),
        courses.end());
    if (courses.size() == originalSize) {
        throw std::runtime_error("Course not found: " + courseId);
    }
    saveAll(courses);
}

Course FileCourseRepository::findById(const std::string& courseId) const {
    auto courses = loadAll();
    for (auto& c : courses) {
        if (c.getId() == courseId) {
            return c;
        }
    }
    throw std::runtime_error("Course not found: " + courseId);
}

std::vector<Course> FileCourseRepository::findAll() const {
    return loadAll();
}

std::vector<Course> FileCourseRepository::loadAll() const {
    std::vector<Course> courses;
    std::ifstream in(filepath_);
    if (!in.is_open()) {
        return courses;
    }
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        auto parts = split(line, '|');
        if (parts.size() < 5) continue;
        const auto& id = parts[0];
        const auto& title = parts[1];
        const auto& desc = parts[2];
        Course c(id, title, desc);
        auto st = stringToStatus(parts[3]);
        switch (st) {
        case Course::Status::Draft:       break;
        case Course::Status::UnderReview: c.submitForReview(); break;
        case Course::Status::Published:   c.submitForReview(); c.approve(); break;
        case Course::Status::Archived:    c.submitForReview(); c.approve(); c.archive(); break;
        case Course::Status::Deleted:     c.remove(); break;
        }
        auto mats = split(parts[4], ';');
        for (auto& m : mats) {
            if (!m.empty()) c.addMaterial(m);
        }
        courses.push_back(std::move(c));
    }
    return courses;
}

void FileCourseRepository::saveAll(const std::vector<Course>& courses) const {
    std::ofstream out(filepath_, std::ofstream::trunc);
    if (!out.is_open()) {
        throw std::runtime_error("Unable to open file: " + filepath_);
    }
    for (const auto& c : courses) {
        out << c.getId() << '|'
            << c.getTitle() << '|'
            << c.getDescription() << '|'
            << statusToString(c.getStatus()) << '|'
            << join(c.getMaterials(), ';')
            << "\n";
    }
}

std::vector<std::string> FileCourseRepository::split(const std::string& s, char delimiter) {
    std::vector<std::string> parts;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delimiter)) {
        parts.push_back(item);
    }
    return parts;
}

std::string FileCourseRepository::join(const std::vector<std::string>& parts, char delimiter) {
    std::string result;
    for (size_t i = 0; i < parts.size(); ++i) {
        result += parts[i];
        if (i + 1 < parts.size()) result += delimiter;
    }
    return result;
}
