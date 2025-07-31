#ifndef TEST_JSON_SERIALIZER_HPP
#define TEST_JSON_SERIALIZER_HPP

#include "JsonSerializer.hpp"
#include "Assertions.hpp"
#include "UnitTest.hpp"

namespace my::test::test_json_serializer {

struct Person : public Object<Person> {
    using Self = Person;

    util::String name;
    i32 age;
    bool is_student;
    util::Vec<i32> scores;
    util::Dict<util::String, util::String> address;

    Person() {}

    Person(util::String name, i32 age, bool is_student, util::Vec<i32> scores, util::Dict<util::String, util::String> address) :
            name(name), age(age), is_student(is_student), scores(scores), address(address) {}

    [[nodiscard]] cmp_t __cmp__(const Self& other) const {
        return name.__cmp__(other.name);
    }
};

DEFINE_JSON_ADAPTER(Person);

fn it_works = []() {
    // Given
    Person p{"Ricky", 20, true, {100, 90, 80}, {{"city", "Beijing"}, {"street", "No.1"}}};
    io::JsonSerializer js;

    // When
    auto res = js.serialize(p);
    auto p2 = js.deserialize<Person>(res);

    // Then
    Assertions::assertEquals(p, p2);
};

fn test_json_serializer() {
    UnitTestGroup group{"test_json_serializer"};

    group.addTest("it_works", it_works);

    group.startAll();
}

} // namespace my::test::test_json_serializer

namespace my::io {

using test::test_json_serializer::Person;

fn to_json(Json& j, const Person& p) {
    j = Json::object(
        "name", p.name,
        "age", p.age,
        "is_student", p.is_student,
        "scores", p.scores,
        "address", p.address);
}

fn from_json(const Json& j, Person& p) {
    p.name = j["name"].as<util::String>();
    p.age = j["age"].as<i32>();
    p.is_student = j["is_student"].as<bool>();
    p.scores = j["scores"].as<util::Vec<i32>>();
    p.address = j["address"].as<util::Dict<util::String, util::String>>();
}

} // namespace my::io

#endif // TEST_JSON_SERIALIZER_HPP