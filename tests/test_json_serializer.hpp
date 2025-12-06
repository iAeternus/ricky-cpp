#ifndef TEST_JSON_SERIALIZER_HPP
#define TEST_JSON_SERIALIZER_HPP

#include <utility>

#include "JsonSerializer.hpp"
#include "ricky_test.hpp"

namespace my::test::test_json_serializer {

struct Person : Object<Person> {
    using Self = Person;

    util::String name;
    i32 age;
    bool is_student;
    util::Vec<i32> scores;
    util::Dict<util::String, util::String> address;

    Person() = default;

    Person(util::String name, i32 age, bool is_student, const util::Vec<i32>& scores, const util::Dict<util::String, util::String>& address) :
            name(std::move(name)), age(age), is_student(is_student), scores(scores), address(address) {}

    [[nodiscard]] cmp_t __cmp__(const Self& other) const {
        return name.__cmp__(other.name);
    }
};

DEFINE_JSON_ADAPTER(Person);

auto it_works = []() {
    // Given
    Person p{"Ricky", 20, true, {100, 90, 80}, {{"city", "Beijing"}, {"street", "No.1"}}};
    util::String s = R"({"name":"Ricky","age":20,"is_student":true,"scores":[100,90,80],"address":{"city":"Beijing","street":"No.1"}})";
    io::JsonSerializer js;

    // When
    auto res = js.serialize(p); // TODO \u
    io::println(res);
    auto p2 = js.deserialize<Person>(s);

    // Then
    Assertions::assertEquals(p, p2);
};

auto test_json_serializer() {
    UnitTestGroup group{"test_json_serializer"};

    group.addTest("it_works", it_works);

    group.startAll();
}

} // namespace my::test::test_json_serializer

namespace my::io {

using test::test_json_serializer::Person;

auto to_json(Json& j, const Person& p) {
    j = Json::object(
        "name", p.name,
        "age", p.age,
        "is_student", p.is_student,
        "scores", p.scores,
        "address", p.address);
}

auto from_json(const Json& j, Person& p) {
    p.name = j["name"].into<util::String>();
    p.age = j["age"].into<i32>();
    p.is_student = j["is_student"].into<bool>();
    p.scores = j["scores"].into<util::Vec<i32>>();
    p.address = j["address"].into<util::Dict<util::String, util::String>>();
}

} // namespace my::io

#endif // TEST_JSON_SERIALIZER_HPP