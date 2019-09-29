#include <algorithm>
#include <functional>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

/******************************************************************************
 *              set            unordered_set   map            unordered_map   *
 * insert       O(log n)       O(1)            O(log n)       O(1)            *
 * lookup       O(log n)       O(1)            O(log n)       O(1)            *
 * sorted       O(n)           O(n log n)      O(n)           O(n log n)      *
 ******************************************************************************/

/* The two main options for storing student data against their names is
 * map or unordered_map. unordered_map has the advantage that inserts/lookups
 * are constant time, while for map they are O(log n). However, returning all
 * data sorted by name and printing it is only linear time for map,
 * but O(n log n) for unordered_map. For sorting by other fields it would be
 * O(n log n) for both.
 *
 * I have therefore chosen to store the data in an unordered_map, for the
 * faster constant time lookups, and the fact that printing sorted data
 * for any field takes the same O(n log n) time.
 *
 * The unordered_map is from the student name to an index into a vector
 * which holds all the student data. This means accessing student data
 * is also constant time.
 */

typedef int Grade;
typedef std::array<Grade, 4> Grades;
typedef std::pair<std::string const, Grades> Student;
typedef std::unordered_map<std::string, Grades> Students;
typedef std::function<bool(Student const &, Student const &)> Order;
typedef std::function<bool(Student const &)> Predicate;

std::size_t
subject_map(std::string &subject)
{
	if (subject == "Biology") {
		return 0;
	} else if (subject == "Chemistry") {
		return 1;
	} else if (subject == "Mathematics") {
		return 2;
	} else if (subject == "Physics") {
		return 3;
	}
	return -1;
}

auto
find_student(Students &students, std::string &student_name)
{
	auto iterator = students.find(student_name);
	if (iterator == students.end()) {
		/* student_name not found in map */
		auto pair = std::make_pair(student_name, Grades{0, 0, 0, 0});
		iterator = students.insert(pair).first;
	}
	return iterator;
}

void
build_table(Students &students, char filename[])
{
	std::string student_name;
	std::string subject;
	Grade grade;
	std::size_t subject_index;
	std::ifstream file;
	file.open(filename);
	for (;;) {
		file >> student_name;
		file >> subject;
		file >> grade;
		if (file.eof())
			break;
		subject_index = subject_map(subject);
		auto iterator = find_student(students, student_name);
		iterator->second[subject_index] = grade;
	}
	/* File closed automatically when variable `file' goes out of scope. */
}

int
total_grade(Grades const &grades)
{
	int total = 0;
	total += grades[0];
	total += grades[1];
	total += grades[2];
	total += grades[3];
	return total;
}

void
print_data(std::vector<Student const *> order, bool total = false)
{
	for (auto iter : order) {
		std::cout << iter->first;
		std::cout << " " << iter->second[0];
		std::cout << " " << iter->second[1];
		std::cout << " " << iter->second[2];
		std::cout << " " << iter->second[3];
		if (total) {
			std::cout << " " << total_grade(iter->second);
		}
		std::cout << std::endl;
	}
}

auto
sorted_data(Students const &students, Order order)
{
	std::vector<Student const *> ptrs;
	for (auto iter = students.begin(); iter != students.end(); ++iter) {
		ptrs.push_back(&*iter);
	}
	std::sort(ptrs.begin(), ptrs.end(),
	          [&] (auto a, auto b) {
	              return order(*a, *b); });
	return ptrs;
}

void
print_students(Students const &students)
{
	auto order = sorted_data(students,
	    [&] (auto a, auto b) {
	        return a.first < b.first; });
	print_data(order);
}

void
print_subject(Students const &students, int subject)
{
	auto order = sorted_data(students,
	    [&] (auto a, auto b) {
	        return a.second[subject] > b.second[subject]; });
	print_data(order);
}

void
print_total(Students const &students)
{
	auto order = sorted_data(students,
	    [&] (auto a, auto b) {
	        return total_grade(a.second) > total_grade(b.second); });
	print_data(order, true);
}

int
main(int argc, char *argv[])
{
	(void) argc;
	std::unordered_map<std::string, Grades> students;
	std::string input, command;
	build_table(students, argv[1]);
	for (;;) {
		std::cout << "> ";
		std::getline(std::cin, input);
		auto sstream = std::istringstream(input);
		sstream >> command;
		if (command == "students") {
			print_students(students);
		} else if (command == "biology") {
			print_subject(students, 0);
		} else if (command == "chemistry") {
			print_subject(students, 1);
		} else if (command == "maths") {
			print_subject(students, 2);
		} else if (command == "physics") {
			print_subject(students, 3);
		} else if (command == "total") {
			print_total(students);
		}
	}
	return 0;
}
