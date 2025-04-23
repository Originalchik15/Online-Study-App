#pragma once
#include <string>
#include <stdexcept>
#include "Course.hpp"

class User {
public:
	enum class Role{ Student, Teacher, Admin};

	User(const string& id, const string& name, Role role);
	
	const string& getId() const;
	const string& getName() const;
	Role getRole() const;

	bool canCreateCourse() const;
	bool canEditCourse(const Course& course) const;
	bool canDeleteCourse(const Course& course) const;
	bool canViewCourse(const Course& course) const;
private:
	string id_;
	string name_;
	Role role_;
};